SQL 语言分类：
1. DDL （Data Definition Language）：数据定义语言，用来定义数据库对象：库、表、列等；
2. DML （Data Manipulation Language）：数据操作语言
3. DCL （Data Control Languange）：数据控制语言，用来定义访问权限和安全级别
4. DQL （Data Query Language）

# DDL
数据库
1. 查询所有数据库 SHOW DATABASES
2. 切换数据库     USE 数据库名
3. 创建数据库     CREATE DATABASE [IF NOT EXISTS] 数据库名 [CHARSET=utf8]
4. 删除数据库     DROP DATABASE [IF EXIST] 数据库名
5. 修改数据库编码 ALTER DATABASE 数据库名 CHARACTER SET utf8

数据类型
1. int 整型
2. double 浮点型   double(5, 2)
3. decimal 浮点型
4. char 固定长度字符串型
5. varchar 可变长度字符串型
6. text(clob) 字符串类型
7. blob 字节类型
8. date 日期类型      格式为：yyyy-MM-dd
9. time 时间类型      格式为：hh:mm:ss
10. timestamp 时间戳类型

表
1. 创建表：
```sql
CREATE TABLE [IF NOT EXISTS] 表明 (
    列名 列类型，
    ...
    列名 列类型
)
```
2. 查看当前数据库中的所有表名称： SHOW TABLES
3. 查看指定表的创建语句结构：     SHOW CREATE TABLE 表名
4. 查看表结构： DESC TABLE 表名
5. 删除表： DROP TABLE 表名
6. 修改表： 添加列
```sql
ALTER TABLE 表名 ADD(
    列名 列类型，
    列名 列类型，
    ...
)
```
7. 修改表： 修改列类型   ALTER TABLE 表名 MODIFY 列名 列类型
8. 修改表： 修改列名称   ALTER TABLE 表名 CHANGE 原列名 新列名 列类型
9. 修改表： 修改删除列   ALTER TABLE 表名 DROP 列名
10. 修改表名称：        ALTER TABLE 原表名 RENAME TO 新表名

# DML
插入数据
1. INSERT INTO 表名(列名1, 列名2, ...) VALUES(列值1, 列值, ...)
2. INSERT INTO 表名VALUES(列值1, 列值, ...)

修改数据
1. UPDATE 表名 SET 列名1=列值1, 列名2=列值2, ... [WHERE 条件]
2. 条件  运算符有：=、!=、<>、>、<、>=、<=、BETWEEN ... AND ...、 IN(....)、IS NULL、NOT、OR、AND

删除数据
1. DELETE FROM 表名 [WHERE 条件]
2. TRUNCATE TABLE 表名： TRUNCATE 是 DDL 语句， 他是先删除 drop 该表，再create该表。而且无法回滚

# DCL
创建用户
1. CREATE USER 用户名@IP地址 IDENTIFIED BY '密码';   用户只能在指定的 IP 地址登录
2. CREATE USER 用户名@'%' IDENTIFIED BY '密码';     用户可以在任意IP地址上登录

给用户授权
1. GRANT 权限1, 权限2,...  ON 数据库.* TO 用户名@IP地址
   1. 例如： GRANT CREATE, ALTER, DROP, INSERT, UPDATE, DELETE, SELECT, ON mydb1.* TO user1@localhost;
2. GRANT ALL ON 数据库.* TO 用户名@IP地址

撤销权限
1. REVOKE 权限1, 权限2, ... ON 数据库.* TO 用户名@IP地址

查看权限：SHOW GRANTS FOR 用户名@IP地址

删除用户：DROP USER 用户名@IP地址

# DQL 数据查询语言
基本查询
1. SELECT *|列1[, 列2, ... 列n] FROM 表名;
2. SELECT DISTINCT *|列1[, 列2, ... 列n] FROM 表名;
3. SELECT sal + comm FROM emp;
4. SELECT CONCAT('$', sal) FROM emp;
5. SELECT IFNULL(comm, 0) + 1000 FROM emp;
6. SELECT IFNULL(comm, 0) + 1000 AS 奖金 FROM emp;   as 可以省略


