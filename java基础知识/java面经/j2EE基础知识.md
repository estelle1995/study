# Servlet 总结
在 Java Web程序中，Servlet主要负责接收用户请求 HttpServletRequest, 在 doGet(), doPost() 中做相应的处理，并将回应 HttpServletResponse 反馈给用户。Servlet 可以设置初始化参数，供Servlet内部使用。一个Servlet类只会有一个实例，在它初始化时调用init()方法，销毁时调用destroy()方法**。**Servlet需要在web.xml中配置（MyEclipse中创建Servlet会自动配置），一个Servlet可以设置多个URL访问。Servlet不是线程安全，因此要谨慎使用类变量。

## 阐述 Servlet 与 CGI 的区别？
### CGI 的不足之处
1. 需要为每个请求启动一个操作CGI程序的系统进程，如果请求频繁，这将会带来很大的开销
2. 需要为每个请求加载和运行一个CGI程序
3. 需要重复编写处理网络协议的代码以及编码，这些工作都是非常耗时的

### Servlet 的优点
1. 只需要启动一个操作系统进程以及加载一个JVM， 大大降低了系统的开销
2. 如果多个请求需要做同样处理的时候，这时候只需要加载一个类
3. 所有动态加载的类可以实现对网络协议以及请求解码的共享，降低了工作量
4. Servlet能直接和Web服务器交互，而普通CGI程序不能。Servlet还能再各个程序之间共享数据，使数据库连接池之类的功能很容易实现。

补充：Sun Microsystems公司在1996年发布Servlet技术就是为了和CGI进行竞争，Servlet是一个特殊的Java程序，一个基于Java的Web应用通常包含一个或多个Servlet类。Servlet不能够自行创建并执行，它是在Servlet容器中运行的，容器将用户的请求传递给Servlet程序，并将Servlet的响应回传给用户。通常一个Servlet会关联一个或多个JSP页面。以前CGI经常因为性能开销上的问题被诟病，然而Fast CGI早就已经解决了CGI效率上的问题，所以面试的时候大可不必信口开河的诟病CGI，事实上有很多你熟悉的网站都使用了CGI技术。


## Servlet接口中有哪些方法及Servlet生命周期探秘
```
void init(ServletConfig config) throws ServletException
void service(ServletRequest req, ServletResponse resp) throws ServletException, java.io.IOException
void destroy()
java.lang.String getServletInfo()
ServletConfig getServletConfig()
```
生命周期： Web容器加载Servlet并将其实例化后，Servlet生命周期开始，容器运行其init()方法进行Servlet的初始化；请求到达时调用Servlet的service()方法，service()方法会根据需要调用与请求对应的doGet或doPost等方法；当服务器关闭或项目被卸载时服务器会将Servlet实例销毁，此时会调用Servlet的destroy()方法。init方法和destroy方法只会执行一次，service方法客户端每次请求Servlet都会执行。Servlet中有时会用到一些需要初始化与销毁的资源，因此可以把初始化资源的代码放入init方法中，销毁资源的代码放入destroy方法中，这样就不需要每次处理客户端的请求都要初始化与销毁资源。

## GET 和 POST 的区别
1. GET在浏览器在浏览器回退时是无害得，而POST会再次提交请求。
2. GET产生的URL地址可以被Bookmark，而POST不可以。
3. GET请求会被浏览器主动cache，而POST不会，除非手动设置。
4. GET请求只能进行url编码，而POST支持多种编码方式。
5. GET请求参数会被完整保留在浏览器历史记录里，而POST中的参数不会被保留
6. GET请求在URL中传送的参数是有长度限制的，而POST没有
7. 对参数的数据类型，GET只接受ASCII字符，而POST没有限制
8. GET比POST更不安全，因为参数直接暴露在URL上，所以不能用来传递敏感信息。
9. GET参数通过URL传递，POST放在Requst body中。

GET 和 POST 还有一个重大区别： GET 产生一个 TCP 数据包；POST 产生两个 TCP 数据包。

对于 GET 方式的请求，浏览器会把 http header 和 data 一并发送出去，服务器响应 200 （返回数据）；

