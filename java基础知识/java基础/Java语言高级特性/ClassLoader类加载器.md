CLASSPATH 主要作用是在JVM启动的时候进行类路径的加载定义。

在JVM里面可以根据类加载器而后进行指定路径中类的加载

# 系统类的加载器
如果说现在要想获得类加载器，那么一定要通过ClassLoader来获取，而要想获取ClassLoader类对象，则必须利用Class类实现。方法: public ClassLoader getClassLoader(); 当获取了ClassLoader之后还可以继续获取其父类的ClassLoader类对象

```java
package cn.mldn.demo;

class Message {}

public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		Class<?> clazz = Message.class;
		System.out.println(clazz.getClassLoader());                     //sun.misc.Launcher$AppClassLoader@73d16e93
		System.out.println(clazz.getClassLoader().getParent());         //sun.misc.Launcher$ExtClassLoader@15db9742
		System.out.println(clazz.getClassLoader().getParent().getParent());  //null
	}
	
}

```

1. AppClassLoader  应用程序加载器
2. PlatformClassLoader   平台类加载器 （JDK1.8及以前是扩展类加载器 （ExtClassLoader））
3. Bootstrap (看不见)    系统类加载器

从JDK1.8之后的版本，提供有一个 PlatformClassLoader 类加载器，而在 JDK 1.8及以前提供的是 ExtClassLoader, 因为在JDK的安装目录中提供有一个ext目录，开发者可以将*.jar文件直接拷贝到此目录里面，这样就可以直接执行了，这种处理开发并不安全，同时为了与系统类加载器和应用类加载器之间保持设计的平衡，提供有平台类加载器

当你获得了类加载器之后就可以利用类加载器来实现类的反射加载处理。

# 自定义类加载器
自定义类加载器的加载顺序是在所有系统类加载器的最后。

系统类中的类加载器都是根据CLASSPATH找到的路径进行类加载，如果有自定义的类加载器，开发者就可以任意指派类的加载位置。

将字节转换为类 protected final Class<?> defineClass(String name, byte[] b, int off, int len) throws ClassFormatError

自定义类的加载器使用如下
1. 自定义类Message.java 然后编译成.class文件:
```java
package cn.mldn.util;
public class Message {
	public void send() {
		System.out.println();
	}
}
```
2. 自定义类加载器如下：
```java
package cn.mldn.util;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

public class MLDNClassLoader extends ClassLoader {
	private static final String MESSAGE_CLASS_PATH = "C:\\Users\\estelle\\eclipse-workspace\\FirstProject\\src\\cn\\mldn\\util\\Message.class";
	
	/**
	 * 指定类的加载
	 * @param className 类的完整名称"包.类"
	 * @return 返回一个指定类的Class对象
	 * @throws Exception
	 */
	public Class<?> loadData(String className) throws Exception {
		byte [] data = this.loadClassData();
		if (data != null) {
			return super.defineClass(className, data, 0, data.length);
		}
		return null;
	}
	
	public byte [] loadClassData() throws Exception {                     //通过文件进行类的加载
		InputStream input = null;
		ByteArrayOutputStream bos = null;
		byte [] res = null;
		try {
			input = new FileInputStream(new File(MESSAGE_CLASS_PATH));
			bos = new ByteArrayOutputStream();
			byte[] datas = new byte[1024];
			int len = 0;
			while ((len = input.read(datas)) != -1) {
				bos.write(datas, 0, len);
			}
			res = bos.toByteArray();
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (input != null) input.close();
			if (bos != null) bos.close();
		}
		return res;
	}
}
```
3. 测试如下:
```java
package cn.mldn.demo;

import cn.mldn.util.MLDNClassLoader;

public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		MLDNClassLoader classLoader = new MLDNClassLoader();
		Class<?> cls = classLoader.loadData("cn.mldn.util.Message");
		System.out.println(cls);
	}
	
}
```

如果在以后结合网络程序开发的话，就可以通过一个远程的服务器来确定类的功能。

如果现在定义个一类，这个类的名字为： java.lang.String, 并且利用了自定义的类加载器，这个类将不会被加载，Java之中针对于类加载器提供有双亲加载机制。如果现在要加载的程序类是由系统提供的类则会由系统类进行加载。