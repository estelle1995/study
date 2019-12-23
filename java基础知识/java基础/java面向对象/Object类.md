Object类的主要特点是可以解决参数的统一问题，也就是说使用Object类可以接收所有的数据类型

# Object类简介
所有的类的定义都是Object的子类

# Object 获取对象信息 toString()
```java
class Person {}
class JavaDemo {
    public void static void main(String [] args) {
        Person per = new Person();
        System.out.println(per);                           // Person@6dle7682
        System.out.println(per.toString());                // Person@6dle7682
    }
}
```
可以发现在之前进行对象直接输出的时候所调用的方法就是toString()方法，
 
# 对象比较 equals()
public boolean equals(Object obj)


