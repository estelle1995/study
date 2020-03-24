java不能直接访问操作系统底层，而是通过本地方法来访问。Unsafe类提供了硬件级别的原子操作，主要提供了以下功能：

# 使用 Unsafe 需要注意以下几个问题
1. Unsafe 有可能在未来的 jdk版本移除或者不允许 Java 应用代码使用，这一点可能导致使用了 Unsafe 的应用无法运行在高版本的 JDK 中
2. Unsafe 的不少方法中必须提供原始地址（内存地址）和被替换对象的地址，偏移量要自己计算，一旦出现问题就是 JVM 崩溃级别的异常，会导致整个 JVM 实例崩溃，表现为应用程序直接 crash 掉
3. Unsafe 提供的直接访问内存访问的方法中使用的内存不受 JVM 管理（无法被GC），需要手动管理，一旦出现疏忽很有可能称为内存泄漏的源头。

# 通过 Unsafe 类可以分配内存、可以释放内存
类中提供的 3 个本地方法 allocateMemory, reallocateMemory, freeMemory分别用于分配内存，扩充内存和释放内存，与C语言中的3个方法对应。

# 可以定位对象某字段的内存位置，也可以修改对象的字段值，即使它是私有的
```java
public native long allocateMemory(long l);
public native long reallocateMemory(long l, long l1);
public native void freeMemory(long l);
```
字段的定义
1. Java中对象的字段的定位可能通过 staticFieldOffset 方法实现，该方法返回给定 field 的内存地址偏移量，这个值对于给定的 field 是唯一且是固定不变的。
2. getIntVolatile方法获取对象中offset偏移地址对应的整型field的值,支持volatile load语义。
3. getLong方法获取对象中offset偏移地址对应的long型field的值

## 数组元素定位
Unsafe类中有很多以BASE_OFFSET结尾的常量，比如ARRAY_INT_BASE_OFFSET，ARRAY_BYTE_BASE_OFFSET等，这些常量值是通过arrayBaseOffset方法得到的。arrayBaseOffset方法是一个本地方法，可以获取数组第一个元素的偏移地址。Unsafe类中还有很多以INDEX_SCALE结尾的常量，比如 ARRAY_INT_INDEX_SCALE ， ARRAY_BYTE_INDEX_SCALE等，这些常量值是通过arrayIndexScale方法得到的。arrayIndexScale方法也是一个本地方法，可以获取数组的转换因子，也就是数组中元素的增量地址。将arrayBaseOffset与arrayIndexScale配合使用，可以定位数组中每个元素在内存中的位置。
```java
public final class Unsafe {
    public static final int ARRAY_INT_BASE_OFFSET;
    public static final int ARRAY_INT_INDEX_SCALE;

    public native long staticFieldOffset(Field field);
    public native int getIntVolatile(Object obj, long l);
    public native long getLong(Object obj, long l);
    public native int arrayBaseOffset(Class class1);
    public native int arrayIndexScale(Class class1);

    static 
    {
        ARRAY_INT_BASE_OFFSET = theUnsafe.arrayBaseOffset([I);
        ARRAY_INT_INDEX_SCALE = theUnsafe.arrayIndexScale([I);
    }
}
```

# 挂起与恢复
将一个线程进行挂起是通过park方法实现的，调用 park后，线程将一直阻塞直到超时或者中断等条件出现。unpark可以终止一个挂起的线程，使其恢复正常。整个并发框架中对线程的挂起操作被封装在 LockSupport类中，LockSupport类中有各种版本pack方法，但最终都调用了Unsafe.park()方法。
```java
public class LockSupport {
    public static void unpark(Thread thread) {
        if (thread != null) 
            unsafe.unpark(thread);
    }

    public static void park(Object blocker) {
        Thread t = Thread.currentThread();
        setBlocker(t, blocker);
        unsafe.park(false, 0L);
        setBlocker(t, null);
    }

    public static void parkNanos(Object blocker, long nanos) {
        if (nanos > 0) {
            Thread t = Thread.currentThread();
            setBlocker(t, blocker);
            unsafe.park(false, nanos);
            setBlocker(t, null);
        }
    }

    public static void parkUntil(Object blocker, long deadline) {
        Thread t = Thread.currentThread();
        setBlocker(t, blocker);
        unsafe.park(true, deadline);
        setBlocker(t, null);
    }

    public static void park() {
        unsafe.park(false, 0L);
    }

    public static void parkNanos(long nanos) {
        if (nanos > 0)
            unsafe.park(false, nanos);
    }

    public static void parkUntil(long deadline) {
        unsafe.park(true, deadline);
    }
}
```

# CAS 操作
是通过 compareAndSwapXXX 方法实现的
```java
/**
* 比较 obj 的 offset 处内存位置中的值和期望的值，如果相同则更新。此更新是不可中断的
* @param obj 需要更新的对象
* @param offset obj 中整型 field 的偏移量
* @param expect 希望field中存在的值
* @param update 如果期望值expect与field的当前值相同，设置filed的值为这个新值
* @return 如果field的值被更改返回true
*/
public native boolean compareAndSwapInt(Object obj, long offset, int expect, int update);
```