而对于POST，浏览器先发送header，服务器响应100 continue，浏览器再发送data，服务器响应200 ok（返回数据）。
1. GET与POST都有自己的语义，不能随便混用。
2. 据研究，在网络环境好的情况下，发一次包的时间和发两次包的时间差别基本可以无视。而在网络环境差的情况下，两次包的TCP在验证数据包完整性上，有非常大的优点。
3. 并不是所有浏览器都会在POST中发送两次包，Firefox就只发送一次。


## 转发（Forward）和重定向（Redirect）的区别
转发是服务器行为，重定向是客户端行为。

转发（Forward）通过 RequestDispatcher 对象的 forward(HttpServlet request, HttpServletResponse response) 方法实现的。RequestDispatcher可以通过HttpServletRequest的getRequestDispatcher() 方法获得。例如下面的代码就是跳转到 login_success.jsp页面。
```java
request.getRequestDispatcher("login_success.jsp").forward(request, response)
```

重定向（Redirect）是利用服务器返回的状态码来实现的。客户端浏览器请求服务器的时候，服务器会返回一个状态码。服务器通过 HttpServletResponse 的 setStatus(int status) 方法设置状态码。如果服务器返回301或者302，则浏览器则会到新的网址重新请求该资源。
1. 从地址栏显示来说
    forward是服务器请求资源,服务器直接访问目标地址的URL,把那个URL的响应内容读取过来,然后把这些内容再发给浏览器.浏览器根本不知道服务器发送的内容从哪里来的,所以它的地址栏还是原来的地址. redirect是服务端根据逻辑,发送一个状态码,告诉浏览器重新去请求那个地址.所以地址栏显示的是新的URL.
2. 从数据共享来说
   forward:转发页面和转发到的页面可以共享request里面的数据. redirect:不能共享数据.
3. 从运用地方来说
   forward:一般用于用户登陆的时候,根据角色转发到相应的模块. redirect:一般用于用户注销登陆时返回主页面和跳转到其它的网站等
4. 从效率来说
   forward:高. redirect:低.

## 自动刷新
自动刷新不仅可以实现一段时间之后自动跳转到另一个页面，还可以实现一段时间之后自动刷新本页面。Servlet中通过HttpServletResponse对象设置Header属性实现自动刷新例如：
```java
Response.setHeader("Refresh", "5;URL=http://localhost:8080/servlet/example.htm");
```   
其中5为时间，单位为秒。URL指定就是要跳转的页面（如果设置自己的路径，就会实现每过5秒自动刷新本页面一次）

## Servlet与线程安全
Servlet不是线程安全的，多线程并发的读写会导致数据不同步的问题。 解决的办法是尽量不要定义name属性，而是要把name变量分别定义在doGet()和doPost()方法内。虽然使用synchronized(name){}语句块可以解决问题，但是会造成线程的等待，不是很科学的办法。 注意：多线程的并发的读写Servlet类属性会导致数据不同步。但是如果只是并发地读取属性而不写入，则不存在数据不同步的问题。因此Servlet里的只读属性最好定义为final类型的。

## JSP和Servlet什么关系？
Servlet是一个特殊的Java程序，它运行于服务器的JVM中，能够依赖服务器的支持向浏览器提供显示内容。JSP本质上是Servlet一种简易形式，JSP会被服务器处理成一个类似于Servlet的Java程序，可以简化页面内容的生成。Servlet和JSP不同点再于，Servlet的应用逻辑是在Java文件中，并且完全从表示层中的HTML分离开来。而JSP的情况是Java和HTML可以组合成一个扩展名为.jsp的文件。JSP侧重于视图，Servlet更侧重于控制逻辑，在MVC架构模式中，JSP适合充当视图（view）而Servlet适合充当控制器（controller）。

## JSP 有哪些内置对象、作用分别是什么？
1. request: 封装客户端请求，其中包含来自GET或POST请求的参数
2. response: 封装服务器对客户端的响应
3. pageContext: 通过该对象可以获取其他对象
4. session：封装用户会话的对象
5. application：封装服务器运行环境的对象
6. out: 输出服务器响应的输出流对象
7. config：Web应用的配置对象
8. page：JSP页面本身（相当于Java程序中的this）
9. exception：封装页面抛出异常的对象

