# @staticmethod 和 @classmethod
Python其实有3个方法,即静态方法(staticmethod),类方法(classmethod)和实例方法,如下:

# python 中的单下划线和双下划线
```
__foo__: 一种约定，python 内部的名字，用来区别其他用户自定义的命名，以访冲突，就是例如 __init__(), __del__(), __call__() 这些特殊方法

_foo: 一种约定,用来指定变量私有.程序员用来指定私有变量的一种方式.不能用from module import * 导入，其他方面和公有一样访问；

__foo: 这个有真正的意义:解析器用_classname__foo来代替这个名字,以区别和其他类相同的命名,它无法直接像公有成员一样随便访问,通过对象名._类名__xxx这样的方式可以访问.
```

# 字符串格式化：% 和 .format

# 迭代器和生成器

# *args and **kwargs
用 *args 和 **kwargs 只是为了方便并没有强制使用它们。

当你不确定你的函数里将要传递多少参数时你可以用 *args. 例如，它可以传递任意数量的参数：

相似的， **kwargs 允许你使用没有事先定义的参数名：
```python
>>> def table_things(**kwargs):
...     for name, value in kwargs.items():
...         print '{0} = {1}'.format(name, value)
...
>>> table_things(apple = 'fruit', cabbage = 'vegetable')
cabbage = vegetable
apple = fruit
```

你也可以混着用.命名参数首先获得参数值然后所有的其他参数都传递给* args和 ** kwargs.命名参数在列表的最前端. * args 和** kwargs可以同时在函数的定义中,但是*args必须在**kwargs前面.

# 面向切面编程 AOP 和装饰器
装饰器是一个很著名的设计模式，经常被用于有切面需求的场景，较为经典的有插入日志、性能测试、事务处理等。装饰器是解决这类问题的绝佳设计，有了装饰器，我们就可以抽离出大量函数中与函数功能本身无关的雷同代码并继续重用。概括的讲，装饰器的作用就是为已经存在的对象添加额外的功能。

# __new__ 和 __init__ 的区别
1. __new__是一个静态方法,而__init__是一个实例方法.
2. __new__方法会返回一个创建的实例,而__init__什么都不返回.
3. 只有在__new__返回一个cls的实例时后面的__init__才能被调用.
4. 当创建一个新实例时调用__new__,初始化一个实例时用__init__.

__metaclass__是创建类时起作用.所以我们可以分别使用__metaclass__,__new__和__init__来分别在类创建,实例创建和实例初始化的时候做一些小手脚.

# 单例模式
**1. 使用 __new__ 方法**
```python
class Singleton(object):
    def __new__(cls, *args, **kw):
        if not hasattr(cls, '_instance'):
            orig = super(Singleton, cls)
            cls._instance = orig.__new__(cls, *args, **kw)
        return cls._instance
```

# python 中的作用域
Python 中，一个变量的作用域总是由在代码中被赋值的地方所决定的。

当 Python 遇到一个变量的话他会按照这样的顺序进行搜索：

本地作用域（Local）→当前作用域被嵌入的本地作用域（Enclosing locals）→全局/模块作用域（Global）→内置作用域（Built-in）

# GIL 线程全局锁
线程全局锁（Global Interpreter Lock），即 python 为了保证线程安全而采取的独立线程运行的限制，说白了就是一个核只能在同一时间运行一个线程。对于io密集型任务，python的多线程起到作用，但对于cpu密集型任务，python的多线程几乎占不到任何优势，还有可能因为争夺资源而变慢。

# 协程
简单点说协程是进程和线程的升级版,进程和线程都面临着内核态和用户态的切换问题而耗费许多切换时间,而协程就是用户自己控制切换的时机,不再需要陷入系统的内核态.

Python里最常见的yield就是协程的思想!
```python
#生产者消费者
import time
def consumer():
    r=''
    while True:
        n = yield r
        if not n:
            return
        print("[Consumer] Consuming %s..." % n)
        time.sleep(1)
        r = '200 OK'
def produce(c):
    c.next()
    n = 0
    while n < 5:
        n = n + 1
        print("[Producer]Producing %s..." % n)
        r = c.send(n)
        print('[PRODUCER] Consumer return: %s' % r)
    c.close()
if __name__=='__main__':
    c = consumer()
    produce(c)
```

# 闭包
闭包是函数式编程的重要的语法结构。闭包也是一种组织代码的结构，它同样提高了代码的可重复使用性。

当一个内嵌函数引用其外部作用域的变量，我们就会得到一个闭包。总结一下，创建一个闭包必须满足以下几点：
1. 必须有一个内嵌函数
2. 内嵌函数必须引用外部函数中的变量
3. 外部函数的返回值必须是内嵌函数

# python 里的拷贝
引用和copy(),deepcopy()的区别
```python
import copy
a = [1, 2, 3, 4, ['a', 'b']]  #原始对象

b = a  #赋值，传对象的引用
c = copy.copy(a)  #对象拷贝，浅拷贝
d = copy.deepcopy(a)  #对象拷贝，深拷贝

a.append(5)  #修改对象a
a[4].append('c')  #修改对象a中的['a', 'b']数组对象

print 'a = ', a
print 'b = ', b
print 'c = ', c
print 'd = ', d

输出结果：
a =  [1, 2, 3, 4, ['a', 'b', 'c'], 5]
b =  [1, 2, 3, 4, ['a', 'b', 'c'], 5]
c =  [1, 2, 3, 4, ['a', 'b', 'c']]
d =  [1, 2, 3, 4, ['a', 'b']]
```

# Python 垃圾回收机制
Python GC 主要使用引用计数来跟踪和回收垃圾。在引用计数的基础上，通过“标记-清除”解决容器对象可能产生的循环引用问题，通过“分代回收”（generation collection）以空间换时间的方法提高垃圾回收效率。



