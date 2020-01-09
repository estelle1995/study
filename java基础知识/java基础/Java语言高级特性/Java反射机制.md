Object中有获取 Class 对象信息: public final Class<?> getClass();

反射之中所有的核心操作都是通过 Class 类对象展开的， 可以说 Class 类是反射操作的根源所在，这个类要想获取它的实例化对象，可以采用三种方式完成

java.lang.Class类的定义
```java
public final class Class<T> extends Object implements Serializable, GenericDeclaration, Type, AnnotatedElement
```
从JDK1.5开始，Class类在定义的时候可以使用泛型进行标记，这样的用法主要是希望可以避免所谓的向下转型。

1. [Object 类支持] Object 类可以根据实例化对象获取 Class 对象。如果现在只是想获得Class对象，必须产生指定类对象的实例
2. [JVM 直接支持]  类.class
3. [Class 类支持] 在Class类中提供有一个静态方法： public static Class<?> forName(String className) throws ClassNotFoundException; 


# 反射实例化对象
从JDK1.9之前反射实例化对象的方法是 public T newInstance() throws InstantiationException, IllegalAccessException

在JDK1.9之后，被替换成 clazz.getDeclaredConstructor().newInstance();


## 反射与工厂设计模式
见 (工厂模式.md) 文档中 使用反射机制改善简单工厂, 如果一个工厂有多个接口实现则用下述方法
```java
package cn.mldn.demo;

interface IMessage {
	void send();
}

class NetMessage implements IMessage {
	@Override
	public void send() {
		System.out.println("NetMessage");
	}
}

interface IService {
	public void service();
}

class HouseService implements IService {
	@Override
	public void service() {
		System.out.println("HouseService");
	}
}

class Factory {
	private Factory() {}
	public static <T> T getInstance(Class<? extends T> clazz, Class<T> iClazz) {
		T instance = null;
		try {
			instance = clazz.newInstance();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return instance;
	}
}
public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		IMessage msg = Factory.getInstance(NetMessage.class, IMessage.class);
		msg.send();
	}
	
}

```

## 反射与单例设计模式
```java
public class Singleton {
    private volatile static Singleton singleton;
    private Singleton(){}
    public static Singleton getSingleton() {
        if (singleton == null) {
            synchronized(Singleton.class) {
                if (singleton == null) {
                    singleton = new Singleton();
                }
            }
        }
        return singleton;
    }
}
```

# 反射与类操作
在反射机制的处理过程之中不仅仅只是一个实例化对象的处理操作，更多的情况下，还有类的基本组成结构：父类(父接口)、包、属性、方法(构造方法、普通方法)

## 获取类的基本信息
一个类的基本信息主要包括类所在的包名称、父类的定义、父接口的定义

1. 获取包名称    public Package getPackage();
2. 获取父类      public Class<? super T> getSuperClass();
3. 获取父接口    public Class<?>[] getInterfaces()

```java
package cn.mldn.demo;

interface IMessageService{
	public void send();
}

interface IChannelService {
	public boolean connect();
}

abstract class AbstractBase {}

class Person extends AbstractBase implements IMessageService, IChannelService {

	@Override
	public boolean connect() {
		return true;
	}

	@Override
	public void send() {
		if (this.connect()) {
			System.out.println("[信息发送]www.mldn.cn");
		}
	}
	
}

public class JavaAPIDemo {
	
	public static void main(String[] args) {
		Class<?> cls = Person.class;
		Package pack = cls.getPackage();
		Class<?> per_super = cls.getClass();
		Class<?>[] interfaces = cls.getInterfaces();
		System.out.println(pack.getName());
		System.out.println(per_super.getName());
		for (int i = 0; i < interfaces.length; i++) {
			System.out.println(interfaces[i].getName());
		}
	}
	
}
```
当获取一个类的class对象之后就意味着这个对象可以获取类之中的一切继承结构信息


