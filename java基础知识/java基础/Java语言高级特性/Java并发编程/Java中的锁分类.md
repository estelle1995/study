1. 公平锁/非公平锁
2. 可重入锁
3. 独享锁/共享锁
4. 互斥锁/读写锁
5. 乐观锁/悲观锁
6. 分段锁
7. 偏向锁/轻量级锁/重量级锁
8. 自旋锁

# 公平锁/非公平锁
公平锁是指多个线程按照申请锁的顺序来获取锁




# 可重入锁
可重入锁又名递归锁，是指在同一个线程在外层方法获取锁的时候，在进入内层方法会自动获取锁。如下
```java
synchronized void setA() throws Exception{
    Thread.sleep(1000);
    setB();
}
synchronized void setB() throws Exception{
    Thread.sleep(1000);
}
```
ReentrantLock, synchronized 都是可重入锁

# 独享锁/共享锁
独享锁是指该锁一次只能被一个线程所持有。共享锁是指该锁可被多个线程所持有

共享锁是指该锁可被多个线程所持有。

对于 ReentrantLock 而言， 其实是独享锁，但是对于 Lock 的另一个实现类