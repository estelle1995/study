线程池就是维持几个工作线程，然后从任务队列中获取任务执行。所以要实现延时或者定时执行任务，就要做到以下三点：
1. 任务要能返回它的延时时间和是否为定时任务
2. 任务队列要根据任务的延时时间进行排序。
3. 如果是定时任务，任务执行完成之后，还可以再次执行它。

所以要分析ScheduledThreadPoolExecutor原理关键就是对它的任务类ScheduledFutureTask的分析。
```java
private class ScheduledFutureTask<V> extends FutureTask<V> implements RunnableScheduledFuture<V>{}
```
# RunnableScheduledFuture 接口
我们知道要实现延时或者定时执行任务，任务要能返回它的延时时间，任务是否为定时任务，任务能根据延时时间排序。所以可以想象出RunnableScheduledFuture接口中的方法了。
```java
public interface Comparable<T> {
    // 比较两个实例的大小
    public int compareTo(T o);
}
/**
 * 实现Comparable接口，说明Delayed实例可以进行比较
 */
public interface Delayed extends Comparable<Delayed> {

    /**
     * @return 返回剩余的延时时间
     */
    long getDelay(TimeUnit unit);
}

/**
 * 实现了Delayed接口，可以返回延时时间以及能够根据延时时间进行比较
 */
public interface ScheduledFuture<V> extends Delayed, Future<V> {
}

// 继承自Runnable和Future接口。可以当做Runnable实例使用
public interface RunnableFuture<V> extends Runnable, Future<V> {
    // 运行任务
    void run();
}

public interface RunnableScheduledFuture<V> extends 
                 RunnableFuture<V>, ScheduledFuture<V> {

    /**
     * 是否为周期定时任务
     * @return 返回true，表示是定时任务
     */
    boolean isPeriodic();
}
```
RunnableScheduledFuture接口中，最重要的就是这四个方法：
1. compareTo(): 可以比较任务的延时时间，进行排序用的
2. getDelay(TimeUnit unit) 返回任务剩余的延时时间
3. run() 运行任务。如果是定时任务，任务完成之后，还可以继续执行。
4. isPeriodic(): 是否为周期定时任务

# 何时创建 ScheduledFutureTask 任务
## 创建延时任务
何时创建ScheduledFutureTask任务，就是ScheduledExecutorService接口四个方法会创建ScheduledFutureTask任务实例。
```java
/ 给定的延迟时间delay之后，才会执行任务command
    public ScheduledFuture<?> schedule(Runnable command,
                                       long delay,
                                       TimeUnit unit) {
        if (command == null || unit == null)
            throw new NullPointerException();
        /**
         * 创建一个延时任务ScheduledFutureTask实例。
         * triggerTime方法会根据给定的延时时间delay，返回任务开始的时间。
         * decorateTask方法是让子类能够修饰ScheduledFutureTask任务实例，本类中没做处理。
         */
        RunnableScheduledFuture<?> t = decorateTask(command,
            new ScheduledFutureTask<Void>(command, null,
                                          triggerTime(delay, unit)));
        // 延时执行任务
        delayedExecute(t);
        return t;
    }

    // 给定的延迟时间delay之后，才会执行任务callable
    public <V> ScheduledFuture<V> schedule(Callable<V> callable,
                                           long delay,
                                           TimeUnit unit) {
        if (callable == null || unit == null)
            throw new NullPointerException();
        /**
         * 创建一个延时任务ScheduledFutureTask实例。
         * triggerTime方法会根据给定的延时时间delay，返回任务开始的时间。
         * decorateTask方法是让子类能够修饰ScheduledFutureTask任务实例，本类中没做处理。
         */
        RunnableScheduledFuture<V> t = decorateTask(callable,
            new ScheduledFutureTask<V>(callable,
                                       triggerTime(delay, unit)));
        // 延时执行任务
        delayedExecute(t);
        return t;
    }
```
这两个方法的流程是一样的，只不过一个是Runnable类型的任务，一个Callable类型的任务。
1. 调用 triggerTime 方法根据给定的延时时间 delay，返回任务开始的时间
2. 创建一个延时任务 ScheduledFutureTask 实例
3. 调用decorateTask方法是让子类能够修饰ScheduledFutureTask任务实例。
4. 调用delayedExecute(t)方法延时执行任务。
## 创建定时延时任务
```java
    public ScheduledFuture<?> scheduleAtFixedRate(Runnable command,
                                                  long initialDelay,
                                                  long period,
                                                  TimeUnit unit) {
        if (command == null || unit == null)
            throw new NullPointerException();
        if (period <= 0)
            throw new IllegalArgumentException();
        /**
         * 创建一个延时定时任务ScheduledFutureTask实例。
         * triggerTime方法会根据给定的延时时间delay，返回任务开始的时间。
         */
        ScheduledFutureTask<Void> sft =
            new ScheduledFutureTask<Void>(command,
                                          null,
                                          triggerTime(initialDelay, unit),
                                          unit.toNanos(period));
        // decorateTask方法是让子类能够修饰ScheduledFutureTask任务实例，本类中没做处理。
        RunnableScheduledFuture<Void> t = decorateTask(command, sft);
        // outerTask作用就是下一次定时执行的任务，在reExecutePeriodic方法中需要
        sft.outerTask = t;
        // 延时执行任务
        delayedExecute(t);
        return t;
    }

    public ScheduledFuture<?> scheduleWithFixedDelay(Runnable command,
                                                     long initialDelay,
                                                     long delay,
                                                     TimeUnit unit) {
        if (command == null || unit == null)
            throw new NullPointerException();
        if (delay <= 0)
            throw new IllegalArgumentException();
        /**
         * 创建一个延时定时任务ScheduledFutureTask实例。
         * triggerTime方法会根据给定的延时时间delay，返回任务开始的时间。
         */
        ScheduledFutureTask<Void> sft =
            new ScheduledFutureTask<Void>(command,
                                          null,
                                          triggerTime(initialDelay, unit),
                                          unit.toNanos(-delay));
        // decorateTask方法是让子类能够修饰ScheduledFutureTask任务实例，本类中没做处理。
        RunnableScheduledFuture<Void> t = decorateTask(command, sft);
        // outerTask作用就是下一次定时执行的任务，在reExecutePeriodic方法中需要
        sft.outerTask = t;
        // 延时执行任务
        delayedExecute(t);
        return t;
    }
```
scheduledAtFixedRate 方法是固定周期时间去执行任务，而 scheduledWithFixedDelay 方法是任务完成后，延时 delay 时间再去执行任务。
但是我们发现这两个方法几乎是一模一样的，唯一不同地就是创建延时定时任务ScheduledFutureTask实例时，一个传递的是正数period，一个传递的是负数-delay。
1. 调用 triggerTime 方法根据给定的延时时间 delay, 返回任务开始的时间
2. 创建一个延时定时任务 ScheduleFutureTask
3. 调用decorateTask方法是让子类能够修饰ScheduledFutureTask任务实例。
4. 设置outerTask，作用就是下一次定时执行的任务，在reExecutePeriodic方法中需要
5. 延时执行任务。

