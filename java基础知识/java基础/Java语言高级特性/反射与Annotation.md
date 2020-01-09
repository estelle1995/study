从 JDK1.5 开始提供了Annotation的支持
# 获取Annotation 信息
在进行类或方法定义的时候都可以使用一系列的 Annotation 进行声明。在 java.lang.reflect 中有一个 AccessibleObject 类提供有获取 Annotation 接口的方法
1. AccessibleObject 类的定义 ：public class AccessibleObject extends Object implements AnnotatedElement.    它有子类 Executable 和 Field
2. 获取全部 Annotation:  public Annotation[] getAnotations
3. 获取指定 Annotation:  public <T extends Annotation> T getAnnotation(Class<T> annotationClass)
```java
package cn.mldn.demo;

import java.io.Serializable;
import java.lang.annotation.Annotation;

@FunctionalInterface
@Deprecated
interface IMessage {
	public void send();
}


@SuppressWarnings("serial")     // 无法在程序执行的时候获取
class Message implements IMessage, Serializable{
	@Override                  // 无法在程序执行的时候获取
	public void send() {
		System.out.println("[发送消息] www.mldn.cn");
	}
}

public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		{
			Annotation [] annotations = IMessage.class.getAnnotations();
			for (Annotation a:annotations) {
				System.out.println(a);
			}
		}
		{
			Annotation [] annotations = Message.class.getAnnotations();
			for (Annotation a:annotations) {
				System.out.println(a);
			}
		}
		{
			Annotation [] annotations = Message.class.getDeclaredMethod("send").getAnnotations();
			for (Annotation a:annotations) {
				System.out.println(a);
			}
		}
	}
	
}
```
以上输出结果
```
@java.lang.FunctionalInterface()
@java.lang.Deprecated()
```
不同的 Annotation 有它的存在范围，对比两个接口 @FunctionalInterface 和 @SuppressWarnings("serial")
1. 两者定义
```java
@Documented
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface FunctionalInterface {}

@Target({TYPE, FIELD, METHOD, PARAMETER, CONSTRUCTOR, LOCAL_VARIABLE})
@Retention(RetentionPolicy.SOURCE)
public @interface SuppressWarnings
```
@FunctionalInterface 是在运行时生效的, 所以当程序执行的时候可以获取此 Annotation.   @SuppressWarnings 是在源代码编写的时候有效的

RetentionPolicy 枚举类中还有一个 Class 的定义，指的是在类定义的时候生效


# 实现自定义的 Annotation
使用 @interface 来定义新的 Annotation
```java
package cn.mldn.demo;

import java.io.Serializable;
import java.lang.annotation.Annotation;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.reflect.Method;

@Retention(RetentionPolicy.RUNTIME)
@interface DefaultAnnotation {
	public String title();
	public String url() default "www.mldn.cn";
}

@FunctionalInterface
@Deprecated
interface IMessage {
	public void send(String msg);
}


@SuppressWarnings("serial")     // 无法在程序执行的时候获取
class Message implements IMessage, Serializable{
	@DefaultAnnotation(title="MLDN")
	@Override
	public void send(String msg) {
		System.out.println("[发送消息]" + msg);
	}
}

public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		Method method = Message.class.getDeclaredMethod("send", String.class);
		DefaultAnnotation annotation = method.getAnnotation(DefaultAnnotation.class);
		String msg = annotation.title() + "(" + annotation.url() + ")";
		method.invoke(Message.class.getDeclaredConstructor().newInstance(), msg);
	}
	
}

```
使用annotation之后的最大特点是可以结合反射机制进行程序处理


# 工厂设计模式与 Annotation结合
```java
package cn.mldn.demo;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

interface IMessage {
	public void send(String msg);
}

class Message implements IMessage{
	@Override
	public void send(String msg) {
		System.out.println("[消息发送]" + msg);
	}
}

class NetMessage implements IMessage {
	@Override
	public void send(String msg) {
		System.out.println("[网络消息发送]" + msg);
	}
	
}

@Retention(RetentionPolicy.RUNTIME)
@interface UseMessage {
	public Class<?> clazz();
}

@UseMessage(clazz = Message.class)
class MessageService {
	private IMessage message;
	public MessageService() {
		this.message = (IMessage)Factory.getInstance(MessageService.class.getAnnotation(UseMessage.class).clazz());
	}
	public void send(String msg) {
		this.message.send(msg);
	}
}

class Factory {
	private Factory() {}
	public static <T> T getInstance(Class<T> clazz) {
		try {
			return (T) new MessageProxy().bind(clazz.getDeclaredConstructor().newInstance());
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}
}

class MessageProxy implements InvocationHandler {
	private Object target;
	
	public Object bind(Object target) {
		this.target = target;
		return Proxy.newProxyInstance(target.getClass().getClassLoader(), target.getClass().getInterfaces(), this);
	}
	
	public boolean connect() {
		System.out.println("[代理操作]进行消息发送通道的连接");
		return true;
	}
	
	public void close() {
		System.out.println("[代理操作]关闭连接通道");
	}

	@Override
	public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
		try {
			if (this.connect())
				return method.invoke(this.target, args);
			else throw new Exception("[Error]消息连接失败");
		} finally {
			this.close();
		}
	}
}

public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		MessageService service = new MessageService();
		service.send("hello, world");
	}
	
}
```