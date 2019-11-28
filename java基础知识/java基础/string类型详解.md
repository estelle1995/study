JDK1.8以前String保存的是字符数组，从JDK1.9开始String保存的是字节数组

# 字符串比较
 字符串的相等判断 public boolean equals(String str);

# 字符串常量（字符串匿名对象）
所有用字符串匿名对象初始化的变量都指向同一个内存空间：主要原因是因为在Java程序的底层里面提供有一个专门的字符串池（字符串引用表）


# 分析构造方法实例化
```java
String str = new String("mldn")
```
在常量池中查找是否有”abc”对象，有则返回对应的引用实例，没有则创建对应的实例对象(1个)在堆中 new 一个 String(“abc”) 对象(1个)将对象地址赋值给str，创建一个引用

可以发现构造方法实例化的对象实际上是属于一种自己专用的内存空间，但在String类里面提供有手工入池的处理情况，如下：
```java
String strA = "mldn";
String strB = new String("mldn").intern();
System.out.println(strA == strB);                               //true
```


# String 对象常量池
Java之中对象（常量）池实际上分为两种
1. 静态常量池：指的是程序(.class)在加载的时候会自动将此程序之中保存的字符串，普通的常量（1，2）类和方法等等，全部进行分配
2. 运行时常量池: 当一个程序（.class）加载之后，里面可能有一些变量，（需要用户来输入，输入之后再进行接收），这个时候提供的常量池叫做运行时常量池

## 静态常量池
```java
String strA = "www.mldn.cn";
String strB = "www." + "mldn" + ".cn";
System.out.println(strA == strB);                        //true
```


## 运行时常量池
```java
String info = "mldn";
String strA = "www.mldn.cn";
String strB = "www." + info + ".cn";
System.out.println(strA == strB);                    //false
```
这个时候之所以是一个false，是因为程序在加载的时候并不确定info是什么内容。因为在字符串连接的时候info是一个变量，变量的内容是可以修改的，所以它不认为最终的strB的结果就是一个所需要最终的结果


# 字符串长度不可修改
在String类之中包含的是一个数组，数组的最大缺点在于长度不可改变，当设置一个字符串时，会自动开辟一个数组空间，该长度固定。

```java
String str = "www.";
str += "mldn";
str += str + "cn";
```

# JavaDoc简介
在以后进行开发的过程之中肯定要大量的去使用Java的API文档(JavaDoc),这个文档可以直接通过Oracle的在线访问进行查看。https://docs.oracle.com/javase/8/docs/api/

在JDK1.9之前，所有的Java中的常用类库都会在JVM启动的时候进行全部的加载，这样的话性能实际上会有下降。JDK1.9之后提供有模块化的设计，将一些程序类放在不同的模块内。

# 字符串与字符数组
1. public String(char[] value)
2. public String(char[] value, int offset, int count) offset开始的位置，count个数，将部分字符数组变为字符串
3. char charAt(int index) 获取指定索引位置的字符
4. public char[] toCharArray() 将字符串转换为字符数组

## 字符串与字节数组
1. public String(byte[] bytes)
2. public String(byte[] bytes, int offset, int length)
3. public byte[] getBytes()    将字符串转为字节数组
4. public byte[] getBytes(String charsetName) throws UnsupportedEncodingException    编码转换


## 字符串比较
1. public boolean equals(Object anObject)
2. public boolean equalsIgnoreCase(String anotherString)  不区分大小写比较
3. public int compareTo(String anotherString) 进行字符串大小比较
4. public int compareToIgnoreCase(String str) 

## 字符串查找
1. public boolean contains(CharSequence s) 判断子字符串是否存在    (jdk1.5之前该方法不存在 )
2. public int indexOf(String str) 从头查找字符串的位置，找不到返回-1
3. public int indexOf(String str, int fromIndex) 从指定位置开始查找字符串的位置
4. public int lastIndexOf(String str) 由后向前查找指定字符串的位置
5. public int lastIndexOf(String str, int fromIndex) 从指定位置由后向前查找指定字符串的位置
6. public boolean startsWith(String prefix) 判断是否已指定的字符串开头
7. public boolean startsWith(String prefix, int toffset)
8. public boolean endsWith(String suffix)

## 字符串替换
1. public String replaceAll(String regex, String replacement)
2. public String replaceFirst(String regex, String repacement)

## 字符串拆分
1. public String[] split(String regex)
2. public String[] split(String regex, int limit) 按照指定字符串拆分为指定个数的字符串数组

## 字符串截取
1. public String substring(int beginIndex)
2. public String substring(int beginIndex, int endIndex)  

## 字符串格式化
从JDK1.5开始为了吸引更多的传统开发人员，Java提供有了格式化数据的处理操作，类似于可以利用占位符实现数据的输出，对于占位符而言，常用的：字符串(%s),字符(%c),z整数(%d),小数(%f)
1. static String format(String format, Object... args)     根据指定结构进行格式化显示
   
## 其他操作方法
1. public String concat(String str)
2. public String intern() 字符串入池
3. public boolean isEmpty() 判断是否为空字符串(不是null,一定要在有实例化对象时调用)
4. public int length()      计算字符串的长度
5. public String trim() 去除字符串左右的空格
6. public String toUpperCase()  转大写
7. public String toLowerCase()  转小写 

String类没有提供首字母大写的方法