Servlet
1. 优点：动态资源，可以编程
2. 缺点：不适合设置 html 响应体，需要大量的 response.getWriter().print("html")

html
1. 优点：不用为输出 html 标签而发愁
2. 缺点：html是静态页面，不能包含动态信息

jsp
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