## 反射调用构造方法
clzz.getDeclaredConstructor().newInstance();

1. 获取所有构造方法：public Constructor<?>[] getDeclaredConstructors() throws SecurityException
2. 获取指定构造方法：public Constructor<T> getDeclaredConstructor(Class<?>...parameterTypes) throws NoSuchMethodException, SecurityException
3. 获取所有构造方法：public Constructor<?>[] getConstructors() throws SecurityException
4. 获取指定构造方法：public Constructor<T> getConstructor(Class<?>...parameterTypes) throws NoSuchMethodException, SecurityException

Constructor 类中有方法 public T newInstance(Object... initargs) throws InstantiationException, IllegalAccessException, IllegalArgumentException,InvocationTargetException

getDeclaredConstructor 可以获得类的私有构造方法，  getConstructor 不可以


```java
package cn.mldn.demo;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

interface IMessageService{
	public void send();
}

interface IChannelService {
	public boolean connect();
}

abstract class AbstractBase {
	public AbstractBase() {}
	public AbstractBase(String msg) {}
}

class Person extends AbstractBase implements IMessageService, IChannelService {
	private String name;
	private int age;
	
	public Person() {}
	
	public Person(String name, int age) {
		this.name = name;
		this.age = age;
	}

	@Override
	public boolean connect() {
		return true;
	}

	@Override
	public void send() {
		if (this.connect()) {
			System.out.println("[信息发送]www.mldn.cn");
		}
	}
	
	public String toString() {
		return "姓名：" + name + ", 年龄：" + age;
	}
	
}

public class JavaAPIDemo {
	
	public static void main(String[] args) {
		Class<?> cls = Person.class;
		Constructor<?> [] constructors = cls.getDeclaredConstructors();
		try {
			Constructor<?> constructor = cls.getDeclaredConstructor(String.class, int.class);
			Object obj;
			try {
				obj = constructor.newInstance("小强", 15);
				System.out.println(obj);
			} catch (InstantiationException e) {
				e.printStackTrace();
			} catch (IllegalAccessException e) {
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				e.printStackTrace();
			}
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
		} catch (SecurityException e) {
			e.printStackTrace();
		}
		for (Constructor<?> cons : constructors) {
			System.out.println(cons);
		}
	}
	
}
```

## 获取方法：
如果想要通过反射获取类中全部方法（除构造方法）必须有一个前提条件，类之中提供有实例化对象。

1. 获取全部方法           public Method[] getMethods() throws SecurityException
2. 获取指定方法           public Method getMethod(String name, Class<?>... parameterTypes) throws NoSuchMethodException, SecurityException
3. 获取本类全部方法       public Method[] getDeclaredMethods() throws SecurityException
4. 获取本类指定方法       public Method getDeclaredMethod(String name, Class<?>... parameterTypes) throws NoSuchMethodException, SecurityException

getMethod() 方法可以获得包括父类的所有方法, getDeclaredMathod() 方法只可以获得本类的所有方法

```java
public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		Class<?> cls = Person.class;
		{
			Method [] methods = cls.getMethods();
			for (Method met : methods) {
				System.out.println(met);
			}
		}
		System.out.println();
		{
			Method [] methods = cls.getDeclaredMethods();
			for (Method met : methods) {
				System.out.println(met);
			}
		}
	}
	
}
```

```java
public class JavaAPIDemo {
	
	@SuppressWarnings("rawtypes")
	public static void main(String[] args) throws Exception {
		Class<?> cls = Person.class;
		{
			Method [] methods = cls.getMethods();
			for (Method met : methods) {
				System.out.print(Modifier.toString(met.getModifiers()) + " ");
				System.out.print(met.getReturnType().getName() + " ");
				System.out.print(met.getName() + "(");
				Class<?> [] params = met.getParameterTypes();
				for (int x = 0; x < params.length; x++) {
					System.out.print(params[x].getName() + " " + "arg-" + x);
					if (x < params.length - 1) {
						System.out.print(", ");
					}
				}
				System.out.print(")");
				Class<?> [] exp = met.getExceptionTypes();
				for (int x = 0; x < exp.length; x++) {
					if (x == 0) {
						System.out.print(" throws ");
					}
					System.out.print(exp[x].getName());
					if (x < exp.length - 1) {
						System.out.print(", ");
					}
				}
				System.out.println();
			}
		}
	}
	
}
```

