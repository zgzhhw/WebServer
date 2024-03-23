#include "httpconn.h"
using namespace std;

const char* HttpConn::srcDir;
std::atomic<int> HttpConn::userCount;
bool HttpConn::isET;

HttpConn::HttpConn() 
{ 
    fd_ = -1;   // 文件描述符
    addr_ = { 0 };  // 地址
    isClose_ = true; // 是否关闭
};

HttpConn::~HttpConn() 
{ 
    Close(); 
};

// 初始化, 传入文件描述符和地址
void HttpConn::init(int fd, const sockaddr_in& addr) 
{
    // assert fd > 0,表示文件描述符有效
    assert(fd > 0);
    userCount++;    // 增加用户数
    addr_ = addr;
    fd_ = fd;
    writeBuff_.RetrieveAll();   // 清空写缓冲区
    readBuff_.RetrieveAll();    // 清空读缓冲区
    isClose_ = false;   // 未关闭
    LOG_INFO("Client[%d](%s:%d) in, userCount:%d", fd_, GetIP(), GetPort(), (int)userCount);
}

// 关闭
void HttpConn::Close() 
{
    response_.UnmapFile();
    if(isClose_ == false)
    {
        isClose_ = true; 
        userCount--;    // 减少用户数
        close(fd_);
        LOG_INFO("Client[%d](%s:%d) quit, UserCount:%d", fd_, GetIP(), GetPort(), (int)userCount);
    }
}

int HttpConn::GetFd() const 
{
    return fd_;
};

struct sockaddr_in HttpConn::GetAddr() const 
{
    return addr_;
}

const char* HttpConn::GetIP() const 
{
    return inet_ntoa(addr_.sin_addr);
}

int HttpConn::GetPort() const 
{
    return addr_.sin_port;
}

// 读取数据
ssize_t HttpConn::read(int* saveErrno) {
    ssize_t len = -1;
    do {
        len = readBuff_.ReadFd(fd_, saveErrno);
        if (len <= 0) {
            break;
        }
    } while (isET); // ET:边沿触发要一次性全部读出
    return len;
}

// 主要采用writev连续写函数
ssize_t HttpConn::write(int* saveErrno) {
    ssize_t len = -1;
    do 
    {
        len = writev(fd_, iov_, iovCnt_);   // 将iov的内容写到fd中
        if(len <= 0) {
            *saveErrno = errno;
            break;
        }
        if(iov_[0].iov_len + iov_[1].iov_len  == 0) { break; } /* 传输结束 */
        else if(static_cast<size_t>(len) > iov_[0].iov_len) 
        {
            // iov_[1].iov_base指向第二个iov的剩余内容
            iov_[1].iov_base = (uint8_t*) iov_[1].iov_base + (len - iov_[0].iov_len);
            // iov_[1].iov_len更新为剩余长度
            iov_[1].iov_len -= (len - iov_[0].iov_len);
            
            // 如果iov_[0].iov_len不为0,则表示第一个iov的内容已经全部写入
            if(iov_[0].iov_len) 
            {
                writeBuff_.RetrieveAll();
                iov_[0].iov_len = 0;
            }
        }
        // len <= iov_[0].iov_len
        else 
        {
            // 同理，更新iov_[0]的指针和长度，指向未写入的内容
            iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len; 
            iov_[0].iov_len -= len; 
            writeBuff_.Retrieve(len);
        }
    } while(isET || ToWriteBytes() > 10240);
    return len;
}

// 判断是否保持连接
bool HttpConn::process() 
{
    request_.Init();
    if(readBuff_.ReadableBytes() <= 0) 
    {
        return false;
    }
    else if(request_.parse(readBuff_)) 
    {    // 解析成功
        LOG_DEBUG("%s", request_.path().c_str());
        response_.Init(srcDir, request_.path(), request_.IsKeepAlive(), 200);
    } 
    else 
    {
        response_.Init(srcDir, request_.path(), false, 400);
    }

    response_.MakeResponse(writeBuff_); // 生成响应报文放入writeBuff_中
    // 响应头
    iov_[0].iov_base = const_cast<char*>(writeBuff_.Peek());
    iov_[0].iov_len = writeBuff_.ReadableBytes();
    iovCnt_ = 1;

    // 文件
    if(response_.FileLen() > 0  && response_.File()) 
    {
        iov_[1].iov_base = response_.File();
        iov_[1].iov_len = response_.FileLen();
        iovCnt_ = 2;
    }
    LOG_DEBUG("filesize:%d, %d  to %d", response_.FileLen() , iovCnt_, ToWriteBytes());
    return true;
}

