JDBC(Java DataBase Connectivity) 就是 Java 数据库连接，

```java
package cn.mldn.demo;

import java.sql.Connection;
import java.sql.DriverManager;

public class Main {
    // MySQL 8.0 以下版本 - JDBC 驱动名及数据库 URL
    //static final String JDBC_DRIVER = "com.mysql.jdbc.Driver";  
    //static final String DB_URL = "jdbc:mysql://localhost:3306/RUNOOB";
 
    // MySQL 8.0 以上版本 - JDBC 驱动名及数据库 URL
    //static final String JDBC_DRIVER = "com.mysql.cj.jdbc.Driver";  
    //static final String DB_URL = "jdbc:mysql://localhost:3306/RUNOOB?useSSL=false&serverTimezone=UTC";

	private static final String DATABASE_DRIVER = "com.mysql.cj.jdbc.Driver";
	private static final String DATABASE_URL = "jdbc:mysql://localhost:3306/test_db?useSSL=false&serverTimezone=UTC";
	private static final String USER = "root";
	private static final String PASSWORD = "123456";
	
    public static void main(String [] args) throws Exception{
    	Class.forName(DATABASE_DRIVER);
    	Connection conn = DriverManager.getConnection(DATABASE_URL, USER, PASSWORD);
    	System.out.println(conn);
    }
}
```

# ResultSet 结果集
获取结果集元数据
1. 得到元数据: rs.getMetaData(), 返回值为 ResultSetMetaData

## ResultSetMetaData
1. 获取结果集列数：int getColumnCount()
2. 获取指定列的列名：String getColumnName(int colIndex)


默认的结果集只能使用下一行，其他方法存在，但不能使用！默认的结果集不可滚动

1. void beforeFirst(): 把光标放到第一行的前面，这也是光标的默认位置
2. void afterLast(): 把光标放到最后一行的后面
3. boolean first(): 把光标放到第一行的位置上，返回值表示调控光标是否成功
4. boolean last()
5. boolean isBeforeFirst()
6. boolean isAfterLast()
7. boolean isFirst()
8. boolean isLast()
9. boolean previous()
10. boolean next()
11. boolean relative(int row): 相对位移，当row为正数时表示向下移动 row 行，为负数时，表示向上移动 row 行
12. boolean absolute(int row): 绝对位移，把光标移动到指定的行上
13. int getRow(): 返回当前光标所有行。

上述方法分为两类，一类用来判断游标的位置，另一类用来移动游标。如果结果集是不可滚动的，那么只能使用 next() 方法来移动游标，而 beforeFirst(), afterLast(), first(), last(), previous(), relative()，absolute()方法都不能使用

## 结果集特性
1. 是否可滚动
2. 是否敏感
3. 是否可更新

结果集是否支持滚动，要从 Connection 类的 createStatement() 方法说起。也就是说创建的 Statement 决定了使用 Statement 创建的 ResultSet 是否支持滚动。

Statement createStatement(): 生成的结果集：不滚动，不敏感，不可更新

Statement createStatement(int resultSetType, int resultSetConcurrency), resultSetType 的可选值如下：
1. ResultSet.TYPE_FORWARD_ONLY: 不滚动结果集
2. ResultSet.TYPE_SCROLL_INSENSITIVE: 滚动结果集，但结果集数据不会再跟随数据库而变化
3. ResultSet.TYPE_SCROLL_SENSITIVE：滚动结果集，但结果集数据不会再跟随数据库而变化， 没有数据库驱动会支持它

resultSetConcurrency 的可选值
1. CONCUR_READ_ONLY: 结果集是只读的，不能通过修改结果集而反向影响数据库
2. CONCUR_UPDATABLE: 结果集是可更新的，对结果集的更新可以反向影响数据库。一般不使用



# PreparedStatement
Statement 接口的子接口
1. 防止 SQL 攻击
2. 提高代码的可读性和可维护性
3. 提高效率

