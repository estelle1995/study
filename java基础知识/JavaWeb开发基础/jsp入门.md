Servlet
1. 优点：动态资源，可以编程
2. 缺点：不适合设置 html 响应体，需要大量的 response.getWriter().print("html")

html
1. 优点：不用为输出 html 标签而发愁
2. 缺点：html是静态页面，不能包含动态信息

jsp（JavaServer Pages）
1. 优点：在原有 html 的基础上添加 java 脚本，构成 jsp 页面。

### jsp 和 Servlet 分工
jsp：作为请求发起页面，例如表单，超链接，请求结束页面，例如显示数据

servlet： 作为请求中处理数据的环节

### jsp 的组成
1. jsp = html + java脚本 + jsp标签
2. jsp 中无需创建即可使用的对象一共有 9 个，被称之为 9 大内置对象。例如：request 对象、out 对象
3. 3种 java 脚本
   1. <%...%>  java 代码片段（常用），用于定义 0~N 条 Java 语句！ 方法内能写什么，它就可以写什么
   2. <%=...%> java 表达式，用于输出（常用），用于输出一条表达式（或变量）的结果， response.getWriter().print() 里能写什么就能写什么
   3. <%!...%> 声明，用来创建类的成员变量和成员方法（基本不用，但容易被考到），类体中可以写什么，就能写什么


### jsp 原理
jsp 其实是一种特殊的 Servlet
1. 当 jsp 页面第一次被访问时，服务器会把 jsp 编译成 java 文件（这个 java 其实是一个 servlet 类）
2. 然后再把 java 编译成 .class
3. 然后创建该类对象
4. 最后调用它的 service 方法
5. 第二次请求同一 jsp 时， 直接调用 service() 方法

在 tomcat 的 work 目录下可以找到 jsp 对应的 .java 源代码

查看 jsp 对应 java 文件
1. java 脚本
2. html 

jsp 注释： <%--...--%>: 当服务器把 jsp 编译成 java 文件时已经忽略了注释部分！

# Cookie

## Http协议与 Cookie协议(了解)
Cookie 协议是 HTTP 协议制定的！先由服务器保存 Cookie 到浏览器，再下次浏览器请求服务器时把上一次请求得到的 Cookie 再归还给服务器

由服务器创建保存到客户端浏览器的一个键值对！服务器保存 Cookie 的响应头：Set-Cookie: aaa=AAA set-Cookie: bbb = BBB

当浏览器请求服务器时，会把该服务器保存的 Cookie 随请求发送给服务器。浏览器归还 Cookie 的请求头： Cookie:aaa=AAA; bbb=BBB

Http协议规定（保证不给浏览器太大压力）
1. 1个Cookie最大 4KB
2. 1个服务器最多向1个浏览器保存20个cookie
3. 1个浏览器最多可以保存300个cookie

Cookie 的用途
1. 服务器使用 Cookie 来跟踪客户端的状态
2. 保存购物车（购物车中的商品不能使用 request 保存，因为它是一个用户向服务器发送的多个请求消息
3. 显示上次登录名（也是一个用户的多个请求）

JavaWeb中使用 Cookie
1. 使用 response 发送 Set-Cookie 响应头， 使用 request 获取 Cookie 请求头
2. 使用 response.addCookie() 方法向浏览器保存 Cookie, 使用 request.getCookies() 方法获取浏览器归还的 Cookie

Cookie 详解
1. Cookie 不只有 name 和 value 两个属性
2. Cookie 的 maxAge (掌握)：Cookie 的最大生命，即 Cookie 可保存的最大时长，以秒为单位，例如: cookie.setMaxAge(60) 表示这个 Cookie 会被浏览器保存到硬盘上 1 分钟
   1. maxAge > 0 浏览器会把 cookie 保存到客户机硬盘上，有效时长为 maxAge 的值决定
   2. maxAge < 0 cookie 只在浏览器内存中存在，当用户关闭浏览器时，浏览器进程结束，同时 Cookie 也就死亡了。
   3. maxAge = 0 浏览器会马上删除这个 Cookie
3. Cookie 的 path (理解)
   1. Cookie 的 path 并不是设置这个 Cookie 在客户端的保存路径
   2. Cookie 的 path 由服务器创建 Cookie 时设置
   3. 当浏览器访问服务器某个路径时，需要归还哪些 Cookie 给服务器，这由 Cookie 的 path 决定
   4. 浏览器访问服务器的路径，如果包含某个 Cookie 的路径，那么就会归还这个 Cookie
   5. Cookie的path默认值：当前访问路径的父路径，例如在访问/day11_1/jsp/a.jsp则Cookie的默认path为/day11_1/jsp
