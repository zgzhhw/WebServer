#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>
class Buffer {
public:
    Buffer(int initBuffSize = 1024); // 默认1k
    ~Buffer() = default; // 默认析构函数

    size_t WritableBytes() const;   // 可写的字节数
    size_t ReadableBytes() const ;  // 可读的字节数
    size_t PrependableBytes() const;    // 前面预留的字节数

    const char* Peek() const;   // 返回读的位置
    void EnsureWriteable(size_t len);   // 确保有足够的空间
    void HasWritten(size_t len);    // 写入len个字节

    void Retrieve(size_t len);  // 读走len个字节
    void RetrieveUntil(const char* end);    // 读走直到end

    void RetrieveAll(); // 读走所有
    std::string RetrieveAllToStr(); // 读走所有并返回

    const char* BeginWriteConst() const;    // 返回写的位置
    char* BeginWrite(); // 返回写的位置

    void Append(const std::string& str);    // 添加字符串, 重载
    void Append(const char* str, size_t len);   // 添加字符串
    void Append(const void* data, size_t len);  // 添加字符串
    void Append(const Buffer& buff);    // 添加buffer

    ssize_t ReadFd(int fd, int* Errno); // 从fd读
    ssize_t WriteFd(int fd, int* Errno);    // 写到fd

private:
    char* BeginPtr_();  // buffer开头
    const char* BeginPtr_() const;  // buffer开头
    void MakeSpace_(size_t len);    // 空间不够时扩容

    std::vector<char> buffer_;  // buffer，用vector<char>实现
    std::atomic<std::size_t> readPos_;  // 读的下标
    std::atomic<std::size_t> writePos_; // 写的下标
};

#endif //BUFFER_H

// buffer的结构
// |-----------------buffer_-----------------|
//            readPos_     writePos_
//               ^            ^
// |---已读数据---|---可读数据---|---可写数据---|

