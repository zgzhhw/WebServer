#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>    // 正则表达式
#include <errno.h>     
#include <mysql/mysql.h>  //mysql

#include "../buffer/buffer.h"
#include "../log/log.h"
#include "../pool/sqlconnpool.h"

class HttpRequest {
public:
    enum PARSE_STATE // 解析状态
    {
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,        
    };
    
    HttpRequest() { Init(); }
    ~HttpRequest() = default;

    void Init();
    bool parse(Buffer& buff);   

    std::string path() const; // 获取路径
    std::string& path();      // 获取路径
    std::string method() const; // 获取方法
    std::string version() const; // 获取版本
    std::string GetPost(const std::string& key) const;  // 获取post请求, 重载
    std::string GetPost(const char* key) const; // 获取post请求

    bool IsKeepAlive() const;

private:
    bool ParseRequestLine_(const std::string& line);    // 处理请求行
    void ParseHeader_(const std::string& line);         // 处理请求头
    void ParseBody_(const std::string& line);           // 处理请求体

    void ParsePath_();                                  // 处理请求路径
    void ParsePost_();                                  // 处理Post事件
    void ParseFromUrlencoded_();                        // 从url种解析编码

    // 从url中解析编码
    static bool UserVerify(const std::string& name, const std::string& pwd, bool isLogin);  // 用户验证

    PARSE_STATE state_; // 解析状态
    std::string method_, path_, version_, body_;    // 方法，路径，版本，请求体
    std::unordered_map<std::string, std::string> header_;   // 请求头
    std::unordered_map<std::string, std::string> post_;    // post请求

    static const std::unordered_set<std::string> DEFAULT_HTML;  // 默认html
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG; // 默认html标签
    static int ConverHex(char ch);  // 16进制转换为10进制
};

#endif
