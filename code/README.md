## buffer
buffer中存放了生成缓冲区的代码，通过自增长的char类型数组实现

---

## http
http中存放了http request报文的解析代码与http response报文的生成代码，并且编写了相应的连接函数

---

## log
log文件夹中存放了日志相关的生成函数

---

## pool
pool文件夹中存放了线程池相关的代码，包括thread pool用于线程池进行高并发函数执行，SQL Conn pool进行数据库的单例连接，提高效率

---

## timer
timer存放了小根堆实现的定时器，定期清理超时事件

---

## server
server代码将所有的类串联起来，综合实现了webserver类