Method 类里面有一个重要的方法 public Object invoke(Object obj, Object...args) throws IllegalAccessException, IllegalArgumentException, InvocationTargetException    反射调用类中的方法

```java
public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		Class<?> cls = Person.class;
		String arribute = "name";          //要操作的类属性
		String value = "小强子";             //要设置的属性内容
		Object obj = cls.getDeclaredConstructor().newInstance();
		String setMethodName = "setName";
		Method setMethod = cls.getDeclaredMethod(setMethodName, String.class);
		setMethod.invoke(obj, value);
		String getMethodName = "getName";
		Method getMethod = cls.getDeclaredMethod(getMethodName);
		getMethod.invoke(obj);
	}
	
}

```
这样的处理避免了某一个类的耦合。


## 反射调用成员
1. 获取本类全部成员: public Field[] getDeclaredFields() throws SecurityException
2. 获取本类指定成员：public Field getDeclaredField(String name) throws NoSuchFieldException, SecurityException
3. 获取父类公共全部成员：    public Field[] getFields() throws SecurityException
4. 获取父类公共指定成员：    public Field getField(String name) throws NoSuchFieldException, SecurityException

```java
public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		Class<?> cls = Person.class;
		{
			//获取父类的公共成员信息
			Field [] fields = cls.getFields();
			for (Field field : fields) {
				System.out.println(field);
			}
		}
		System.out.println();
		{
			//获取子类的信息
			Field [] fields = cls.getDeclaredFields();
			for (Field field : fields) {
				System.out.println(field);
			}
		}
	}
	
}
```


Field 里的三个方法
1. 设置属性内容 public void set(Object obj, Object value) throws IllegalArgumentException, IllegalAccessException
2. 获取属性内容 public Object get(Object obj) throws IllegalArgumentException, IllegalAccessException
3. 解除封装    public void setAccessible(boolean flag)   该方法继承于AccessibleObject
4. 获取成员类型 public class<?> getType();


## sun.misc.Unsafe 工具
在Java 里面也提供有一个Unsafe 类(不安全的操作)，这个类的主要特点是可以利用反射来获取对象，并且直接使用底层的 C++ 来代替 JVM 执行，即可以绕过 JVM 的相关的对象的管理机制，如果你一旦使用了 Unsafe 类，那么你的项目之中将无法继续使用 JVM 的内存管理机制以及垃圾回收处理。

1. 构造方法         private Unsafe();
2. 私有常量         private static final Unsafe theUnsafe = new Unsafe();

在Unsafe类里面并没有提供static方法，即：不能够通过类似于传统的单例设计模式之中提供的样式来进行操作，要养获得这个类的对象，就必须利用反射机制来完成
```java
class Singleton {
	private Singleton() {}
	public void print() {
		System.out.println("Singleton");
	}
}

public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		Field field = Unsafe.class.getDeclaredField("theUnsafe");
		field.setAccessible(true);
		Unsafe unsafe = (Unsafe) field.get(null);  // static属性不需要传递实例化对象
		//利用Unsafe类绕过了JVM管理机制，可以在没有实例化对象的情况下获取一个Singleton类实例化对象
		Singleton instance = (Singleton)unsafe.allocateInstance(Singleton.class);
		instance.print();
	}
	
}
```
Unsafe 最大的特征绕过实例化对象的管理，为我们的开发提供了一些更加方便地处理机制，但是这种操作由于不受JVM的管理所以如果不是必须的情况下，不建议使用
