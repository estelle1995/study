# 实现最小值函数
自己设计一个泛型的获取数组最小值的函数，并且这个方法只能接受 Number 的子类并且实现了 Comparable 接口
```java
 public static <T extends Number & Comparable<? super T>> T min(T [] values) {
    if (values == null || values.length == 0) return null;
    T min = values[0];
    for (T value : values) {
    	if (value.compareTo(min) < 0) min = value;
    }
    return min;
}
```

# 使用数组实现栈
自己实现一个栈，要求这个栈具有 push(), pop()(返回栈顶元素并出栈)、peek()(返回栈顶元素不出栈)、isEmpty()、size()
```java
public class MyStack {
    private int[] storage;//存放栈中元素的数组
    private int capacity;//栈的容量
    private int count;//栈中元素数量
    private static final int GROW_FACTOR = 2;

    //TODO：不带初始容量的构造方法。默认容量为8
    public MyStack() {
        this.capacity = 8;
        this.storage=new int[8];
        this.count = 0;
    }

    //TODO：带初始容量的构造方法
    public MyStack(int initialCapacity) {
        if (initialCapacity < 1)
            throw new IllegalArgumentException("Capacity too small.");

        this.capacity = initialCapacity;
        this.storage = new int[initialCapacity];
        this.count = 0;
    }

    //TODO：入栈
    public void push(int value) {
        if (count == capacity) {
            ensureCapacity();
        }
        storage[count++] = value;
    }

    //TODO：确保容量大小
    private void ensureCapacity() {
        int newCapacity = capacity * GROW_FACTOR;
        storage = Arrays.copyOf(storage, newCapacity);
        capacity = newCapacity;
    }

    //TODO：返回栈顶元素并出栈
    private int pop() {
        count--;
        if (count == -1)
            throw new IllegalArgumentException("Stack is empty.");

        return storage[count];
    }

    //TODO：返回栈顶元素不出栈
    private int peek() {
        if (count == 0){
            throw new IllegalArgumentException("Stack is empty.");
        }else {
            return storage[count-1];
        }
    }

    //TODO：判断栈是否为空
    private boolean isEmpty() {
        return count == 0;
    }

    //TODO：返回栈中元素的个数
    private int size() {
        return count;
    }

}
```