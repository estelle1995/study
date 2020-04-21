AQS 全称为 (AbstractQueuedSynchronizer), 这个类在 java.util.concurrent.locks 包下面

AQS 是一个用来构建锁和同步器的框架，使用 AQS 能简单且高效地构造出应用广泛的大量的同步器，比如我们提到的 ReentrantLock, Semaphore, 其他的诸如 ReentrantReadWriteLock, SynchronousQueue, FutureTask等等都是基于 AQS 的，当然，我们也能利用 AQS 非常轻松容易地构造出符合我们自己需求的同步器。

# AQS 原理
AQS 核心思想是，如果被请求的共享资源空闲，则将当前请求资源的线程设置为有效的工作线程，并且将共享资源设置为锁定状态。如果被请求的共享资源被占用，那么就需要一套线程阻塞等待以及被唤醒时锁分配的机制，这个机制 AQS 是用 CLH 队列锁实现的，即将暂时获取不到锁的线程加入到队列中。
![](assets/AQS实现原理.png)
CLH（Craig, Landin, and Hagersten）队列是一个虚拟的双向队列（虚拟的双向队列即不存在队列实例，仅存在节点之间的关联关系）。AQS 是将每条请求共享资源的线程封装成一个 CLH 锁队列的一个节点（Node）来实现锁的分配。

AQS 使用一个 int 成员变量来表示同步状态，通过内置的 FIFO 队列来完成获取资源线程的排队工作。AQS 使用 CAS 对该同步状态进行原子操作实现对其值的修改。

状态信息通过 protected 类型的 getState, setState, compareAndSetState 进行操作
```java
// 返回同步状态当前值
protected final int getState() {
    return state;
}
//设置同步状态的值
protected final void setState(int newState) {
    state = newState;
}
//原子的（CAS操作）将同步状态值设置为给定值 update  如果当前同步状态的值等于 expect(期望值)
protected final boolean compareAndSetState(int expect, int update) {
    return unsafe.compareAndSwapInt(this, stateOffset, expect, update);
}
```

# AQS 对资源的共享方式
## AQS 定义两种资源共享方式
1. EXCLUSIVE 独占
2. SHARED 共享

### 独占
只有一个线程能执行，如 ReentrantLock。又可分为公平锁和非公平锁,ReentrantLock 同时支持两种锁,下面以 ReentrantLock 对这两种锁的定义做介绍：

1. 公平锁：按照线程在队列中的排队顺序，先到者先拿到锁
2. 非公平锁：当线程要获取锁时，先通过两次 CAS 操作去抢锁，如果没抢到（线程尝试直接获取锁，线程入队前再次尝试获取锁），当前线程再加入到队列中等待唤醒
   
ReentrantLock 默认采用非公平锁，因为考虑获得更好的性能，通过 boolean 来决定是否用公平锁（传入 true 用公平锁）。
```java
private final Sync sync;
public ReentrantLock() {
    //默认非公平锁
    sync = new NonfairSync();
}
public ReentrantLock(boolean fair) {
    sync = fair ? new FairSync() : new NonfairSyn();
}
```
ReentrantLock 中公平锁的 lock 方法
```java
static final class FairSync extends Sync {
    final void lock() {
        acquire(1);
    }
    //AbstractQueuedSynchronizer 方法
    public final void acquire(int arg) {
        if (!tryAcquire(arg) && acquireQueued(addWaiter(Node.EXECLUSIVE), arg))
            selfInterrupt();
    }
    public final boolean tryAcquire(int acquires) {
        final Thread current = Thread.currentThread();
        int c = getState();
        if (c == 0) {
            //和非公平锁相比。这里多了一个判断：是否有线程再等待
            if (!hasQueuedPredecessors() && comapreAndSetState(0, acquires)) {
                setExclusiveOwnerThread(current);
                return true;
            }
        } else if (current == getExclusiveOwnerThread()) {
            int nextc = c + acquires;
            if (nextc < 0) throw new Error("Maximum lock count exceeded");
            setState(nextc);
            return true;
        }
        return false;
    }
}
```
ReentrantLock 非公平锁的 lock 方法：
```java
static final class NonfairSync extends Sync {
    final void lock() {
        if (compareAndSetState(0, 1)) setExclusiveOwnerThread(Thread.currentThread());
        else acquire(1);
    }
    protected final boolean tryAcquire(int acquires) {
        return nonfairTryAcquire(acquires);
    }
}
abstract static class Sync extends AbstractQueuedSynchronizer {
    final boolean nonfairTryAcquire(int acquires) {
        final Thread current = Thread.currentThread();
        int c = getState();
        if (c == 0) {
            //这里没有对阻塞队列进行判断
            if (compareAndSetState(0, acquires)) {
                setExclusiveOwnerThread(current);
                return true;
            }
        } else if (current == getExclusiveOwnerThread()) {
            int nextc = c + acquires;
            if (nextc < 0) throw new Error("Maximum lock count exceeded");
            setState(nextc);
            return true;
        }
        return false;
    }
}
```
公平锁与非公平锁只有两处不同
1. 非公平锁在调用 lock 后，首先就会调用 CAS 进行一次枪锁，如果这个时候恰巧锁没有被占用，那么直接就获取到锁返回了。
2. 非公平锁在 CAS 失败后，和公平锁一样都会进入到 tryAcquire 方法，在 tryAcquire 方法中，如果发现锁这个时候被释放了 (state == 0), 非公平锁会直接 CAS 抢锁，但是公平锁会判断等待队列是否有线程处于等待状态，如果有则不去抢锁。

