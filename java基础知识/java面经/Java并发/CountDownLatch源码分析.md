# CountDownLatch 源码分析
## 内部类
```java
private static final class Sync extends AbstractQueuedSynchronizer {
    //版本号
    private static final long serialVersionUID = 4982264981922014374L;

    //构造器
    Sync(int count) { setState(count); }

    //返回当前计数
    int getCount() {
        return getState();
    }

    // 试图在共享模式下获取对象状态
    protected int tryAcquireShared(int acquires) {
        return (getState() == 0) ? 1 : -1;
    }

    //试图设置状态来反映共享模式下的一个释放
    protected boolean tryReleaseShared(int release) {
        //无限循环
        for(;;) {
            int c = getState();
            if (c == 0) return false; //没有线程被占有
            int nextc = c - 1;
            if (compareAndSetState(c, nextc)) //CAS设置新值
                return nextc == 0;
        }
    }
}
```
## 类的构造函数
```java
public CountDownLatch(int count) {
    if (count < 0) throw new IllegalArgumentException("count < 0");
    // 初始化状态数
    this.sync = new Sync(count);
}
```
## 核心函数分析
1. await 函数
    
此函数将会使当前线程在锁存器倒计数至零之前一直等待，除非线程被中断。其源码如下　
```java
public void await() throws InterruptedException {
    //转发到 sync 对象上
    sync.acquireSharedInterruptibly(1);
}
```
acquireSharedInterruptibly 的源码如下：
```java
public final void acquireSharedInterruptibly(int arg) throws InterruptedException{
    if (Thread.interrupted()) throw new InterruptedException();
    if (tryAcquireShared(arg) < 0) doAcquireSharedInterruptibly(arg);
    //doAcquireSharedInterruptibly函数只是简单的判断AQS的state是否为0，为0则返回1，不为0则返回-1。
}
```
doAcquireSharedInterruptibly函数的源码如下　　
```java
private void doAcquireSharedInterruptibly(int arg) throws InterruptedException {
    final Node node = addWaiter(Node.SHARED);
    boolean failed = true;
    try {
        for (;;) {
            final Node p = node.predecessor();
            if (p == head) { //前驱节点为头节点
                int r = tryAcquireShared(arg);
                if (r >= 0) {//获取成功
                    //设置头结点并唤醒后继节点
                    setHeadAndPropagate(node, r);
                    p.next = null; //help GC
                    failed = false;
                    return;
                }
            }
            if (shouldParkAfterFailedAcquire(p, node) && parkAndCheckInterrupt()) throw new InterruptedException();
        }
    } finally {
        if (failed) cancelAcquire(node);
    }
}
```
# countDown 函数
```java
public void countDown() {
    sync.releaseShared(1);
}
```

