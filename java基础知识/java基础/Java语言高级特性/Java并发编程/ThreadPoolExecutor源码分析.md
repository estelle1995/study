# 继承关系
## Executor 接口
```
public interface Executor {
    void execute(Runnable command);
}
```
## ExecutorService 接口
```java
public interface ExecutorService extends Executor {
    void shutdown();
    List<Runnable> shutdownNow();
    boolean isShutdown();
    boolean isTerimated();
    boolean awaitTermination(long timeout, TimeUnit unit) throws InterruptedException;
    <T> Future<T> submit(Callable<T> task);
    <T> Future<T> submit(Runnable task, T result);
    Future<?> submit(Runnable task);
    <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> tasks) throws InterruptedException;
    <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> tasks, long timeout, TimeUnit unit)
        throws InterruptedException;
    <T> T invokeAny(Collection<? extends Callable<T>> tasks)
        throws InterruptedException, ExecutionException;
    <T> T invokeAny(Collection<? extends Callable<T>> tasks,
                    long timeout, TimeUnit unit)
        throws InterruptedException, ExecutionException, TimeoutException;
}
```
ExecutorService接口继承Executor接口，并增加了submit、shutdown、invokeAll等等一系列方法。

## AbstractExecutorService 抽象类
```java
public abstract class AbstractExecutorService implements ExecutorService {
    protected <T> RunnableFuture<T> newTaskFor(Runnable runnable, T value) {
        return new FutureTask<T>(runnable, value);   
    }
    protected <T> RunnableFuture<T> newTaskFor(Callable<T> callable) {
        return new FutureTask<T>(callable);
    }
    public Future<?> submit(Runnable task) {
        if (task == null) throw new NullPointerException();
        RunnableFuture<Void> ftask = newTaskFor(task, null);
        execute(ftask);
        return ftask;
    }
      public <T> Future<T> submit(Runnable task, T result) {
        if (task == null) throw new NullPointerException();
        RunnableFuture<T> ftask = newTaskFor(task, result);
        execute(ftask);
        return ftask;
    }

    public <T> Future<T> submit(Callable<T> task) {
        if (task == null) throw new NullPointerException();
        RunnableFuture<T> ftask = newTaskFor(task);
        execute(ftask);
        return ftask;
    }
    private <T> T doInvokeAny(Collection<? extends Callable<T>> tasks,
                              boolean timed, long nanos)
        throws InterruptedException, ExecutionException, TimeoutException {...}

    public <T> T invokeAny(Collection<? extends Callable<T>> tasks)
        throws InterruptedException, ExecutionException {... }

    public <T> T invokeAny(Collection<? extends Callable<T>> tasks,
                           long timeout, TimeUnit unit)
        throws InterruptedException, ExecutionException, TimeoutException {...}

    public <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> tasks)
        throws InterruptedException {...}

    public <T> List<Future<T>> invokeAll(Collection<? extends Callable<T>> tasks,
                                         long timeout, TimeUnit unit)
        throws InterruptedException {...}
}
```
AbstractExecutorService抽象类实现ExecutorService接口，并且提供了一些方法的默认实现，例如submit方法、invokeAny方法、invokeAll方法。

像execute方法、线程池的关闭方法（shutdown、shutdownNow等等）就没有提供默认的实现。

