ReentrantLock 可以实现公平锁

## 为什么非公平锁性能好
非公平锁对锁的竞争是抢占式的（队列中线程除外），线程在进入等待队列前可以进行两次尝试，这大大增加了获取锁的机会。这种好处体现在两个方面：
1. 线程不必加入等待队列就可以获得锁，不仅免去了构造结点并加入队列的繁琐操作，同时也节省了线程阻塞唤醒的开销，线程阻塞和唤醒涉及到线程上下文切换和操作系统的系统调用，是非常耗时的。在高并发情况下,如果线程持有锁的时间非常短,短到线程入队阻塞的过程超过线程持有并释放锁的时间开销,那么这种抢占式特性对并发性能的提升会更加明显。
2. 减少 CAS 竞争。如果线程必须要加入阻塞队列才能获取锁，那入队时CAS竞争将变得异常激烈，CAS操作虽然不会导致失败线程挂起，但不断失败重试导致的对CPU的浪费也不能忽视。除此之外，加锁流程中至少有两处通过将某些特殊情况提前来减少CAS操作的竞争，增加并发情况下的性能。一处就是获取锁时将非重入的情况提前，如下所示
```java
final void lock() {
    if (compareAndSetState(0, 1)) 
        setExclusiveOwnerThread(Thread.currentThread());
    else 
        acquire(1);
}
```
另一处就是将同步队列非空的情况提前处理
```java
private Node addWaiter(Node mode) {
    // 以给定模式构造结点。mode有两种：EXCLUSIVE（独占）和SHARED（共享）
    Node node = new Node(Thread.currentThread(), mode);

    //尝试快速方式直接放到队尾
    Node pred = tail;
    if (pred != null) {
        node.prev = pred;
        if (compareAndSetTail(pred, node)) {
            pred.next = node;
            return node;
        }
    }
    // 上一步失败则通过enq入队。
    enq(node);
    return node;
}
```