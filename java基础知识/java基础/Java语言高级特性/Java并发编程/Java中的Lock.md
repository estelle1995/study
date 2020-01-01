# synchronized 的缺陷
当一个代码块被 synchronized 修饰了，当一个线程获取了对应的锁，并执行该代码块时，其他线程便只能一直等待，等待获取锁的线程释放锁，而这里获取锁的线程释放锁只会有两种情况：
1. 获取锁的线程执行完了该代码块，然后线程释放对锁的占有；
2. 线程执行发生异常，此时 JVM 会让线程自动释放锁

那么如果这个获取锁的线程由于要等待 IO 或其他原因(比如调用sleep方法)被阻塞了，但是又没有释放锁，其他线程只能干巴巴地等待，影响程序执行效率。因此就需要有一种机制可以不让等待地线程一直无期限的等待下去（比如只等待一定的时间或者能够响应中断），通过 Lock 就可以办到。

另外，通过Lock可以知道线程有没有成功获取到锁，这个是synchronized无法办到的。

1. Lock 不是Java语言内置的，synchronized是Java语言的关键字，因此是内置特性. Lock是一个类，通过这个类可以实现同步访问
2. Lock 和 synchronized 有一点非常到的不同，采用synchronized 不需要用户手动释放锁，当synchronized方法或者synchronized代码块执行完成之后，系统会自动让线程释放对锁的占用；而Lock则必须要用户去手动释放锁，如果没有主动释放锁，就有可能导致出现死锁现象。

# java.util.concurrent.locks 包下常用的类

## Lock
定义如下
```java
public interface Lock {
    void lock();
    void lockInterruptibly() throws InterruptedException;
    boolean tryLock();
    boolean tryLock(long time, TimeUnit unit) throws InterruptedException;
    void unlock();
} 
```
lock()、tryLock()、tryLock(long time, TimeUnit unit)和lockInterruptibly()是用来获取锁的。unLock()方法是用来释放锁的。

由于在前面讲到如果采用Lock，必须主动去释放锁，并且在发生异常时，不会自动释放锁。因此一般来说，使用Lock必须在try{}catch{}块中进行，并且将释放锁的操作放在finally块中进行，以保证锁一定被被释放，防止死锁的发生。通常使用Lock来进行同步的话，是以下面这种形式去使用的：

lock()方法是平常使用最多的一个方法，就是用来获取锁，如果锁已经被其他线程或得，则进行等待。
```java
Lock lock = ...;
lock.lock();
try {
    //处理任务
} catch (Exception e) {

} finally {
    lock.unlock();
}
```

tryLock() 是有返回值的，它表示用来尝试获取锁，如果获取成功，则返回 true, 即锁已被其他线程获取），则返回false，也就说这个方法无论如何都会立即返回。在拿不到锁时不会一直在那等待。

　tryLock(long time, TimeUnit unit)方法和tryLock()方法是类似的，只不过区别在于这个方法在拿不到锁时会等待一定的时间，在时间期限之内如果还拿不到锁，就返回false。如果如果一开始拿到锁或者在等待期间内拿到了锁，则返回true。
```java
Lock lock = ...;
if(lock.tryLock()) {
     try{
         //处理任务
     }catch(Exception ex){
         
     }finally{
         lock.unlock();   //释放锁
     } 
}else {
    //如果不能获取锁，则直接做其他事情
}
```

lockInterruptibly() 方法比较特殊，当通过这个方法去获取锁时，如果线程正在等待获取锁，则这个线程能够响应中断，即中断线程的等待状态。也就是说，当两个线程同时通过lock.Interruptibly() 想获取某个锁时，假若此时线程A获取到了锁，而线程B只有在等待，那么对线程B调用threadB.interrupt()方法能够中断线程B的等待过程。

由于lockInterruptibly()的声明中抛出了异常，所以lock.lockInterruptibly()必须放在try块中或者在调用lockInterruptibly()的方法外声明抛出InterruptedException。
```java
public void method() throws InterruptedException {
    lock.lockInterruptibly();
    try {

    } finally {
        lock.unlock();
    }
}
```

# ReentrantLock “可重入锁”
ReentrantLock是唯一实现了Lock接口的类，并且ReentrantLock提供了更多的方法。

