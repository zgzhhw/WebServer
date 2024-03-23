# ifndef BLOCKQUEUE_H
# define BLOCKQUEUE_H

#include <deque>
#include <condition_variable> // 条件变量
#include <mutex>
#include <sys/time.h> // 获取时间
using namespace std;

template<typename T>
class BlockQueue {
public:
    explicit BlockQueue(size_t maxsize = 1000);
    ~BlockQueue();
    bool empty();
    bool full();
    void push_back(const T& item);
    void push_front(const T& item); 
    bool pop(T& item);  // 弹出的任务放入item
    bool pop(T& item, int timeout);  // 等待时间
    void clear(); // 清空队列
    T front();
    T back();
    size_t capacity();
    size_t size();

    void flush(); // 唤醒消费者
    void Close(); // 关闭队列

private:
    deque<T> deq_;                      // 底层数据结构
    mutex mtx_;                         // 锁
    bool isClose_;                      // 关闭标志
    size_t capacity_;                   // 容量
    condition_variable condConsumer_;   // 消费者条件变量
    condition_variable condProducer_;   // 生产者条件变量
};
// 构造函数
template<typename T>
BlockQueue<T>::BlockQueue(size_t maxsize) : capacity_(maxsize) 
{
    assert(maxsize > 0);
    isClose_ = false;
}

// 析构函数
template<typename T>
BlockQueue<T>::~BlockQueue() 
{
    Close();
}

// 关闭队列
template<typename T>
void BlockQueue<T>::Close() {
    // lock_guard<mutex> locker(mtx_); // 操控队列之前，都需要上锁
    // deq_.clear();                   // 清空队列
    clear();
    isClose_ = true;
    condConsumer_.notify_all(); // 唤醒所有消费者
    condProducer_.notify_all(); // 唤醒所有生产者
}

// 清空队列
template<typename T>
void BlockQueue<T>::clear() 
{
    lock_guard<mutex> locker(mtx_);  // 上锁, 退出作用域自动解锁
    deq_.clear();
}

// 判断队列是否为空
template<typename T>
bool BlockQueue<T>::empty() {
    lock_guard<mutex> locker(mtx_); // 上锁, 退出作用域自动解锁
    return deq_.empty();
}

// 判断队列是否满
template<typename T>
bool BlockQueue<T>::full() {
    lock_guard<mutex> locker(mtx_);
    return deq_.size() >= capacity_;
}

// 生产者生产任务，放在队列尾部
template<typename T>
void BlockQueue<T>::push_back(const T& item) 
{
    // 注意，条件变量需要搭配unique_lock
    unique_lock<mutex> locker(mtx_);    
    while(deq_.size() >= capacity_) 
    {   // 队列满了，需要等待
        condProducer_.wait(locker);     // 暂停生产，等待消费者唤醒生产条件变量
    }
    deq_.push_back(item);
    condConsumer_.notify_one();         // 唤醒一个消费者
}

// 生产者生产任务，放在队列头部
template<typename T>
void BlockQueue<T>::push_front(const T& item) 
{
    unique_lock<mutex> locker(mtx_);
    while(deq_.size() >= capacity_) {   // 队列满了，需要等待
        condProducer_.wait(locker);     // 暂停生产，等待消费者唤醒生产条件变量
    }
    deq_.push_front(item);
    condConsumer_.notify_one();         // 唤醒一个消费者
}

// 消费者消费任务
template<typename T>
bool BlockQueue<T>::pop(T& item) 
{
    unique_lock<mutex> locker(mtx_);
    while(deq_.empty()) {
        condConsumer_.wait(locker);     // 队列空了，需要等待
    }
    item = deq_.front();
    deq_.pop_front();
    condProducer_.notify_one();         // 唤醒生产者
    return true;
}

// 消费者消费任务，带超时时间
template<typename T>
bool BlockQueue<T>::pop(T &item, int timeout) 
{
    unique_lock<std::mutex> locker(mtx_);
    while(deq_.empty()){
        if(condConsumer_.wait_for(locker, std::chrono::seconds(timeout)) 
            == std::cv_status::timeout) // 超时
        {
            return false;
        }
        if(isClose_)
        {
            return false;
        }
    }
    item = deq_.front();
    deq_.pop_front();
    condProducer_.notify_one();
    return true;
}

// 返回队列头部元素
template<typename T>
T BlockQueue<T>::front() 
{
    lock_guard<std::mutex> locker(mtx_);
    return deq_.front();
}

// 返回队列尾部元素
template<typename T>
T BlockQueue<T>::back() 
{
    lock_guard<std::mutex> locker(mtx_);
    return deq_.back();
}

// 返回队列容量
template<typename T>
size_t BlockQueue<T>::capacity() 
{
    lock_guard<std::mutex> locker(mtx_);
    return capacity_;
}

// 返回队列大小
template<typename T>
size_t BlockQueue<T>::size() 
{
    lock_guard<std::mutex> locker(mtx_);
    return deq_.size();
}

// 唤醒消费者
template<typename T>
void BlockQueue<T>::flush() 
{
    condConsumer_.notify_one();
}
# endif