条件查询： SELECT * FROM 表名 WHERE 条件
1. % 匹配 0 ~ N 个任意字符。SELECT * FROM emp WHERE name LIKE '%'; 能匹配任意字符串但是姓名为NULL则查询不出来
2. _ 匹配 1 个字符

排序
1. 升序 SELECT * FROM emp ORDER BY sal [ASC];
2. 降序 SELECT * FROM emp ORDER BY sal DESC;
3. 多列排序 SELECT * FROM emp ORDER BY sal, comm DESC;

聚合函数 （用来做某列的纵向运算）
1. COUNT()
   1. SELECT COUNT(*) FROM emp; 计算emp表中所有列都不为 NULL 的记录的行数
   2. SELECT COUNT(comm) FROM emp; 计算emp表中comm列不为 NULL 的记录的行数
2. MAX()
3. MIN()
4. SUM()
5. AVG()

分组查询
1. SELECT deptno, COUNT(*) FROM emp GROUP BY deptno;
2. SELECT job, MAX(sal) FROM emp GROUP BY job;
3. SELECT deptno, COUNT(\*) FROM emp GROUP BY demptno HAVING COUNT(*) > 3 

limit 子句（只有 MySQL中有） Limit 用来限定查询结果的起使行，以及总行数，例如从第5行查询，一共查询3行记录
1. SELECT * FROM emp LIMIT 4, 3



# 编码
查看 MySQL 数据库编码   SHOW VARIABLES LIKE 'char%';
```sql
--------------------------+---------------------------------------------------------+
| Variable_name            | Value                                                   |
+--------------------------+---------------------------------------------------------+
| character_set_client     | gbk                                                     |
| character_set_connection | gbk                                                     |
| character_set_database   | utf8                                                    |
| character_set_filesystem | binary                                                  |
| character_set_results    | gbk                                                     |
| character_set_server     | utf8mb4                                                 |
| character_set_system     | utf8                                                    |
| character_sets_dir       | C:\Program Files\MySQL\MySQL Server 8.0\share\charsets\
```
1. character_set_client   无论客户端发送的是什么编码数据，mysql 都当成是 utf8 的数据
    把character_set_client 修改为 utf8 ：  set character_set_client=utf8;    只在当前窗口内有效，也就是说，关闭窗口，再打开，又回到 gbk 了。
2. character_set_results  把数据用什么编码发送给客户端 

再配置文件 my.ini 中设置如下
```
[mysql]
# 只可以修改三个变量：character_set_client， character_set_results ， character_set_connection
default-character-set=gbk
```


# 备份与恢复
数据库导出 SQL 脚本:  直接在命令行输入： mysqldump -u用户名 -p密码 数据库名>生成的脚本文件路径
```
mysqldump -uroot -p123456 test_db>c:\test_db.sql
```

执行上述导出的脚本
1. 在命令行直接执行： mysql -uroot -p123456 test_db<c:\test_db.sql
2. 登录mysql， 在 mysql 命令行下输入： source SQL脚本路径


# 约束

## 主键约束
非空， 唯一， 被引用（外键）。
1. 当表的某一列被指定为主键后，该列就不能为空，不能有重复值出现。
2. 当创建表时指定主键的两种方式
```sql
CREATE TABLE stu(
    sid        CHAR(6) PRIMARY KEY,
    sname  VARCHAR(20),
    age            INT,
    gender VARCHAR(10)
);
CREATE TABLE stu(
    sid        CHAR(6),
    sname  VARCHAR(20),
    age            INT,
    gender VARCHAR(10),
    PRIMARY KEY(sid)
);
```
3. 修改表时指定主键列：ALTER TABLE stu ADD PRIMARY KEY(sid);
4. 删除主键： ALTER TABLE stu DROP PRIMARY KEY;
5. 主键自增长：
```sql
CREATE TABLE stu(
    sid INT PRIMARY KEY AUTO_INCREMENT,
    sname  VARCHAR(20),
    age            INT,
    gender VARCHAR(10)
)
```
6. 修改表时设置主键自增长： ALTER TABLE stu CHANGE sid sid INT AUTO_INCREMENT;
7. 修改表时删除主键自增长： ALTER TABLE stu CHANGE sid sid INT;

