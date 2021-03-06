# Java数据类型简介
1. 基本数据类型: 描述的是一些具体的数字单元
2. 引用数据类型：牵扯到内存关系的使用

数据类型选择参考：
1. 如果要描述数字首选的一定是int、double
2. 如果要进行数据传输或者进行文字编码转换使用byte类型（二进制处理操作）
3. 处理中文的时候最方便的操作使用的是字符char来完成（可选概念）
4. 描述内存或文件大小、描述表的主键列（自动增长）可以使用long

所有数据类型转换自动从小类型转换成大类型。

## 基本数据类型
### 数值型
int型数值范围-2147483648到2147483647
1. 整型：byte(1)、short(2)、int(4)、long(8)； （默认值0）
2. 浮点型：float(4)、double(8)；         （默认值0.0）

#### 整型数据
```java
byte num = 20;
byte result = num * num;           //错误
```
以上语句会发生错误（不兼容的类型:从int型转换到byte可能会有损失）正常来将，20是int型，但是在为byte赋值时并没有做强制类型转换，是因为Java对byte类型做了特殊处理，如果每超过byte范围的常量可以自动由int转换成byte，如果超过就要强制转换

#### 浮点数型
所有的小数默认为double类

### 布尔型-boolean  （默认值false）

### 字符型-char (2)     （默认值'\u0000'）
在任何的编程语言中，字符都可以与int进行相互转换，java中的char类型可以保存中文数据
```java
char a = '仁'    //编码值为20161
```
之所以在Java中可以使用char进行中文数据的保存，是因为java使用的是unicode这种十六进制的编码，这种编码特点可以包括任意文字内容

## 引用数据类型
数组、类、接口        (默认值null)

### String 类
因为String类的存在比较特殊，所以可以像普通变量那样采用直接赋值的方式进行字符串的定义

# Java 字符串所占字节数小总结
首先，char为Java的基本类型，基本类型所占的字节数是固定的，如int占4字节，double占8字节，这可以使得Java在不同的平台上所占类型固定，很好地保证了Java的可移植性。因此，Java中char类型固定占2个字节。（注：char类型也可以存储一个汉字）。

其次，String采用一种更灵活的方式进行存储。在String中，一个英文字符占1个字节，而中文字符根据编码的不同所占字节数也不同。在UTF-8编码下，一个中文字符占3个字节；而使用GBK编码时一个中文字符占2个字节。

