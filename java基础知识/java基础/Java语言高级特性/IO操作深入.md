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



# Java 中的 IO 流

节点流： 可以从或向一个特定的地方（节点）读写数据。如 FileReader

处理流：是对一个已存在的流的连接和封装，通过所封装的流的功能调用实现数据读写。如 BufferedReader。处理流的构造方法总是要带一个其他的流对象做参数。

## Java 中的 IO 流分为几种
1. 按照流的流向分，可以分为输入流和输出流
2. 按照操作单元划分， 可以划分为字节流和字符流
3. 按照流的角色划分可以分为节点流和处理流

按操作方式分类
![](assets/IO操作分类结构图.png)

按操作对象分类
![](assets/IO操作对象分类图.png)

## BIO, AIO, NIO 有什么区别
1. BIO (Blocking I/O): 同步阻塞 I/O 模式，数据的读取写入必须阻塞在一个线程内等待其完成。在活动连接数不是特别高（小于单机1000）的情况下，这种模型是比较不错的，可以让每一个连接专注于自己的 I/O 并且编程模型简单，也不用过多考虑系统的过载、限流等问题。线程池本身就是一个天然的漏斗，可以缓冲一些系统处理不了的连接或请求。但是，当面对十万甚至百万级连接的时候，传统的 BIO 模型是无能为力的。因此，我们需要一种更高效的 I/O 处理模型来应对更高的并发量。
2. NIO (New I/O): NIO 是一种同步非阻塞的 I/O 模型，在 Java 1.4 中引入了NIO框架，对应 java.nio 包， 提供了 Channel， Selector， Buffer 等抽象。 NIO 中的 N 可以理解为 Non-blocking， 不单纯是 New。 它支持面向缓冲的，基于通道的 I/O 操作方法。 NIO提供了与传统BIO模型中的 Socket 和 ServerSocket 相对应的 SocketChannel 和 ServerSocketChannel 两种不同的套接字通道实现,两种通道都支持阻塞和非阻塞两种模式。阻塞模式使用就像传统中的支持一样，比较简单，但是性能和可靠性都不好；非阻塞模式正好与之相反。对于低负载、低并发的应用程序，可以使用同步阻塞I/O来提升开发速率和更好的维护性；对于高负载、高并发的（网络）应用，应使用 NIO 的非阻塞模式来开发
3. AIO (Asynchronous I/O): AIO 也就是 NIO 2。在 Java 7 中引入了 NIO 的改进版 NIO 2,它是异步非阻塞的IO模型。异步 IO 是基于事件和回调机制实现的，也就是应用操作之后会直接返回，不会堵塞在那里，当后台处理完成，操作系统会通知相应的线程进行后续的操作。AIO 是异步IO的缩写，虽然 NIO 在网络操作中，提供了非阻塞的方法，但是 NIO 的 IO 行为还是同步的。对于 NIO 来说，我们的业务线程是在 IO 操作准备好时，得到通知，接着就由这个线程自行进行 IO 操作，IO操作本身是同步的。查阅网上相关资料，我发现就目前来说 AIO 的应用还不是很广泛，Netty 之前也尝试使用过 AIO，不过又放弃了。