4. Cookie 的 domain （了解）
   1. domain 用来指定 Cookie 的域名！当多个二级域中共享 Cookie 时才有用
   2. 例如： www.baidu.con、 zhidao.baidu.com、news.baidu.com、tieba.baidu.com 之间共享 Cookie 时可以使用 domain
   3. 设置 domain 为：cookie.setDomain(".baidu.com");
   4. 设置 path 为： cookie.setPath("/");


# HttpSession 
## HttpSession 概述
HttpSession 是由 JavaWeb 提供的，用来会话跟踪的类。session 是服务器端对象，保存在服务器端！
 
HttpSession 是 Servlet 三大域对象之一（request(ServletRequest)、session(HttpSession)、application（servletContext）），所以它也有 setAttribute()、getAttribute()、removeAttribute() 方法。

HttpSession 底层依赖 Cookie， 或是 URL 重写

## HttpSession 的作用
会话范围：会话范围是某个用户从首次访问服务器开始，到该用户关闭浏览器结束！

会话：一个用户对服务器的多次连贯性请求！所谓连贯性请求，就是该用户多次请求中间没有关闭浏览器

服务器会为每个客户端创建一个 Session 对象，session就好比客户在服务器端的账户，他们被服务器保存到一个 Map 中，这个 Map 被称之为 session 缓存
1. HttpSession session = request.getSession(); Servlet 中得到 session 对象
2. JSP 中得到 session 对象： session 是 jsp 内置对象，不用创建，直接使用



## HttpSession 原理
request.getSession() 方法步骤如下
1. 获取 Cookie 中的 sessionId: 如果 sessionId 不存在，创建 session, 把 session 保存起来，把新创建的 sessionId 保存到 Cookie 中。如果 sessionId 存在，通过 sessionId 查找 session 对象，如果没有查找到，创建 session， 把 session 保存起来，把新创建的 sessionId 保存到 Cookie 中，如果查找到，不用创建，返回 session 对象
2. 如果创建了新的 session, 浏览器会得到一个包含了 sessionId 的 Cookie, 这个 Cookie 的生命为-1， 
3. 下次请求时，再次执行 request.getSession() 方法时，因为可以通过 Cookie 中的 sessionId 找到 session 对象，所以与上一次请求使用的是同一 session 对象。

服务器不会马上给你创建 session, 在第一次获取 session 时，才会创建

request.getSession(false), request.getSession(true), request.getSession() 后两个方法相同，第一个方法，如果 session 缓存中（如果 cookie 不存在），不存在 session，那么返回 null, 而不会创建 session 对象。

## HttpSession 其他方法
1. String getId() 获取 SessionId
2. int getMaxInactiveInterval()  获取 session 可以的最大不活动时间（秒），默认为 30 分钟
3. void invalidate() 让 session 失效！调用这个方法会让 session


## web.xml 中配置session的最大不活动时间
```xml
<session-config>
   <session-timeout>30</session-timeout>
</session-config>
```


## URL 重写
1. session 依赖 Cookie, 目的是让客户端发出请求时归还 sessionId,这样才能找到对应的session
2. 如果客户端禁用了 Cookie, 那么就无法得到 sessionId, 那么session也就无用了
3. 也可以使用 URL重写来替代 Cookie
4. 让网站的所有超链接、表单中都添加一个特殊的请求参数，即 sessionId
5. 这样服务器可以通过获取请求参数得到 sessionId,从而找到session对象
6. response.encodeURL(String url) 该方法会对 url 进行智能的重写。当请求中没有归还 sessionid 这个 cookie, 那么该方法会重写 url，否则不重写！

# JSP 三大指令 page, include, taglib
JSP 指令的格式
```jsp
<%@指令名 attr="" attr2=""%>
```

## page指令
page指令是最常用的指令，也是属性最多的指令， page没有必须属性，都是可选属性
```jsp
<%@page language="java"%>
<%@page import="java.util.*"%>
<%@page pageEncoding="utf-8"%>
```
pageEncoding 它指定当前页面的编码,在服务器要把 jsp 编译成 .java 时需要使用

pageContentType 它表示添加一个响应头：Content-Type!等同与 response.setContentType("text/html;charset=utf-8")