# ThreadPoolExecutor
## 线程池状态
```java
//记录线程池状态和线程数量（总共32位，前3位表示线程池状态，后29位表示线程数量）
private final AtomicInteger ctl = new AtomicInteger(ctlOf(RUNNING, 0));
//线程数量统计位数29
private static final int COUNT_BITS = Integer.size - 3;
//容量 000 11111111111111111111111111111
private static final int CAPACITY = (1 << COUNT_BITS) - 1;

//运行中 111 000000000000000000000000000000
private static final int RUNNING = -1 << COUNT_BITS;
//关闭 000 000000000000000000000000000000
private static final int SHUTDOWN = 0 << COUNT_BITS;
//停止 001 000000000000000000000000000000
private static final int STOP = 1 << COUNT_BITS;
//整理 010 000000000000000000000000000000
private static final int TIDYING = 2 << COUNT_BITS;
//终止 011 000000000000000000000000000000
private static final int TERMINATED = 3 << COUNT_BITS;

//获取运行状态（获取前3位）
private static int runStateOf(int c) {return c && ~CAPACITY;}
//获取线程个数（获取后29位）
private statinc int workerCountOf(int c) {return c && CAPACITY;}
private static int ctlOf(int rs, int wc) {return rs | wc;} 
```
1. RUNNING 接受新任务并且处理阻塞队列里的任务
2. SHUTDOWN 拒绝新任务并且处理阻塞队列里的任务
3. STOP 拒绝新任务并且抛弃阻塞队列里的任务同时会中断正在处理的任务
4. TIDYING 所有任务都执行完（包含阻塞队列里面任务）当前线程池活动线程为0，将要调用 terminated 方法
5. TERMINATED 终止状态。termianted方法调用完成以后的状态。

线程池状态转换：
1. RUNNING -> SHUTDOWN: 显示调用 shutdown() 方法，或者隐式调用了 finalize() 方法。
2. RUNNING or SHUTDOWN -> STOP: 显示调用 shutdownNow() 方法
3. SHUTDOWN -> TIDYING: 当线程池为空和队列都为空的时候
4. STOP -> TIDYING 当线程池为空的时候
5. TIDYING -> TERMINATED 当 terminated() hook 方法执行完成时候。

## 构造函数
有四个构造函数，其他三个都是调用下面代码中的这个构造函数
```java
public ThreadPoolExecutor(int corePoolSize,
                          int maximumPoolSize,
                          long keepAliveTime,
                          TimeUnit unit,
                          BlockingQueue<Runnable> workQueue,
                          ThreadFactory threadFactory,
                          RejectedExecutionHandler handler) {
}
```
1. corePoolSize 线程池的核心线程数量
2. maximumPoolSize 线程池的最大线程数量
3. keepAliveTime 当线程数大于核心线程数时，多余的空闲线程存活的最长时间
4. unit 时间单位
5. workQueue 任务队列，用来存储等待执行任务的队列
6. threadFactory 线程工厂，用来创建线程，一般默认即可
7. handler 拒绝策略，当提交的任务过多而不能及时处理时，我们可以定制策略来处理任务

## 提交任务
```java
public Future<?> submit(Runnable task) {
    if (task == null) throw new NullPointerException();
    RunnableFuture<Void> ftask = newTaskFor(task, null);
    execute(ftask);
    return ftask;
}

public <T> Future<T> submit(Runnable task, T result) {
    if (task == null) throw new NullPointerException();
    RunnableFuture<T> ftask = newTaskFor(task, result);
    execute(ftask);
    return ftask;
}

public <T> Future<T> submit(Callable<T> task) {
    if (task == null) throw new NullPointerException();
    RunnableFuture<T> ftask = newTaskFor(task);
    execute(ftask);
    return ftask;
}
```
流程步骤如下：
1. 调用 submit 方法，传入 Runnable 或者 Callable 对象
2. 判断传入的对象是否为 null, 为 null 则抛出异常，不为 null 则继续历程
3. 将传入的 runnable 或者 callable 对象转换城 RunnableFuture 对象
4. 将 RunnuableFuture 对象交由 execute 执行
5. 返回 RunnableFuture 对象

## execute 
```java
public void execute(Runnable command) {
    //传进来的线程为 null, 则抛出异常
    if (command == null) throw new NullPointerException();

    //获取当前线程池的状态，与线程个数，（前3位为线程池状态，后29位为线程个数）
    int c = ctl.get();

    //1.若线程的个数小于核心线程个数
    if (workerCountOf(c) < corePoolSize) {
        if (addWorker(command, true)) return;
        c = ctl.get();
    }
    //2.线程的数量大于等于核心线程数，若线程池处于Running状态将任务加入等待队列
    if (isRunning(c) && workQueue.offer(command)) { //如果线程池处于 Running 状态且任务加入等待队列成功
        // 再次检查线程池的运行状态
        int recheck = ctl.get();
        // 如果线程池处于非运行状态，将任务从等待队列移除，并执行拒绝策略
        if (!isRunning(recheck) && remove(command)) reject(command);
        // 否则如果当前线程池为空。则添加一个线程
        else if (workerCountOf(recheck) == 0) addWorker(null, flase);
    }
    //3.线程的数量大于等于线程最大数量，新增线程，如果新增线程失败则执行拒绝策略
    else if (!addWorker(command, false)) reject(command);
}
```
![](assets/提交任务到线程池的步骤.png)


