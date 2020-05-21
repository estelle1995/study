select, poll, epoll 都是 IO 多路复用的机制。I/O多路复用就是通过一种机制，一个进程可以监视多个描述符，一旦某个描述符就绪（一般是读就绪或者写就绪），能够通知程序进行相应的读写操作。但select，poll，epoll本质上都是同步I/O，因为他们都需要在读写事件就绪后自己负责进行读写，也就是说这个读写过程是阻塞的，而异步I/O则无需自己负责进行读写，异步I/O的实现会负责把数据从内核拷贝到用户空间。

# select
监视多个文件句柄的状态变化，程序会阻塞在select处等待，直到有文件描述符就绪或超时。
```c
int select (int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```
select 函数监视的文件描述符分3类，writefds(写状态), readfds(读状态), exceptfds(异常状态)。调用后select函数会阻塞，直到有描述符就绪（有数据 可读、可写、或者有except），或者超时（timeout指定等待时间，如果立即返回设为null即可）。当select函数返回后，可以通过遍历fdset，来找到就绪的描述符。

**缺陷**
1. 单个进程能够监视文件描述符的数量存在最大限制，通常是 1024，当然可以更改数量，但由于select采用轮询的方式扫描文件描述符，文件描述符数量越多，性能越差；(在linux内核头文件中，有这样的定义：#define __FD_SETSIZE 1024)；
2. 内核/用户空间内存拷贝问题，select需要复制大量的句柄数据结构，产生巨大的开销
3. select 返回的是含有整个句柄的数组，应用程序需要遍历整个数组才能发现哪些句柄发生了变化。

以select模型为例，假设我们的服务器需要支持100万的并发连接，则在__FD_SETSIZE 为1024的情况下，则我们至少需要开辟1k个进程才能实现100万的并发连接。除了进程间上下文切换的时间消耗外，从内核/用户空间大量的无脑内存拷贝、数组轮询等，是系统难以承受的。因此，基于select模型的服务器程序，要达到10万级别的并发访问，是一个很难完成的任务。

# poll
与select轮询所有待监听的描述符机制类似，但poll使用pollfd结构表示要监听的描述符，poll使用链表保存文件描述符，因此没有了监视文件数量的限制，但其他select的缺点依然存在
```
int poll (struct pollfd *fds, unsigned int nfds, int timeout);
```
pollfd结构
```
struct pollfd {
    int fd; /* file descriptor */
    short events; /* requested events to watch */
    short revents; /* returned events witnessed */
};
```
pollfd结构包括了events(要监听的事件)和revents(实际发生的事件)。而且也需要在函数返回后遍历pollfd来获取就绪的描述符。

# epoll
epoll 的实现机制与 select/poll 机制完全不同，相对于 select 和 poll 来说，epoll更加灵活，没有描述符限制。epoll 使用一个文件描述符管理多个描述符，将用户关系的文件描述符的事件存放到内核的一个事件表中，这样在用户空间和内核空间的 copy 只需一次。

epoll 在 Linux 内核中申请了一个简易的文件系统， 通过三个函数 epoll_create, epoll_ctl, epoll_wait 实现调度。
```c
int epoll_create(int size)；
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)；
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
```
调用过程如下：
1. 调用 epoll_create() 创建一个 epoll 对象（在epoll文件系统中为这个句柄对象分配资源。
2. 调用 epoll_ctl() 向 epoll 对象中添加连接的套接字
3. 调用 epoll_wait() 收集发生的事件的连接

当某一进程调用 epoll_create 方法时，Linux 内核会创建一个 eventpoll 结构体，这个结构体中有两个成员与 epoll 的使用方式密切相关，eventpoll 结构体如下所示。
```c
struct eventpoll {
    ...
    //红黑树的根节点，这颗树种存储着所有添加到 epoll 中的需要监控的事件
    struct rb_root rbr;
    //双链表中则存放着将要通过 epoll_wait 返回给用户的满足条件的事件
    struct list_head rdlist;
}
```
每一个epoll对象都有一个独立的eventpoll结构体，用于存放通过epoll_ctl方法向epoll对象中添加进来的事件。这些事件都会挂载在红黑树中，如此，重复添加的事件就可以通过红黑树而高效的识别出来(红黑树的插入时间效率是lgn，其中n为树的高度)。

而所有添加到 epoll 中的事件都会与设备（网卡）驱动程序建立回调关系，也就是说，当相应的事件发生时会调用这个回调方法。这个回调方法在内核中叫 ep_poll_callback, 它会将发生的事件添加到 rdlist 双链表中。

在epoll 中，对于每一个事件，都会建立一个 epitem 结构体，如下所示
```c
struct epitem {
    struct rb_node rbn; //红黑树节点
    struct list_head rbllink; //双向链表节点
    struct epoll_filefd ffd; //事件句柄信息
    struct eventpoll *ep; //指向其所属的 eventpoll 对象
    struct epoll_event event; //期待发生的事件类型
}
```
当调用epoll_wait检查是否有事件发生时，只需要检查eventpoll对象中的rdlist双链表中是否有epitem元素即可。如果rdlist不为空，则把发生的事件复制到用户态，同时将事件数量返回给用户。

通过红黑树和双链表数据结构，并结合回调机制，造就了epoll的高效。

如此一来，只需要在进程启动时建立一个epoll对象，然后在需要的时候向这个epoll对象中添加或者删除连接。同时，epoll_wait的效率也非常高，因为调用epoll_wait时，并没有一股脑的向操作系统复制所有连接的句柄数据，内核也不需要去遍历全部的连接。