公平锁和非公平锁就这两点区别，如果这两次 CAS 都不成功，那么后面非公平锁和公平锁是一样的，都要进入到阻塞队列等待唤醒。

相对来说，非公平锁会有更好的性能，因为它的吞吐量比较大。当然，非公平锁让获取锁的时间变得更加不确定，可能会导致在阻塞队列中的线程长期处于饥饿状态。

### 共享
多个线程可同时执行，如 Semaphore/CountDownLatch。 Semaphore、CountDownLatCh、 CyclicBarrier、ReadWriteLock 我们都会在后面讲到。

ReentrantReadWriteLock 可以看成是组合式，因为 ReentrantReadWriteLock 也就是读写锁允许多个线程同时对某一资源进行读。

不同的自定义同步器争用共享资源的方式也不同。自定义同步器在实现时只需要实现共享资源 state 的获取与释放方式即可，至于具体线程等待队列的维护（如获取资源失败入队/唤醒出队等），AQS 已经在上层已经帮我们实现好了。

## AQS 底层使用了模板方法模式
同步器的设计是基于模板方法模式的，如果需要自定义同步器一般的方式是这样（模板方法模式很经典的一个应用）：
1. 使用者继承 AbstractQueuedSychronizer 并重写指定的方法。（这些重写方法很简单，无非是对于共享资源 state 的获取和释放）
2. 将 AQS 组合在自定义同步组件的实现中，并调用其模板方法，而这些模板方法会调用使用者重写的方法。

模板方法模式是基于”继承“的，主要是为了在不改变模板结构的前提下在子类中重新定义模板中的内容以实现复用代码。举个很简单的例子假如我们要去一个地方的步骤是：购票buyTicket()->安检securityCheck()->乘坐某某工具回家ride()->到达目的地arrive()。我们可能乘坐不同的交通工具回家比如飞机或者火车，所以除了ride()方法，其他方法的实现几乎相同。我们可以定义一个包含了这些方法的抽象类，然后用户根据自己的需要继承该抽象类然后修改 ride()方法。

AQS 使用了模板方法模式，自定义同步器时需要重写下面几个 AQS 提供的模板方法：
1. isHeldExclusively() 该线程是否正在独占资源。只有用到 condition 才需要去实现它
2. tryAcquire(int) 独占方式。尝试获取资源，成功则返回 true， 失败则返回 false
3. tryRelease(int) 独占方式。尝试释放资源，成功则返回 true， 失败则返回 false
4. tryAcquireShared(int) 共享方式，尝试获取资源，负数表示失败；0表示成功，但没有剩余可用资源；正数表示成功，且有剩余资源。
5. tryReleaseSahred(int) 共享方式，尝试释放资源，成功则返回 true， 失败则返回 false

默认情况下，每个方法都抛出 UnsupportedOperationException。 这些方法的实现必须是内部线程安全的，并且通常应该简短而不是阻塞。AQS 类中的其他方法都是 final ，所以无法被其他类重写，只有这几个方法可以被其他类重写。

