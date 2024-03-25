# WebServer
* 描述：
  基于C++实现的高性能服务器

* 环境：
  Ubuntu 22.04


---

## 项目综述
* 利用I/O复用技术epoll与线程池实现多线程的Reactor高并发模型；

* 利用正则表达式与状态机解析HTTP请求报文，实现处理静态资源的请求，并发送响应报文；

* 利用标准库容器封装char，实现自动增长的缓冲区；

* 基于小根堆实现的定时器，关闭超时的非活动连接；

* 利用单例模式与阻塞队列实现异步的日志系统，记录服务器运行状态；

* 利用RAII机制实现了数据库连接池，减少数据库连接建立与关闭的开销，同时实现了用户注册登录功能。


## Usage
1. 安装配置mysql
2. mysql -u root -p  // 之后输入密码登录 
3. create database webserver;  // 建立yourdb库
4. use webserver; // 创建user表
5. CREATE TABLE user(
    username char(50) NULL,
    password char(50) NULL
    )ENGINE=InnoDB;
6. INSERT INTO user(username, password) VALUES('name', 'password'); // 添加数据，也可到时候注册，这里添加了可直接登录
7. git clone https://github.com/zgzhhw/WebServer
8. 在WebServer处打开终端，输入命令make进行编译
9. **./bin/server**启动服务器
10. 浏览器输入 ```localhost:1316```进入首页

---

## Tips
由于前端页面不够完善，尚且无法进行正常功能使用，欢迎dalao加入开发

## 参考与鸣谢
https://blog.csdn.net/weixin_51322383/article/details/130464403#commentBox