## Request 对象的主要方法有哪些？
1. setAttribute(String name,Object)：设置名字为name的request 的参数值
2. getAttribute(String name)：返回由name指定的属性值
3. getAttributeNames()：返回request 对象所有属性的名字集合，结果是一个枚举的实例
4. getCookies()：返回客户端的所有 Cookie 对象，结果是一个Cookie 数组
5. getCharacterEncoding() ：返回请求中的字符编码方式 = getContentLength() ：返回请求的 Body的长度
6. getHeader(String name) ：获得HTTP协议定义的文件头信息
7. getHeaders(String name) ：返回指定名字的request Header 的所有值，结果是一个枚举的实例
8. getHeaderNames() ：返回所以request Header 的名字，结果是一个枚举的实例
9. getInputStream() ：返回请求的输入流，用于获得请求中的数据
10. getMethod() ：获得客户端向服务器端传送数据的方法
11. getParameter(String name) ：获得客户端传送给服务器端的有 name指定的参数值
12. getParameterNames() ：获得客户端传送给服务器端的所有参数的名字，结果是一个枚举的实例
13. getParameterValues(String name)：获得有name指定的参数的所有值
14. getProtocol()：获取客户端向服务器端传送数据所依据的协议名称
15. getQueryString() ：获得查询字符串
16. getRequestURI() ：获取发出请求字符串的客户端地址
17. getRemoteAddr()：获取客户端的 IP 地址
18. getRemoteHost() ：获取客户端的名字
19. getSession([Boolean create]) ：返回和请求相关 Session
20. getServerName() ：获取服务器的名字
21. getServletPath()：获取客户端所请求的脚本文件的路径
22. getServerPort()：获取服务器的端口号
23. removeAttribute(String name)：删除请求中的一个属性


## request.getAttribute() 和 request.getParameter() 有何区别？
从获取方向来看： getParameter() 是获取 POST/GET 传递的参数值。  getAttribute() 是获取对象容器中的数据值

从用途来看： getParameter() 用于客户端重定向时，即点击了链接或提交按钮时传值用，即用于在用表单或 url 重定向传值时接收数据用。  getAttribute() 用于服务器端重定向时，即在servlet中使用了forward函数，或struts中使用了mapping.findForward。getAttribute 只能收到程序用 setAttribute 传过来的值。

另外，可以用 setAttribute(),getAttribute() 发送接收对象.而 getParameter() 显然只能传字符串。 setAttribute() 是应用服务器把这个对象放在该页面所对应的一块内存中去，当你的页面服务器重定向到另一个页面时，应用服务器会把这块内存拷贝另一个页面所对应的内存中。这样getAttribute()就能取得你所设下的值，当然这种方法可以传对象。session也一样，只是对象在内存中的生命周期不一样而已。getParameter()只是应用服务器在分析你送上来的 request页面的文本时，取得你设在表单或 url 重定向时的值。

总结：
1. getParameter() 返回的是 String，用于读取提交的表单中的值；（获取之后会根据实际需要转换为自己需要的响应类型）
2. getAttribute() 返回的是 Object, 需进行转换,可用setAttribute()设置成任意对象，使用很灵活，可随时用


## include 指令 include的行为的区别
1. include 指令：JSP可以通过include指令来包含其他文件。被包含的文件可以是JSP文件、HTML文件或文本文件。包含的文件就好像是该JSP文件的一部分，会被同时编译执行。 语法格式如下： <%@ include file="文件相对 url 地址" %>
2. include 动作：<jsp:include> 动作元素用老包含静态和动态文件。该动作把指定文件插入正在生成的页面。语法格式如下：<jsp:include page = "相对>

## JSP九大内置对象、七大动作、三大指令
三大指令： include指令、taglib指令、page指令

七大动作
1. jsp:include 在页面被请求的时候引入一个文件。
2. jsp:useBean 寻找或者实例化一个JavaBean
3. jsp:setProperty 设置 JavaBean的属性
4. jsp:getProperty 输出某个 JavaBean的属性
5. jsp:forward 把请求转到一个新的页面
6. jsp:plugin 根据浏览器类型为 Java 插件生成OBJECT或EMBED标记
7. 