# Semapore (信号量)-允许多个线程同时访问
synchronized 和 ReentrantLock 都是一次只允许一个线程访问某个资源， Semaphore(信号量)可以指定多个线程同时访问某个资源。
```java
/**
 *
 * @author Snailclimb
 * @date 2018年9月30日
 * @Description: 需要一次性拿一个许可的情况
 */
public class SemaphoreExample1 {
  // 请求的数量
  private static final int threadCount = 550;

  public static void main(String[] args) throws InterruptedException {
    // 创建一个具有固定线程数量的线程池对象（如果这里线程池的线程数量给太少的话你会发现执行的很慢）
    ExecutorService threadPool = Executors.newFixedThreadPool(300);
    // 一次只能允许执行的线程数量。
    final Semaphore semaphore = new Semaphore(20);

    for (int i = 0; i < threadCount; i++) {
      final int threadnum = i;
      threadPool.execute(() -> {// Lambda 表达式的运用
        try {
          semaphore.acquire();// 获取一个许可，所以可运行线程数量为20/1=20
          test(threadnum);
          semaphore.release();// 释放一个许可
        } catch (InterruptedException e) {
          // TODO Auto-generated catch block
          e.printStackTrace();
        }

      });
    }
    threadPool.shutdown();
    System.out.println("finish");
  }

  public static void test(int threadnum) throws InterruptedException {
    Thread.sleep(1000);// 模拟请求的耗时操作
    System.out.println("threadnum:" + threadnum);
    Thread.sleep(1000);// 模拟请求的耗时操作
  }
}
```
除了 acquire方法之外，另一个比较常用的与之对应的方法是tryAcquire方法，该方法如果获取不到许可就立即返回 false。

Semaphore 有两种模式，公平模式和非公平模式。默认非公平模式
1. 公平模式： 调用 acquire 的顺序就是获取许可证的顺序，遵循 FIFO；
2. 非公平模式： 抢占式的。

# CountDownLatch 倒计时器
CountDownLatch 允许 count 个线程阻塞在一个地方，直至所有线程的任务都执行完毕。在 Java 并发中，countdownlatch 的概念是一个常见的面试题，所以一定要确保你很好的理解了它。

CountDownLatch 是共享锁的一种实现，它默认构造 AQS 的 state 值为 count。当线程使用 countDown 方法时，其实使用了 tryReleaseShared 方法以CAS 操作来减少 state， 直至 state 为0就代表了所有的线程都调用了 countDown 方法。当调用 await 方法时后，如果state不为0，就代表仍然有线程没有调用countDown方法，那么就把已经调用过countDown的线程都放入阻塞队列Park,并自旋CAS判断state == 0，直至最后一个线程调用了countDown，使得state == 0，于是阻塞的线程便判断成功，全部往下执行。
## countDownLatch 的使用示例
```java
package cn.mldn.thread;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class CountDownLatchExample {
	// 请求的数量
	private static final int threadCount = 550;

	public static void main(String[] args) throws InterruptedException {
	    // 创建一个具有固定线程数量的线程池对象（如果这里线程池的线程数量给太少的话你会发现执行的很慢）
	    ExecutorService threadPool = Executors.newFixedThreadPool(300);
	    final CountDownLatch countDownLatch = new CountDownLatch(threadCount);
	    for (int i = 0; i < threadCount; i++) {
	    	final int threadnum = i;
	    	threadPool.execute(()->{
	    		try {
	    			test(threadnum);
	    		} catch(InterruptedException e) {
	    			e.printStackTrace();
	    		} finally {
	    			countDownLatch.countDown();
	    		}
	    	});
	    }
	    countDownLatch.await();
	    threadPool.shutdown();
	    System.out.println("finish");
	}
	
	public static void test(int threadnum) throws InterruptedException {
		Thread.sleep(1000);
		System.out.println("threadnum:" + threadnum);
		Thread.sleep(1000);
	}
}
```
## countDownLatch 的两种典型用法
1. 某一线程在开始运行前等待 n 个线程执行完毕。将 CountDownLatch 的计数器初始化为 n ：new CountDownLatch(n)，每当一个任务线程执行完毕，就将计数器减 1 countdownlatch.countDown()，当计数器的值变为 0 时，在CountDownLatch上 await() 的线程就会被唤醒。一个典型应用场景就是启动一个服务时，主线程需要等待多个组件加载完毕，之后再继续执行。
2. 实现多个线程开始执行任务的最大并行性。注意是并行性，不是并发，强调的是多个线程在某一时刻同时开始执行。类似于赛跑，将多个线程放到起点，等待发令枪响，然后同时开跑。做法是初始化一个共享的 CountDownLatch 对象，将其计数器初始化为1：new CountDownLatch(1), 多个线程在开始执行任务前首先 countDownLatch.await(), 当主线成调用 countDown() 时，计数器变为0，多个线程同时被唤醒。

## CountDownLatch 的不足
CountDownLatch 是一次性的，计数器的值只能在构造方法中初始化一次，之后没有任何机制再次对其设置值，当 CountDownLatch 使用完毕后，它不能再次被使用。

# CyclicBarrier (循环栅栏)
CyclicBarrier 和 CountDownLatch 非常类似，它也可以实现线程间的技术等待，但是它的功能比 CountDownLatch 更加复杂和强大。主要应用场景和 CountDownLatch 类似。

CountDownLatch的实现是基于AQS的，而CycliBarrier是基于 ReentrantLock(ReentrantLock也属于AQS同步器)和 Condition 的.