如果两个属性只设置一个，那么没设置的默认为设置的那一个。如果两个属性都没有设置，则默认为 ISO

import 导包，可以出现多次

errorPage 当前页面如果抛出异常，那么要转发到哪一个页面

isErrorPage 它指定当前页面是否为处理错误的页面！当该属性为true时，这个页面会设置状态码为500！而且这个页面可以使用9大内置对象的exception！

autoFlush 指定jsp的输出缓冲区满时，是否自动刷新！默认为true， 如果false，那么在缓冲区满时抛出异常

buffer 指定缓冲区大小，默认为 8kb，通常不需要修改！

isELIgnored 是否忽略 el 表达式，默认值为 false, 不忽略，即支持！

language 指定当前 jsp 编译后的语言类型，默认值为 java

info 信息

isThreadSafe 当前的 jsp 是否支持并发访问

session 当前页面是否支持 session 如果为false， 那么当前页面就没有 session 这个内置对象

extends 让 jsp 生成的 servler去继承该属性指定的类

## JSP 九大内置对象
1. out   等同于 response.getWriter()
2. config 等同于 ServletConfig
3. page  等同于 this
4. pageContext 页面上下文对象
5. exception
6. request       HttpServletRequest 对象 
7. response      HttpServletResponse对象
8. application   ServletContext 类对象
9. session       HttpSession 对象

servlet 三大域 HttpSession, ServlerContext, HttpServletRequest

jsp 四大域 application, session, request, pageContext

### pageContext
这个域是当前jsp面和当前jsp页面中使用的标签之间共享数据。 它能代理其他域, 可以获取其他内置对象，全域查找, 查找顺序：pageContext, request, session, application


# include 静态包含
与 RequestDispatcher 的 include 方法的功能相似！
```
<%@include%>
```
它是在 jsp 编译成 java文件时完成的！它们共同生成一个 java （就是一个 servlet）文件，然后再生成一个 class

RequestDispatcher 的 include() 是一个方法，包含和被包含的是两个 servlet, 即两个 .class! 他们只是把响应的内容再运行时合并了！

作用：把页面分解了， 使用包含的方式组合在一起，这样一个页面中不变的部分，就是一个独立jsp，而我们只需要处理变化的页面


# taglib 导入标签库
两个属性
1. prefix 指定标签库在本页面中的前缀！由我们自己来起名
2. uri 指定标签库的位置
```jsp
<%@taglib prefix="pre" uri="/struct-tags"%>
```


# JSP 动作标签
这些 jsp 的动作标签， 与 html 提供的标签有本质的区别
1. 动作标签是由 tomcat(服务器) 来解释执行！ 它与 java 代码一样，都是在服务器端执行
2. html 标签由浏览器来执行
```
<jsp:forward> 转发！ 它与RequestDispatcher的forward方法是一样的，一个是在Servlete中使用，一个是在jsp中使用
<jsp:include> 包含！ 它与RequestDispatcher的include方法是一样的，一个是在Servlete中使用，一个是在jsp中使用
<jsp:param> 它用来作为 forward 和 include 的子标签！用来给转发或包含的页面传递参数
```



# JavaBean
1. 必须要有一个默认构造器
2. 提供 get/set 方法，如果只有 get 方法，那么这个属性是只读属性！
3. 属性： 有 get/set 方法的成员，还可以没有成员，只有 get/set 方法，属性名称由 get/set 方法来决定！而不是成员名称！
4. 方法名称满足一定的规范，那么他就是属性！ boolean 类型的属性，它的方法可以是 is 开头也可以是 get 开头


## 内省
内省的目标是得到 JavaBean 属性的读、写方法的反射对象，通过反射对 JavaBean 属性进行操作的一组 API。例如 User 类有名为 username 的 JavaBean 属性，通过两个 Method 对象（一个是 getUsername()，一个是 setUsername()）来操作 User 对象

jsp 中与 JavaBean 相关的标签：
```jsp
<jsp:useBean>   创建或查询 bean
eg：<jsp:useBean id="user1" class="cn.itcast.domain.User" scope="session"/>
<jsp:setProperty>
eg：<jsp:setProperty property="username" name="user1" value="admin"/>
<jsp:getProperty>
eg: <jsp:getProperty property="username" name="user1"/>
```

# EL 表达式
EL表达式是jsp内置的表达式。jsp2.0 开始，不让再使用 java 脚本，而是使用 el 表达式和动态标签来替代 java 脚本

