Annocation是从JDK1.5后提供的一个新的开发技术结构，利用Annotation可以有效的减少程序配置，利用Annotation进行一些结构化的定义。Annotation是以一种注解的形式实现的程序的开发。

程序开发过程
1. 在程序定义的时候将所有可能使用到的资源全部定义在程序代码中

如果此时服务器的相关地址发生了改变，那么对于程序而言就需要进行源代码的修改，维护需要开发人员去完成

2. 引入配置文件，在配置文件之中定义全部要使用的服务器资源

在配置项不多的时候，非常好用，如果所有项目都采用这种方式，那么配置文件会暴多，所有的操作都需要通过配置文件完成，这样对于开发的难度提高了

3. 将配置信息重新写回到程序里面，利用一些特殊的标记与程序代码进行分离，这就是注解的作用
   
如果全部都使用注解开发，难度太高了，配置文件有好处也有缺点，现在使用配置文件加注解的形式完成

Java中提供的几个注解:@Override、@Deprecated、@SuppressWarnings

# @Override
当子类继承父类之后，如果发现父类中的某些方法功能不足往往会采用覆写的操作对方法功能扩充

可以避免子类扩写父类方法方法名写错 @Override明确地表明该方法为覆写方法，主要是帮助程序在编译时候，发现程序地的错误

# @Deprecated
所谓的过期操作指的是在一个软件项目的迭代开发过程中，可能有某一个方法或是某一个类，在最初设计时有缺陷，导致新版本的应用有不适应的地方（老版本不影响），但又不能直接删除该操作，就给一个过渡的时间，于是就可以采用过期的声明，目的是告诉新的用户这些操作不要再用了，旧用户还可以继续使用，需用@Deprecated注解声明定义
```java
class Channel {
    @Deprecated                                          //老系统继续用，新系统不用
    public void connect() {
        System.out.println("**************************** Channel *********************************");
    }
    public String connection() {
        return "获取了Xxx通道连接信息";
    }
}
public class JavaDemo {
    public static void main(String [] args) {
        new Channel().connect();
    }
}
```

# @SupressWarnings 压制警告
以上述@Deprecated里的程序可以发现在进行程序编译的时候会出现一个错误的提示信息：
```
注: JavaDemo.java使用或覆盖了已过时的API
注：有关详细信息，请使用-Xlint:deprecation重新编译
```
如果此时不愿意见到这些提示信息（或者已经明确的知道了错误在哪了），那么就可以进行警告信息的压制，如下
```java
class Channel {
    @Deprecated                                          //老系统继续用，新系统不用
    public void connect() {
        System.out.println("**************************** Channel *********************************");
    }
    public String connection() {
        return "获取了Xxx通道连接信息";
    }
}
public class JavaDemo {
    @SuppressWarnings({"deprecation"})
    public static void main(String [] args) {
        new Channel().connect();
    }
}
```
它做的只是让警告信息不出现

