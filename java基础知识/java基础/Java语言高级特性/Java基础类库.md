# StringBuffer
可以修改
1. 构造方法 public StringBuffer();  public StringBuffer(String str);
2. 数据追加 public StringBuffer append(数据类型 变量) 相当与String中的+

在String类中不具备的方法
1. public StringBuffer insert(int offset, 数据类型 变量);
2. public StringBuffer delete(int start, int end);
3. public StringBuffer reverse()

与StringBuffer类还有一个类似的功能类:StringBuilder类。StringBuilder是在JDK1.5之后提供的，该类中提供的方法与StringBuffer的功能相同。最大的区别在于StringBuffer类中的方法属于线程安全的，全部使用synchronized进行标志。


# CharSequence 接口
CharSequence 是一个描述字符串结构的接口，在这个接口里面一般发现有三种常用的子类 String， StringBuffer, StringBuilder


# AutoCloseable 接口
AutoCloseable 主要是用于日后进行资源开发的处理上，以实现资源的自动关闭（释放资源），比如以后再以后进行文件，网络，数据库的开发过程之中，由于服务器的资源有限，所以使用之后一定要关闭资源，这样才可以被更多的使用者所使用。这个是JDK1.7之后提出的，并且该接口只提供有一个方法 public void close() throws Exception;
该接口的关闭需要结合异常才能处理。如下所示
```java
package cn.mldn.demo;

interface IMessage extends AutoCloseable {
	public void send();                    //消息发送
}

class NetMessage implements IMessage{                            //实现消息的处理机制
	private String msg;
	public NetMessage(String msg) {
		this.msg = msg;
	}
	public boolean open() {                //获取资源
		System.out.println("[open]获取消息发送连接资源");
		return true;
	}
	
	public void close() throws Exception {
		System.out.println("[close]关闭消息发送通道");
	}
	
	@Override
	public void send() {
		if (this.open()) {
			System.out.println("[发送消息]" + this.msg);
		}
	}
}

public class JavaAPIDemo {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		try (IMessage msg = new NetMessage("网络资源")){
			msg.send();
		} catch(Exception e) {
			e.printStackTrace();
		}
	}

}
```

# Runtime 类
Runtime类描述的是运行时的状态，也就是说在整个的JVM之中，Runtime类是惟一一个与JVM运行状态有关的类，并且都会默认提供有一个该类的实例化对象

由于在每一个JVM进程里面只允许提供有一个Runtime类对象，所以这个类的构造方法被默认私有化了。所以这个类使用的是单例设计模式，单例设计模式一定会提供有一个 static 方法获取本类的实例。通过 public static Runtime getRuntime() 方法获取

通过这个类中的 public int availableProcessors() 方法可以获取本机的CPU内核数

除了以上的方法之外，在Runtime类里面还提供有以下四个重要的操作方法
1. 获取最大可用内存空间 public long maxMemory()   默认的配置为本机系统内存的四分之一
2. 获取可用内存空间     public long totalMemory() 默认的配置为本机系统内存的六十四分之一
3. 获取空闲内存空间     public long freeMemory()   
4. 手工进行GC处理       public void gc()

```java
public class JavaAPIDemo {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Runtime run = Runtime.getRuntime();
		System.out.println("1.MAX_MEMORY " + run.maxMemory());
		System.out.println("1.TOTAL_MEMORY " + run.totalMemory());
		System.out.println("1.FREE_MEMORY " + run.freeMemory());
		String str = "";
		for(int i = 0; i < 30000; i++) {
			str += i;
		}
		system.println();
		System.out.println("2.MAX_MEMORY " + run.maxMemory());
		System.out.println("2.TOTAL_MEMORY " + run.totalMemory());
		System.out.println("2.FREE_MEMORY " + run.freeMemory());
		run.gc();
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println();
		System.out.println("3.MAX_MEMORY " + run.maxMemory());
		System.out.println("3.TOTAL_MEMORY " + run.totalMemory());
		System.out.println("3.FREE_MEMORY " + run.freeMemory());
	}

}
```
上述代码输出如下:
```
1.MAX_MEMORY 3791650816
1.TOTAL_MEMORY 257425408
1.FREE_MEMORY 254741016

2.MAX_MEMORY 3791650816
2.TOTAL_MEMORY 1600126976
2.FREE_MEMORY 685482856

3.MAX_MEMORY 3791650816
3.TOTAL_MEMORY 1600126976
3.FREE_MEMORY 1571343224
```

# System 类
1. 数组拷贝: public static void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
2. 获取当前的日期时间数值: public static long currentTimeMillis();
3. 进行垃圾回收           public static void gc(); 该方法中继续调用了 Runtime.getRuntime().gc();


# Cleaner类
Cleaner类是JDK1.9之后提供的一个对象清理的操作，其主要的功能是进行finalize()方法的替代

Object类中提供的有protected void finalize​() throws Throwable方法	从JDK1.9开始这一操作已经不建议使用了，而对于对象回收释放。建议使用AutoCloseable或者使用 java.lang.ref.Cleaner类进行回收处理。

```java
class Member implements Runnable {
	public Member() {
		System.out.println("[构造]成员小强诞生");
	}

	@Override
	public void run() {
		// TODO Auto-generated method stub
		System.out.println("[回收]最终小强死了");
	}
	
}

class MemberCleaning implements AutoCloseable {
	private static Cleaner cleaner = Cleaner.create();
	private Member member;
	private Cleaner.Cleanable cleanable;
	
	public MemberCleaning() {
		this.member = new Member();
		this.cleanable = this.cleaner.register(this, this.member);
	}
	@Override
	public void close() throws Exception {
		// TODO Auto-generated method stub
		this.cleanable.clean();
	}
	
}
```


# 对象克隆
需要使用Object类中提供的 clone 方法
```
protected Object clone() throws CloneNotSupportedException
```
CloneNotSupportedException-if the object's class does not support the Cloneable interface

所有的类都会继承Object父类，所以所有的类都一定会有clone()方法，但是并不是所有的对象都希望被克隆。实现对象克隆，那么对象所在的类需要实现一个Cloneable接口，此接口并没有任何的方法提供，是因为它描述的是一种能力
```java
package cn.mldn.demo;

class Member implements Cloneable {
	private String name;
	private int age;
	public Member(String name, int age) {
		this.name = name;
		this.age = age;
	}
	
	@Override
	public String toString() {
		return "[" + super.toString() + "]name = " + this.name + " , age = " + this.age;
	}
	
	@Override
	protected Object clone() throws CloneNotSupportedException {
		return super.clone();
	}
}

public class JavaAPIDemo {

	public static void main(String[] args) {
		Member memberA = new Member("小强", 24);
		Member memberB = null;
		try {
			memberB = (Member)memberA.clone();
		} catch (CloneNotSupportedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println(memberA);
		System.out.println(memberB);
	}
}

```