CyclicBarrier 的字面意思是可循环使用（Cyclic）的屏障（Barrier）。它要做的事情是，让一组线程到达一个屏障（也可以叫同步点）时被阻塞，直到最后一个线程到达屏障时，屏障才会开门，所有被屏障拦截的线程才会继续干活。CyclicBarrier 默认的构造方法是 CyclicBarrier(int parties)，其参数表示屏障拦截的线程数量，每个线程调用await方法告诉 CyclicBarrier 我已经到达了屏障，然后当前线程被阻塞。

## CyclicBarrier 的使用示例
```java
package cn.mldn.thread;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class CyclicBarrierExample {
	//请求的数量
	private static final int threadCount = 50;
	//需要同步的线程的数量
	private static final CyclicBarrier cyclicBarrier = new CyclicBarrier(5);
	
	public static void main(String[] args) throws InterruptedException {
		ExecutorService threadPool = Executors.newFixedThreadPool(10);
		for (int i = 0; i < threadCount; i++) {
			final int threadNum = i;
			Thread.sleep(1000);
			threadPool.execute(()->{
				try {
					test(threadNum);
				} catch(InterruptedException e) { e.printStackTrace(); }
				catch(BrokenBarrierException e) { e.printStackTrace(); }
			});
		}
		threadPool.shutdown();
	}
	
	public static void test(int threadnum) throws InterruptedException, BrokenBarrierException {
		System.out.println("threadnum:" + threadnum + "is ready");
		cyclicBarrier.await();
		System.out.println("threadnum:" + threadnum + "is finish");
	}
}
```
## CyclicBarrier 的应用场景
CyclicBarrier 可以用于多线程计算数据，最后合并计算结果的应用场景。比如我们用一个 Excel 保存了用户所有银行流水，每个 Sheet 保存一个帐户近一年的每笔银行流水，现在需要统计用户的日均银行流水，先用多线程处理每个 sheet 里的银行流水，都执行完之后，得到每个 sheet 的日均银行流水，最后，再用 barrierAction 用这些线程的计算结果，计算出整个 Excel 的日均银行流水。
## CyclicBarrier 源码分析
1. 构造函数
```java
public CyclicBarrier(int parties) {
    this(parties, null);
}
//barrierAction当线程到达屏障时，优先执行
public CyclicBarrier(int parties, Runnable barrierAction) {
    if (parties <= 0) throw new IllegalArgumentException();
    this.parties = parties;
    this.count = parties;
    this.barrierCommand = barrierAction;
}
```
2. await() 方法
```java
public int await() throws InterruptedException, BrokenBarrierException {
    try { return dowait(false, 0L); }
    catch (TimeoutException toe) { throw new Error(toe); } //cannot happen
}
```
doawait(false, 0L) 源码如下：
```java
private int count;
private int dowait(boolean timed, long nanos) throws InterruptedException, BrokenBarrierException, TimeoutException {
    final ReentrantLock lock = this.lock;
    lock.lock();
    try {
        final Generation g = generation;
        if (g.broken) throw new BrokenBarrierException();
        // 如果线程中断了，抛出异常
        if (Thread.interrupted()) {
            breakBarrier();
            throw new InterruptedException();
        }
        // cout减1
        int index = --count;
        // 当 count 数量减为 0 之后说明最后一个线程已经到达栅栏了，也就是达到了可以执行await 方法之后的条件
        if (index == 0) {  // tripped
            boolean ranAction = false;
            try {
                final Runnable command = barrierCommand;
                if (command != null) command.run();
                ranAction = true;
                // 将 count 重置为 parties 属性的初始化值
                // 唤醒之前等待的线程
                // 下一波执行开始
                nextGeneration();
                return 0;
            } finally {
                if (!ranAction)
                    breakBarrier();
            }
        }

        // loop until tripped, broken, interrupted, or timed out
        for (;;) {
            try {
                if (!timed)
                    trip.await();
                else if (nanos > 0L)
                    nanos = trip.awaitNanos(nanos);
            } catch (InterruptedException ie) {
                if (g == generation && ! g.broken) {
                    breakBarrier();
                    throw ie;
                } else {
                    // We're about to finish waiting even if we had not
                    // been interrupted, so this interrupt is deemed to
                    // "belong" to subsequent execution.
                    Thread.currentThread().interrupt();
                }
            }

            if (g.broken)
                throw new BrokenBarrierException();

            if (g != generation)
                return index;

            if (timed && nanos <= 0L) {
                breakBarrier();
                throw new TimeoutException();
            }
        }
    } finally {
        lock.unlock();
    }
}
```
# ReentrantLock 和 ReentrantReadWriteLock 的区别
