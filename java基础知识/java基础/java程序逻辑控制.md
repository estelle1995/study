顺序结构、分支结构、循环结构

# switch 开关语句
switch中可以判断的只能够是数据（int、char、枚举、String），不能使用逻辑判断

从jdk1.7开始switch可以进行字符串数据的判断

# 循环结构
for与while选择参考
1. 在明确确定循环次数的情况下优先选择for循环，
2. 不知道循环次数但是直到循环终止条件时考虑while循环

## 循环控制结构

### continue
continue 还有以下功能
```java
point: for (int x = 0; x < 10; x++) {
    for (int y = 0; y < 3; y++) {
        if (x == 3) {
            continue point;                       //循环结束
        }
        System.out.print(x + "、");
    }
    System.out.println();
}
```
输出
```
0、0、0
1、1、1
2、2、2
4、4、4
5、5、5
6、6、6
7、7、7
8、8、8
9、9、9
```
对此类功能非常不建议去使用
