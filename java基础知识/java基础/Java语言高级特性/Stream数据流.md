因为进入到了大数据时代，所以在类集里面也支持有数据的流式分析处理操作, 所以提供了 Stream 接口，同时在Collection接口里面提供了有为此实例化的方法

Collections 提供有如下两个方法
1. default Stream<E> parallelStream()  获取 并行Stream 接口对象（多个线程参与分析）
2. default Stream<E> Stream()          获取 Stream 接口对象 （一个线程参与分析）

# Stream 基础操作
Stream 主要功能是进行数据的分析处理，同时主要是针对集合中的数据进行分析操作。
```java
public class JavaAPIDemo {
	
	public static void main(String[] args) {
		List<String> all = new ArrayList<String>();
		Collections.addAll(all, "Java", "JavaScript", "Python", "Rudy", "Go");
		Stream<String> stream = all.stream();      
//		System.out.println(stream.count());
		System.out.println(stream.filter((ele)->ele.toLowerCase().contains("j")).count());
	}
	
}
```
但是以上的程序只实现了数据的个数统计，但是更多情况下，需要获取满足数据条件的内容，实现数据的采集操作。
```java
public class JavaAPIDemo {
	
	public static void main(String[] args) {
		List<String> all = new ArrayList<String>();
		Collections.addAll(all, "Java", "JavaScript", "Python", "Rudy", "Go");
		Stream<String> stream = all.stream();      
//		System.out.println(stream.count());
		List<String> list = stream.filter((ele)->ele.toLowerCase().contains("j")).collect(Collectors.toList());
		System.out.println(list);
	}
	
}
```
在Stream 数据流处理的过程之中，还允许数据的分页处理，提供有两个方法
1. public Stream<T> limit(long maxSize) 设置取出的最大的数据量
2. Stream<T> skip(long n)               跳过指定数据
```java
public class JavaAPIDemo {
	
	public static void main(String[] args) {
		List<String> all = new ArrayList<String>();
		Collections.addAll(all, "Java", "JavaScript", "JSP", "Json", "Python", "Rudy", "Go");
		Stream<String> stream = all.stream();      
//		System.out.println(stream.count());
		List<String> list = stream.filter((ele)->ele.toLowerCase().contains("j")).skip(2).limit(2).collect(Collectors.toList());
		System.out.println(list);
	}
	
}
```
输出为： [JSP, Json]


## 为什么需要 Stream ？
Stream 作为 Java 8 的一大亮点， 它与 Java.io 包里的 InputStream 和 OutputStream 是完全不同的概念。 它也不同于StAX 对 XML解析的Stream，也不是 Amazon Kinesis 对大数据实时处理的 Stream。 Java 8 中的Stream是对集合（Collection）对象功能的加强，它专注于对集合对象进行各种非常便利、高效的聚合操作（aggregate operation），或者大批量数据操作（bulk data operation）。Stream API 借助于同样出现的 Lamda 表达式，极大的提高编程效率和程序可读性。同时它提供串行和并行两种模式进行汇聚操作，并发模式能够充分利用多核处理器的优势，使用 fork/join 并行方式来拆分任务和加速处理过程。通常编写并行代码很难而且容易出错，但使用 Stream API 无需编写一行多线程的代码，就可以很方便地写出高性能地并发程序。所以说， Java 8 中首次出现地java.util.Stream 是一个函数式语言 + 多核时代综合影响的产物

## 什么是聚合操作
在传统的 J2EE 应用中， Java 代码经常不得不依赖于关系型数据库的聚合操作完成诸如：
1. 客户每月平均消费金额
2. 最昂贵的在售商品
3. 本周完成的有效订单（排除了无效的）
4. 取十个数据样本作为首页推荐

这类操作

但是在当今这个数据大爆炸的时代，在数据来源多样化、数据海量化的今天，很多时候不得不脱离RDBMS， 或者以底层返回的数据为基础进行更上层的数据统计。而 Java 的集合 API 中，仅仅有极少量的辅助型方法，更多的时候是程序员需要用 Iterator 来遍历集合，完成相关的聚合应用逻辑。这是一种远不够高效、笨拙的方法。

## Stream 总览
### 什么是流
Stream 不是集合元素，它不是数据结构并不保存数据，它是有关算法和计算的，它更像一个高级版本的 Iterator。原始版本的 Iterator， 用户只能显式地一个一个遍历元素并对其执行某些操作；高级版本的 Stream，用户只要给出需要对其包含的元素执行什么操作，比如“过滤掉长度大于10的字符串”、“获取每个字符串的首字母”等， Stream 会隐式的在内部遍历，做出相应的数据转换。

Stream就如同一个迭代器（Iterator）单向，不可往复，数据只能遍历一次，遍历过一次后即用尽了，

而和迭代器又不同的是， Stream可以并行化操作，迭代器只能命令地、串行化操作。顾名思义，当使用串行方式去遍历时，每个 item 读完后再读下一个 item。而使用并行去遍历时，数据会被分成多个段，其中每一个都在不同的线程中处理，然后将结构一起输出。Stream 的并行操作依赖于 Java7 中引入的 Fork/Join 框架（JSR166y）来拆分任务和加速处理过程。Java 的并行 API 演变历程基本如下：
1. 1.0-1.4 中的 java.lang.Thread
2. 5.0 中的 java.util.concurrent
3. 6.0 中的 Phasers 等
4. 7.0 中的 Fork/Join 框架
5. 8.0 中的 Lambda

Stream 的另外一大特点是，数据源本身可以是无限的。

### 流的构成
当我们使用一个流的时候，通常包括三个基本步骤： 获取一个数据源（source），数据转换， 执行操作获取想要的结果。

每次转换原有 Stream 对象不改变，返回一个新的 Stream 对象（可以有多次转换），这就允许对其操作可以像链条一样排列，变成一个管道，

有多种方式生成 Stream Source
1. Collection.stream();
2. Collection.parallelStream()
3. Arrays.stream(T array) or Stream.of()
4. java.io.BufferedReader.lines()
5. java.util.stream.IntStream.range()
6. java.nio.file.Files.walk()
7. java.util.Spliterator
8. Random.ints()
9. BitSet.stream()
10. Pattern.splitAsStream(java.lang.CharSequence)
11. JarFile.stream()

# MapReduce
在进行数据分析的处理之中， 有一个最重要的基础模型： MapReduce 模型，对于这个模型一共是分为两个部分， Map 处理部分， Reduce分析部分
```java
public class JavaAPIDemo {
	
	public static void main(String[] args) {
		List<Order> all = new ArrayList<Order>();
		all.add(new Order("小强娃娃", 9.9, 10));
		all.add(new Order("手机", 2000, 3));
		all.add(new Order("不强牌笔记本电脑", 8000, 8));
		all.add(new Order("若强牌茶杯", 2.9, 800));
		all.add(new Order("煎饼", 0.9, 138));
		
		DoubleSummaryStatistics stat = all.stream().filter((ele)->{
			return ele.getName().contains("强");
		}).mapToDouble((ele)->{
			return ele.getPrice() * ele.getAmount();
		}).summaryStatistics();
		
		System.out.println("商品购买数量:" + stat.getCount());
		System.out.println("商品购买总价: " + stat.getSum());
		System.out.println("平均花费: " + stat.getAverage());
		System.out.println("最低花费: " + stat.getMin());
		System.out.println("最高花费: " + stat.getMax());
	}
	
}
```