## delayedExecute 方法
```java
    private void delayedExecute(RunnableScheduledFuture<?> task) {
        // 如果线程池不是RUNNING状态，那么调用reject(task)方法，
        // 拒绝执行任务task
        if (isShutdown())
            reject(task);
        else {
            // 将任务添加到任务队列中，会根据任务的延时时间进行排序
            super.getQueue().add(task);
            /**
             * 如果线程池不是RUNNING状态，那么就判断能不能在当前状态下运行，
             * 主要就是能不能在SHUTDOWN状态下运行。
             * 如果不能在当前状态下运行，那么就调用remove方法，
             * 从任务队列中移除刚刚添加的任务task。
             *
             * 只有移除成功了，才可以调用task.cancel(false)方法取消任务，
             * 否则这个延时任务task都还要执行。
             */
            if (isShutdown() &&
                !canRunInCurrentRunState(task.isPeriodic()) &&
                remove(task))
                task.cancel(false);
            else
                // 预先启动工作线程，确保线程池中有工作线程。
                ensurePrestart();
        }
    }
```
这个方法的主要作用就是将任务添加到任务队列中，因为这里任务队列是优先级队列DelayedWorkQueue，它会根据任务的延时时间进行排序。
1. 如果线程池不是 RUNNING 状态，不能执行延时任务 task, 那么调用 reject(task) 方法，拒绝执行任务 task
2. 将任务添加到任务队列中，会根据任务的延时时间进行排序
3. 因为是多线程并发环境，就必须判断在添加任务的过程中，线程池状态是否被别的线程更改了，那么就可能要取消任务了。
4. 将任务添加到任务队列后，还要确保线程池中有工作线程，不然任务也不为执行。所以ensurePrestart()方法预先启动工作线程，确保线程池中有工作线程。

# ScheduleFutureTask 类
ScheduledFutureTask是一个延时定时任务，它可以返回任务剩余延时时间，可以被周期性地执行。
## 重要属性成员
```java
//是一个序列，每次创建任务的时候，都会自增
        private final long sequenceNumber;

        /** 任务能够开始执行的时间 */
        private long time;

        /**
         * 任务周期执行的时间
         * 0表示不是一个周期定时任务
         * 正数表示固定周期时间去执行任务
         * 负数表示任务完成之后，延时period时间再去执行任务
         */
        private final long period;

        /** 表示再次执行的任务，在reExecutePeriodic中调用 */
        RunnableScheduledFuture<V> outerTask = this;

        /**
         * 表示在任务队列中的索引位置，用来支持快速从队列中删除任务。
         */
        int heapIndex;
```
