Asynchronous Javascript And XML （异步 JavaScript 和 XML），是指一种创建交互、快速动态网页应用的网页开发技术，无需重新加载整个网页的情况下，能够更新部分网页的技术。

通过在后台与服务进行少量数据交换，Ajax可以使网页实现异步更新。这意味着可以在不重新加载整个网页的情况下，对网页的某部分进行更新。

# 异步与同步交互
同步：发一个请求, 就要等待服务器的响应结束，才能发第二个请求！中间这段时间就是一个“卡”，刷新的是整个页面

异步：发一个请求后，无需等待服务器的响应，然后就可以发第二个请求。可以使用 js 接收服务器的响应，然后使用 js 来局部刷新


# ajax的优缺点
优点
1. 异步交互：增强了用户体验
2. 性能：因为服务器无需再响应整个页面，只需要响应部分内容，所以服务器的压力减轻了

缺点：
1. ajax 不能用在所有的场景中
2. ajax 无端的增多了对服务器的访问次数，给服务器增加了压力

# HTTP响应状态码
HTTP 状态码和状态信息及其含义
1. 1XX 信息    100 continue        服务器收到了客户端的请求行和头部信息，告诉客户端继续发送数据部分。客户端通常要先发送 Expect:100-continue 头部字段告诉服务器自己还有数据要发送
2. 2XX 成功    200 OK              请求成功
3. 3XX 重定向  301 Moved Permanently  资源被转移了，请求将被重定向
4. 3XX 重定向  302 Found           通知客户端资源能在其他地方找到，但需要使用 Get 方法来获得
5. 3XX 重定向  304 Not Modified    表示被申请的资源没有更新，和之前获得的相同
6. 3XX 重定向  307 Temporary Redirect 通知客户端资源能在其他地方找到。与 302 不同的是，客户端可以使用和原始请求相同的请求方法来访问目标资源
7. 4XX 客户端错误  400 Bad Request  通用客户请求错误
8. 4XX 客户端错误  401 Unauthorized 请求需要认证信息
9. 4XX 客户端错误  403 Forbidden    访问被服务器禁止，通常是由于客户端没有权限访问该资源
10. 4XX 客户端错误 404 NOT Found    资源没有找到
11. 4XX 客户端错误 405 Method Not Allow 方法不被允许
12. 4XX 客户端错误 407 Proxy Authentication Required 客户端需要先获得代理服务器的认证
13. 5XX 服务器错误 500 Internal Server Error 通用服务器错误
14. 5XX 服务器错误 503 Service Unavailable   暂时无法访问服务器

# ajax 发送异步请求
1. 得到 XMLHttpRequest 对象 : 大多数浏览器都支持 var xmlHttp = new XMLHttpRequest();   但是 IE6.0 需要通过 var xmlHttp = new ActiveXObject("Msxml2.XMLHTTP"); 来得到XMLHttpRequest对象。 IE5.5以及更早版本的IE：vae xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
```js
function createXMLHttpRequest() {
    try {
        return new XMLHttpRequest();
    } catch(e) {
        try {
            return new ActiveXObject("Msxml2.XMLHTTP");
        } catch(e) {
            try {
               return new ActiveXObject("Microsoft.XMLHTTP"); 
            } catch(e) {
                alert("请查看使用的浏览器版本！");
                throw e;
            }
        }
    }
}
```
2. 打开与服务器的连接 

xmlHttp.open() 用来打开与服务器的连接，它需要三个参数， 请求方式： 可以是 GET 或 POST。  请求的URL： 指定服务器端的资源。  请求是否为异步： true发送异步请求，否则发送同步请求

3. 发布请求

xmlHttp.send(null)  必须要有参数 null, 如果没有，可能会造成部分浏览器无法发送

上述函数的参数，就是请求体的内容，如果是 GET 请求，必须给出 null.

4. 在 XMLHttpRequest 对象的一个事件上注册监听器  onreadystatechange. XMLHttpRequest对象有5种状态
   1. 0状态： 初始化未完成状态，只是创建了 XMLHttpRequest 对象，还未调用 open() 方法
   2. 1状态： 请求已开始，open() 方法已调用，但还没有调用 send() 方法
   3. 2状态： 请求发送完成状态， send() 方法已调用
   4. 3状态： 开始读取服务器响应
   5. 4状态： 读取服务器响应结束

得到xmlHttp 对象的状态 var state = xmlHttp.readyState;

得到服务器的 响应状态 var status = xmlHttp.status;

得到服务器响应的内容 var content = xmlHttp.responseText;

var content = xmlHttp.responseXML 得到服务器的xml响应内容，它是 Document 对象

```js
xmlHttp.onreadystatechange = function() { //xmlHttp的5种状态都会调用本方法
    if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
        var text = xmlHttp.responseText;
    }

}
```