```java
public class Test {
    private ArrayList<Integer> arrayList = new ArrayList<Integer>();
    public static void main(String[] args)  {
        final Test test = new Test();
         
        new Thread(){
            public void run() {
                test.insert(Thread.currentThread());
            };
        }.start();
         
        new Thread(){
            public void run() {
                test.insert(Thread.currentThread());
            };
        }.start();
    }  
     
    public void insert(Thread thread) {
        Lock lock = new ReentrantLock();    //注意这个地方
        lock.lock();
        try {
            System.out.println(thread.getName()+"得到了锁");
            for(int i=0;i<5;i++) {
                arrayList.add(i);
            }
        } catch (Exception e) {
            // TODO: handle exception
        }finally {
            System.out.println(thread.getName()+"释放了锁");
            lock.unlock();
        }
    }
}
```
输出结果可能如下：
```java
Thread-0得到了锁
Thread-1得到了锁
Thread-0释放了锁
Thread-1释放了锁
```
在insert方法中的lock变量是局部变量，每个线程执行该方法时都会保存一个副本，那么理所当然每个线程执行到lock.lock()处获取的是不同的锁，所以就不会发生冲突。

正确使用如下：
```java
public class Test {
    private ArrayList<Integer> arrayList = new ArrayList<Integer>();
    private Lock lock = new ReentrantLock();    //注意这个地方
    public static void main(String[] args)  {
        final Test test = new Test();
         
        new Thread(){
            public void run() {
                test.insert(Thread.currentThread());
            };
        }.start();
         
        new Thread(){
            public void run() {
                test.insert(Thread.currentThread());
            };
        }.start();
    }  
     
    public void insert(Thread thread) {
        lock.lock();
        try {
            System.out.println(thread.getName()+"得到了锁");
            for(int i=0;i<5;i++) {
                arrayList.add(i);
            }
        } catch (Exception e) {
            // TODO: handle exception
        }finally {
            System.out.println(thread.getName()+"释放了锁");
            lock.unlock();
        }
    }
}
```

tryLock 用法如下：
```java
public class Test {
    private ArrayList<Integer> arrayList = new ArrayList<Integer>();
    private Lock lock = new ReentrantLock();    //注意这个地方
    public static void main(String[] args)  {
        final Test test = new Test();
         
        new Thread(){
            public void run() {
                test.insert(Thread.currentThread());
            };
        }.start();
         
        new Thread(){
            public void run() {
                test.insert(Thread.currentThread());
            };
        }.start();
    }  
     
    public void insert(Thread thread) {
        if(lock.tryLock()) {
            try {
                System.out.println(thread.getName()+"得到了锁");
                for(int i=0;i<5;i++) {
                    arrayList.add(i);
                }
            } catch (Exception e) {
                // TODO: handle exception
            }finally {
                System.out.println(thread.getName()+"释放了锁");
                lock.unlock();
            }
        } else {
            System.out.println(thread.getName()+"获取锁失败");
        }
    }
}

```

lockInterruptibly()响应中断的使用方法：
```java
package cn.mldn.demo;

import java.util.ArrayList;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class ThreadDemo {
	private ArrayList<Integer> arrayList = new ArrayList<Integer>();
	private static Lock lock = new ReentrantLock();
	
	public static void main(String[] args) throws Exception {
		ThreadDemo demo = new ThreadDemo();
		Thread thread1 = new Thread(()-> {
			try {
				demo.insert(Thread.currentThread());
			} catch (InterruptedException e) {
				System.out.println(Thread.currentThread().getName() + "被中断");
			}
		});
		Thread thread2 = new Thread(()-> {
			try {
				demo.insert(Thread.currentThread());
			} catch (InterruptedException e) {
				System.out.println(Thread.currentThread().getName() + "被中断");
			}
		});
		thread1.start();
		thread2.start();
		Thread.sleep(2000);
		thread2.interrupt();
	}
	
	public void insert(Thread thread) throws InterruptedException {
		lock.lockInterruptibly();
		try {
			System.out.println(thread.getName() + "得到了锁");
			long start = System.currentTimeMillis();
			while (true) {
				if (System.currentTimeMillis() - start >= Integer.MAX_VALUE)
					break;
			}
		} finally {
			System.out.println(Thread.currentThread().getName() + "执行finally");
			lock.unlock();
			System.out.println(thread.getName() + "释放了锁");
		}
	}
}

```