## 非空约束
```sql
CREATE TABLE stu(
    sid INT PRIMARY KEY AUTO_INCREMENT,
    sname  VARCHAR(20)  NOT NULL,
    age            INT,
    gender VARCHAR(10)
)
```

## 唯一约束
```sql
CREATE TABLE stu(
    sid INT PRIMARY KEY AUTO_INCREMENT,
    sname  VARCHAR(20)  NOT NULL UNIQUE,
    age            INT,
    gender VARCHAR(10)
)
```

# 概念模型，关系模型， 对象模型
概念模型：当我们要完成一个软件系统时，需要把系统中的实体抽象出来，形成概念模型


# 外键约束
一对多
```sql
CREATE TABLE dept(
    deptno INT PRIMARY KEY AUTO_INCREMENT,
    dname VARCHAR(50)
);
CREATE TABLE emp(
    empno INT PRIMARY KEY AUTO_INCREMENT,
    ename VARCHAR(50),
    dno INT,
    CONSTRAINT fk_emp_dept FOREIGN KEY(dno) REFERENCES dept(deptno)
);
```

一对一
```sql
CREATE TABLE husband(
    hid INT PRIMARY KEY AUTO_INCREMENT,
    hname VARCHAR(50)
);
CREATE TABLE wife(
    wid INT PRIMARY KEY AUTO_INCREMENT,
    wname VARCHAR(50),
    hid INT NOT NULL UNIQUE,
    CONSTRAINT fk_wife_husband FOREIGN KEY(hid) REFERENCES husband(hid);
);
```

多对多
```SQL
CREATE TABLE student(
    sid INT PRIMARY KEY,
    ...
);
CREATE TABLE teacher(
    tid INT PRIMARY KEY,
    ...
);
CREATE TABLE stu_tea(
    sid INT,
    tid INT,
    CONSTRAINT fk_student FOREIGN KEY(sid) REFERENCES student(sid),
    CONSTRAINT fk_teacher FOREIGN KEY(tid) REFERENCES student(tid)
);
```

# 多表查询
1. 合并结果集
2. 连接查询
3. 子查询
   
## 合并结果集
要求被合并的表中，列的类型和列数相同
1. UNION 去除重复行
2. UNION ALL 不去除重复行
```
select * from ab;
union
select * fron cd;
```
## 连接查询
内连接， 外连接（左外连接，右外连接，全外连接）， 自然连接

内连接：（当不满足WHERE条件的两个表的元组项则不会列出）
1. 只能MySQL中使用：SELECT * FROM 表1 别名1, 表2 别名2 WHERE 别名1.xx=别名2.xx 
2. 标准：SELECT * FROM 表1 别名1 INNER JOIN 表2 别名2 ON 别名1.xx=别名2.xx
3. 自然连接：SELECT * FROM 表1 别名1 NATURAL JOIN 表2 别名2   会找名字相同的列自动匹配。

外连接
1. 左外：SELECT * FROM 表1 别名1 LEFT OUTER JOIN 表2 别名2 ON 别名1.xx=别名2.XX
2. 右外：SELECT * FROM 表1 别名1 RIGHT OUTER JOIN 表2 别名2 ON 别名1.xx=别名2.XX
3. 左外自然连接：SELECT * FROM 表1 别名1 NATURAL LEFT OUTER JOIN 表2 别名2 ON 别名1.xx=别名2.XX
4. 右外自然连接：SELECT * FROM 表1 别名1 NATURAL RIGHT OUTER JOIN 表2 别名2 ON 别名1.xx=别名2.XX
5. 全连接：可以使用 UNION 来完成

## 子查询