EL 替代的是 <%=...%>, 也就是说， EL 只能做输出！

EL 表达式来读取四大域
1. ${xxx}, 全域查找名为 xxx 的属性，如果不存在，输出空字符串，而不是 null
2. ${pageScope.xxx}  ${requestScope.xxx}  ${sessionScope.xxx}  ${applicationScope.xxx} 指定域获取属性

EL 可以输出的东西都在 11 个内置对象中！ pageScope, requestScope, sessionScope, applicationScope, param, paramValues, headers, headerValues, initParam, cookie, pageContext, 除了 pageContext 其他的都是map

## EL 内置对象

### 请求参数相关内置对象
param：Map<String, String> 类型， param 对象可以用来获取参数，与request.getParameter() 方法相同

paramValues: Map<String, String[]> 类型，当一个参数名，对应多个参数值时可以使用它


### 请求头相关内置对象
headers， headerValues 与上类似
```jsp
${header['User-Agent']}
```

### 应用初始化参数相关内置对象
initParam: 获取web.xml中的 context-param 中的参数

### Cookie 相关内置对象
cookie ：Map<String, Cookie> 类型，其中key是 cookie 的 name，value 是 cookie 的对象 ${cookie.username.value}

### pageContext 对象
pageContext 是 PageContext 类型。eg ${pageContext.request.contextPath} 输出 /webProject



## EL 函数库 （由 JSTL 提供的）
导入标签库
```
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
```

### EL 函数库介绍
1. String toUpperCase(String input)
2. String toLowerCase(String input)
3. int indexOf(String input, String substring)
4. boolean contains(String input, String substring)
5. boolean containsIgnoreCase(String input, String substring)
6. boolean startsWith(String input, String substring)
7. boolean endsWith(String input, String substring)
8. String substring(string input, int beginIndex, int endIndex)
9. String substringAfter(String input, String substring)
10. String substringBefore(String input, String substring)
11. String escapeXml(String input) 将字符串的">", "<"等转义了
12. String trim(String input)
13. String replace(String input, String substringBefore, String substringAfter)
14. String[] split(String input, String delimiters)
15. int length(Object obj)
16. String join(String array[], String separator)
```jsp
<%
   String[] strs = {"a", "b", "c"};
   pageContext.setAttribute("arr", strs);
%>
${fn:length(arr)}
```


## EL自定义函数库
写一个Java类，类中可以定义0~N个函数，必须是静态方法且有返回值。在xml文件中声明
```java
package cn.itcast.el.functions;

public class ItcastFunctions {
   public static String test() {
      return "EL自定义测试函数"
   }
}
```
itcast.tld
```xml
<?xml version="1.0" encoding="UTF-8"?>
<taglib xmlns="http://java.sun.com/xml/ns/j2ee"
        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xsi:schemaLocation="http://java.sun.com/xml/ns/j2ee http://java.sun.com/xml/ns/j2ee/web-jsptaglibrary_2_0.xsd"
        version="2.0">

    <description>itcast</description>
    <display-name>itcast-function</display-name>
    <tlib-version>1.0</tlib-version>
    <short-name>it</short-name>
    <uri>http://www.itcast.cn./el/functions</uri>
    
    <function>
    	<description>description</description>
    	<name>test</name>
    	<function-class>cn.itcast.el.functions.ItcastFunctions</function-class>
    	<function-signature>String fun()</function-signature>
    	<example>
    		test="${fn:test()}"
    	</example>
    </function>
</taglib>
```
a.jsp
```jsp
<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<%@ taglib prefix="it" uri="/WEB-INF/tlds/itcast.tld" %>
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Insert title here</title>
</head>
<body>
	<h1>${it:test() }</h1>
</body>
</html>
```


# JSTL 标签库
JSTL 是 apache 对 EL 表达式的扩展（也就是说JSTL依赖EL），JSTL是标签语言！JSTL 标签使用以来非常方便，它与JSP动作标签一定，只不过它不是JSP内置的标签，需要我们自己导报，以及指定标签库而已

使用 jstl 需要导入 jstl-1.2.jar包

## JSTL 四大标签库
1. core 核心标签库
2. fmt 格式化标签库
3. sql 数据库标签库 过时
4. xml xml标签库 过时


## 使用 taglib 指令导入标签库
```jsp
<%@taglib prefix="c" uri="http://java.sun.com/jstl/core%>
```

### core 标签库常用标签