```java
package cn.mldn.demo;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;

public class Main {
	private static final String DATABASE_DRIVER = "com.mysql.cj.jdbc.Driver";
	private static final String DATABASE_URL = "jdbc:mysql://localhost:3306/test_db?useSSL=false&serverTimezone=UTC";
	private static final String USER = "root";
	private static final String PASSWORD = "123456";
	
    public static void main(String [] args) throws Exception{
    	Class.forName(DATABASE_DRIVER);
    	Connection conn = DriverManager.getConnection(DATABASE_URL, USER, PASSWORD);
    	System.out.println(conn);
    	int sid = 1;
    	String sname = "wangwu";
    	String sql = "select * from student where sid=? and sname=?";
    	PreparedStatement state = conn.prepareStatement(sql);
    	state.setInt(1, sid);
    	state.setString(2, sname);
    	ResultSet rs = state.executeQuery();
    	while(rs.next()) {
    		System.out.println(rs.getInt(1) + ", " + rs.getString(2) + ", " + rs.getInt(3) + ", " + rs.getString(4));
    	}
    }
}
```

## 预处理的原理
1. 检验sql语句的语法
2. 编译：一个与函数相似的东西
3. 执行：调用函数

PreparedStatement: 使用的前提，连接的数据库必须支持预处理，几乎没有不支持的。每个PreparedStatement对象都与一个sql模板绑定在一起，先把 sql 模板给数据库，数据库先进行校验，再进行编译。执行时只是把参数传递过去而已。若二次执行时，就不用再次校验语法，也不用再次编译直接执行。

使用 Statement 执行预编译
```java
    public static void main(String [] args) throws Exception{
    	Class.forName(DATABASE_DRIVER);
    	Connection conn = DriverManager.getConnection(DATABASE_URL, USER, PASSWORD);
    	System.out.println(conn);
    	int sid = 1;
    	Statement state = conn.createStatement();
    	state.executeUpdate("prepare myfun from 'select * from student where sid=?'");
    	state.executeUpdate("set @sid=1");
    	ResultSet rs = state.executeQuery("execute myfun using @sid");
    	while(rs.next()) {
    		System.out.println(rs.getInt(1) + ", " + rs.getString(2) + ", " + rs.getInt(3) + ", " + rs.getString(4));
    	}
    }
```

默认使用 PreParedStatement 是不能执行预编译的，这需要在 url 中给出 useServerPrepStmts=true

当使用不同的 PreparedStatement 对象来执行相同的 SQL 语句时，还是会出现编译两次的现象，这是因为驱动没有缓存编译后的函数 key, 导致二次编译。如果希望缓存编译后函数的 key, 那么就要设置 cachePrepStmts 参数为 true。

MySQL 的批处理也需要通过参数来打开： rewriteBatchedStatements=true

```java
public class JDBCUtil {
	public static Connection getConnection() throws IOException, ClassNotFoundException, SQLException {
		InputStream in = JDBCUtil.class.getClassLoader().getResourceAsStream("dbConfig.properties");
		Properties props = new Properties();
		props.load(in);
		
		Class.forName(props.getProperty("driverClassName"));
		String url = props.getProperty("url");
		String username = props.getProperty("username");
		String password = props.getProperty("password");
		return DriverManager.getConnection(url, username, password);
	}
}
```
dbConfig.properties 内容如下
```
driverClassName=com.mysql.cj.jdbc.Driver
url=jdbc:mysql://localhost:3306/test_db?useSSL=false&serverTimezone=UTC
username=root
password=123456
```


# 面向接口编程

## DAO 模式
DAO (Data Access Object) 模式就是写一个类，把访问数据库得代码封装起来。 DAO 在数据库与业务逻辑（service）之间。
1. 实体域，即操作的对象，例如我们操作的表是 User 表，那么就需要先写一个 User 类；
2. DAO 模式需要先提供一个 DAO 接口
3. 然后再提供一个 DAO 接口的实现类
4. 再编写一个 DAO 工厂， Service 通过工厂来获取 DAO 实现

