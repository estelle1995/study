在实际的开发之中对于常用的编码有如下几种：
1. GBK/GB2312:  国标编码，可以描述中文信息，其中GB2312只描述简体中文，而GBK包含简体中文和繁体中文
2. ISO8859-1:   国际通用编码，可以用其描述所有的文字信息
3. UNICODE 编码： 采用十六进制的方式存储，可以描述所有的字母信息。
4. UTF 编码：     象形文字部分使用的十六进制编码，而普通的字母采用 ISO8859-1 的编码

如果想要知道本机支持的编码规则，则可以采用如下代码列出全部的本机属性：
```java
public class JavaAPIDemo {

	public static void main(String[] args) {
		System.getProperties().list(System.out);
	}
}
```


强制性设置编码
```java
OutputStream output = new FileOutputStream(filepath);
output.write("中华人民共和国".getBytes("ISO8859-1"));
output.close();
```

项目中乱码的问题就是编码和解码标准不统一，而最好的解决乱码的方式，所有的编码都用 UTF-8

# 内存操作流
假设说现在需要实现IO操作，可是又不希望产生文件（临时文件），则可以以内存为中断进行处理

在 Java 里面提供有两类的内存操作流
1. 字节内存操作流：   ByteArrayOutputStream, ByteArrayInputStream
2. 字符内存操作流:    CharArrayWriter, CharArrayReader

## ByteArrayInputStream
定义：   public class ByteArrayInputStream extends InputStream

构造方法：public ByteArrayInputStream(byte[] buf)


## ByteArrayOutputStream
定义：   public class ByteArrayOutputStream extends OutputStream

构造方法：public ByteArrayOutputStream（）

获取数据
1. public byte[] toByteArray()    必须使用ByteArrayOutputStream该类的对象调用该方法，因为此方法是 ByteArrayOutputStream 类的扩展方法， OutputStream没有此方法
2. public String toString()


范例，利用内存流实现小写字母转大写字母的操作
```java
package cn.mldn.demo;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class JavaAPIDemo {

	public static void main(String[] args) {
		String str = "www.mldn.cn";
		InputStream input = new ByteArrayInputStream(str.getBytes());
		ByteArrayOutputStream output = new ByteArrayOutputStream();
		int data = 0;
		try {
			int off = 0;
			while ((data = input.read()) != -1) {
				output.write(Character.toUpperCase((char)data));
			}
			byte [] datas = output.toByteArray();
			System.out.println(new String(datas));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
```



# 管道流
实现两个线程之间的IO处理操作
1. 字节管道流   PipedOutputStream, PipedInputStream
2. 字符管道流   PipedWriter, PipedReader

字节管道流连接处理： public void connect(PipedInputStream snk) throws IOException;

字符管道流连接处理： public void connect(PipedReader snk) throws IOException;

实现管道操作
```java
package cn.mldn.demo;

import java.io.IOException;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;

class SendThread implements Runnable {
	private PipedOutputStream output;
	
	public SendThread() {
		this.output = new PipedOutputStream();
	}

	@Override
	public void run() {	
		try {
			this.output.write(("消息发送-" + Thread.currentThread().getName() + " www.mldn.cn\n").getBytes());
			this.output.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public PipedOutputStream getOutput() {
		return this.output;
	}
}

class RecievedThread implements Runnable {
	private PipedInputStream input;
	
	public RecievedThread() {
		this.input = new PipedInputStream();
	}

	@Override
	public void run() {
		try {
			int len = 0;
			byte [] datas = new byte[1024];
			while ((len = input.read(datas)) != -1) {
				System.out.println(Thread.currentThread().getName() + "接收消息{" + new String(datas, 0, len) +"}\n");
			}
			input.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public PipedInputStream getInput() {
		return this.input;
	}
	
}

public class JavaAPIDemo {

	public static void main(String[] args) {
		SendThread send = new SendThread();
		RecievedThread recieved = new RecievedThread();
		try {
			send.getOutput().connect(recieved.getInput());
			new Thread(send, "消息发送线程").start();
			new Thread(recieved, "消息接收线程").start();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

```

# 随机读取类 RandomAccessFile
可以实现文件的跳跃式读取（需要有一个完善的保存形式），数据的保存位数要都确定好

构造方法
1. public RandomAccessFile(File file, String mode) throws FileNotFoundException
2. public RandomAccessFile(String name, String mode) throws FileNotFoundException

文件处理模式， r,  rw , rws, rwd

1. 向后跳n个字节： public int skipBytes(int n) throws IOException
2. 设置从位置pos开始读取： public void seek(long pos) throws IOException