## addWorker
```java
private boolean addWorker(Runnable firstTask, boolean core) {
    retry:
    for (;;) {
        int c = ctl.get();
        int rs = runStateOf(c);

        //以下几种情况会返回 false
        //当前线程池状态为STOP，TIDYING，TERMINAED
        //当前线程池状态为 SHUTDOWN, 且任务非空
        //当前线程池状态为 SHUTDOWN，且任务与任务队列为空
        if (rs >= SHUTDOWN && (rs != SHUTDOWN || firstTask == null || workQueue.isEmpty())) return false;
        for (;;) {
            int wc = workerCountOf(c);
            if (wc >= capacity || wc >= (core?corePoolSize:maximumPoolSize)) return false;
            //CAS增加c，成功则跳出retry
            if (compareAndIncrementWorkerCount(c)) break retry;
            //CAS失败执行下面方法，查看当前线程状态是否变化，变化则继续retry循环，没变化则继续内部循环
            c = ctl.get();
            if (runStateOf(c) != rs) continue retry;
        }
    }
    //CAS 成功
    boolean workerStarted = false;
    boolean workerAdded = false;
    Worker w = null;
    try {
        //新建一个线程
        w = new Worker(firstTask);
        final Thread t = w.thread;
        if (t != null) {
            //加锁
            final ReentrantLock mainLock = this.mainLock;
            mainLock.lock();
            try {
                //重新检查线程池状态
                //避免 ThreadFactory 退出故障或者在锁获取前线程池被关闭
                int rs = runStateOf(ctl.get());
                if (rs < SHUTDOWN || (rs == SHUTDOWN && firstTask == null)) {
                    if (t.isAlive()) throw new IllegalThreadStateException();
                    workers.add(w);
                    int s = workers.size();
                    if (s > largestPoolSize) largestPoolSize = s;
                    workerAdded = true;
                }
            } finally {
                mainLock.unlock();
            }
            if (workerAdded) {
                t.start();
                workerStated = true;
            }
        }
    } finally {
        //判断线程有没有启动成功， 没有则调用 addWorkerFailed 方法
        if (!workerStarted) addWorkerFailed(w);
    }
    return workerStarted;
}
```
## Worker 对象
Worker 是定义在 ThreadPoolExecutor 中的 final 类，其中继承了 AbstractQueuedSynchronizer 类和实现 Runnable 接口，其中的 run 方法如下：
```java
public void run() {
    runWorker(this);
}
```
## runWorker 方法
```java
final void runWorker(Worker w) {
    Thread wt = Thread.currentThread();
    Runnable task = w.firstTask;
    w.firstTask = null;
    w.unLock();
    boolean completelyAbrutly = true;
    try {
        //循环获取任务
        while(task != null || (task = getTask()) != null) {
            w.lock();
            // 当线程池是处于STOP状态或者TIDYING、TERMINATED状态时，设置当前线程处于中断状态
            // 如果不是，当前线程就处于RUNNING或者SHUTDOWN状态，确保当前线程不处于中断状态
            // 重新检查当前线程池的状态是否大于等于STOP状态
            if ((runStateAtLeast(ctl.get(), STOP) ||
                 (Thread.interrupted() &&
                  runStateAtLeast(ctl.get(), STOP))) &&
                !wt.isInterrupted())
                wt.interrupt();
            try {
                //提供给继承类使用做一些统计之类的事情，在线程运行前调用
                beforeExecute(wt, task);
                Throwable thrown = null;
                try {
                    task.run();
                } catch (RuntimeException x) {
                    thrown = x; throw x;
                } catch (Error x) {
                    thrown = x; throw x;
                } catch (Throwable x) {
                    thrown = x; throw new Error(x);
                } finally {
                    //提供给继承类使用做一些统计之类的事情，在线程运行之后调用
                    afterExecute(task, thrown);
                }
            } finally {
                task = null;
                //统计当前worker完成了多少个任务
                w.completedTasks++;
                w.unlock();
            }
        }
        completedAbruptly = false;
    } finally {
        //整个线程结束时调用，线程退出操作。统计整个线程池完成的任务个数之类的工作
        processWorkerExit(w, completedAbruptly);
    }
}
```
## getTask 
```java
private Runnable getTask() {
    boolean timedOut = false; // Did the last poll() time out?
	//循环
    for (;;) {
        int c = ctl.get();
        int rs = runStateOf(c);

        //线程线程池状态和队列是否为空
        if (rs >= SHUTDOWN && (rs >= STOP || workQueue.isEmpty())) {
            decrementWorkerCount();
            return null;
        }
		//线程数量
        int wc = workerCountOf(c);

        
        boolean timed = allowCoreThreadTimeOut || wc > corePoolSize;

        //（当前线程数是否大于最大线程数或者）
        //且（线程数大于1或者任务队列为空）
        //这里有个问题(timed && timedOut)timedOut = false，好像(timed && timedOut)一直都是false吧
        if ((wc > maximumPoolSize || (timed && timedOut))
            && (wc > 1 || workQueue.isEmpty())) {
            if (compareAndDecrementWorkerCount(c))
                return null;
            continue;
        }

        try {
            //获取任务
            Runnable r = timed ?
                workQueue.poll(keepAliveTime, TimeUnit.NANOSECONDS) :
                workQueue.take();
            if (r != null)
                return r;
            timedOut = true;
        } catch (InterruptedException retry) {
            timedOut = false;
        }
    }
}
```
## shutdown
当调用 shutdown 方法时，线程池将不会再接收新的任务，然后将先前放在队列中的任务执行完成。
```java
public void shutdown() {
    final ReentrantLock mainLock = this.mainLock;
    mainLock.lock();
    try {
        checkShutdownAccess();
        advanceRunState(SHUTDOWN);
        interruptIdleWorkers();
        onShutdown(); // hook for ScheduledThreadPoolExecutor
    } finally {
        mainLock.unlock();
    }
    tryTerminate();
}
```
## shutdownNow
立即停止所有的执行任务，并将队列中的任务返回
```java
public List<Runnable> shutdownNow() {
    List<Runnable> tasks;
    final ReentrantLock mainLock = this.mainLock;
    mainLock.lock();
    try {
        checkShutdownAccess();
        advanceRunState(STOP);
        interruptWorkers();
        tasks = drainQueue();
    } finally {
        mainLock.unlock();
    }
    tryTerminate();
    return tasks;
}
```
### shutdown和shutdownNow区别
shutdown和shutdownNow这两个方法的作用都是关闭线程池，流程大致相同，只有几个步骤不同，如下
1. 加锁
2. 检查关闭权限
3. CAS 改变线程池状态
4. 设置中断标志（线程池不再接收任务，队列任务会完成）/中断当前执行的线程
5. 调用 onShutdown 方法（给子类提供的方法）/获取队列中的任务
6. 解锁
7. 尝试将线程池状态变成终止状态TERMINATED
8. 结束/返回队列中的任务

线程池可以给我们多线程编码上提供极大便利，就好像数据库连接池一样，减少了线程的开销，提供了线程的复用。而且ThreadPoolExecutor也提供了一些未实现的方法，供我们来使用，像beforeExecute、afterExecute等方法，我们可以通过这些方法来对线程进行进一步的管理和统计。

在使用线程池上好需要注意，提交的线程任务可以分为CPU 密集型任务和IO 密集型任务，然后根据任务的不同进行分配不同的线程数量。
1. CPU密集型任务：应当分配较少的线程，比如 CPU个数相当的大小
2. IO密集型： 由于线程并不是一直在运行，所以可以尽可能的多配置线程，比如 CPU 个数 * 2
3. 混合型任务： 可以将其拆分为 CPU 密集型任务以及 IO 密集型任务，这样来分别配置。