// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com
#ifndef KBE_HTTP_UTILTY_H
#define KBE_HTTP_UTILTY_H

#include "common/common.h"
#include "common/timer.h"

namespace KBEngine 
{
class MemoryStream;
namespace Network
{
namespace Http
{ 
	bool initialize(); //  初始化Http模块
	void finalise(); //  关闭Http模块

	// 将字节转换为十六进制字符
    inline uint8 toHex(const uint8 &x)
    {
        return x > 9 ? x -10 + 'A': x + '0';
    }

    inline uint8 fromHex(const uint8 &x)
    {
        return isdigit(x) ? x-'0' : x-'A'+10;
    }
 
	// 对字符串进行 URL 编码
    inline std::string URLEncode(const std::string &sIn)
    {
        std::string sOut;
        
        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {      
            uint8 buf[4];
            memset( buf, 0, 4 );
            
            if( isalnum( (uint8)sIn[ix] ) ) //  如果字符是字母或数字，则直接添加到buf中
            {      
                buf[0] = sIn[ix];
            }
            //else if ( isspace( (uint8)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以
            //{
            //    buf[0] = '+';
            //}
            else //  否则，将字符转换为16进制，并添加到buf中
            {
                buf[0] = '%';
                buf[1] = toHex( (uint8)sIn[ix] >> 4 );
                buf[2] = toHex( (uint8)sIn[ix] % 16);
            }
            
            sOut += (char *)buf; //  将buf转换为字符串并添加到sOut中
        }
        
        return sOut;
    };

	// 对字符串进行 URL 解码
    inline std::string URLDecode(const std::string &sIn)
    {
        std::string sOut;
        
        for( size_t ix = 0; ix < sIn.size(); ix++ ) //  遍历输入字符串
        {
            uint8 ch = 0;
            if(sIn[ix]=='%') //  如果当前字符是%则进行解码
            {
                ch = (fromHex(sIn[ix+1])<<4); //  将%后的两个字符转换为十六进制数
                ch |= fromHex(sIn[ix+2]);
                ix += 2; //  跳过已经解码的两个字符
            }
            else if(sIn[ix] == '+') //  如果当前字符是+则将其转换为空格
            {
                ch = ' ';
            }
            else
            {
                ch = sIn[ix];
            }
            
            sOut += (char)ch;
        }
        
        return sOut;
    }

	// HTTP 请求类
	class Request
	{
	public:
		enum Status {
			OK = 0, //  成功
			INVALID_OPT = 1, //  无效选项
			PERFORM_ERROR = 2, //  执行错误
			OPENFILE_ERROR = 3, //  打开文件错误
			INIT_ERROR = 4, //  初始化错误
		};

		/* 
			success, data
		*/
		typedef std::function<void(bool, const Request&, const std::string&)> Callback;

	public:
		Request();
		~Request();

		// 设置请求 URL
		Status setURL(const std::string& url);

		/* example.com is redirected, so we tell libcurl to follow redirection */
		// 设置是否跟随重定向及最大重定向次数
		Status setFollowURL(int maxRedirs);

		Status setPostData(const void* data, unsigned int size); //  设置POST数据

		Status setTimeout(uint32 time); //  设置超时时间

		Status setHeader(const std::vector<std::string>& headers); //  设置HTTP请求头
		Status setHeader(const std::map<std::string, std::string>& headers);
		Status setHeader(const std::string& header);

		// 设置 SSL 证书信息
		Status setCAInfo(const std::string& cainfo);
		Status setSSLCert(const std::string& sslCert);
		Status setSSLKey(const std::string& sslKey);
		Status setSSLKeyPassword(const std::string& sslKeyPwd);

		// set to check cert and host
		Status setSSLVerifyPeer(long v); //  设置检查证书和主机
		Status setSSLVerifyHost(long v);

		Status setProxy(const std::string& proxyIP, long proxyPort); //  设置代理

		Status setCallback(const Callback& resultCallback) {
			resultCallback_ = resultCallback;
			return OK;
		}

		void callCallback(bool success);

		// 执行请求
		Status perform();

		// 获取 HTTP 状态码
		int getHttpCode() const { return httpCode_; }
		// 获取接收到的响应头
		const char* getReceivedHeader() const { return receivedHeader_.c_str(); }
		// 获取接收到的响应内容
		const char* getReceivedContent() const { return receivedContent_.c_str(); }
		// 获取错误信息
		const char* getError() const { return (const char*)&error_[0]; }

		// 接收响应头的回调函数
		static size_t receiveHeaderFunction(char *buffer, size_t size, size_t nitems, void *userdata);
		// 接收响应内容的回调函数
		static size_t receiveContentFunction(char *ptr, size_t size, size_t nmemb, void *userdata);

		// 获取上下文指针
        void* pContext() {
            return pContext_;
        }

        // 设置用户参数
        void setUserargs(void* v) {
            userargs_ = v;
        }

        // 获取用户参数
        void* getUserargs() const {
            return userargs_;
        }

        // 获取请求 URL
        const char* url() const {
            return url_.c_str();
        }

        // 更新 HTTP 状态码
        bool updateHttpCode();

	private:
		void* pContext_;         // 上下文指针
        void* headers_;          // 请求头指针
        MemoryStream* postData_; // POST 数据指针
        long httpCode_;          // HTTP 状态码
        char error_[MAX_BUF];    // 错误信息缓冲区
        bool hasSetRedirect_;    // 是否设置重定向
        int retryTimes_;         // 重试次数

        std::string receivedContent_; // 接收到的响应内容
        std::string receivedHeader_;  // 接收到的响应头

        Callback resultCallback_;     // 回调函数
        bool called_;                 // 是否已调用回调函数

        bool setVerifySSL_;           // 是否设置 SSL 验证
        void* userargs_;              // 用户参数

        std::string url_;             // 请求 URL
	};

	//一个用于处理 HTTP 异步请求的类，它继承自 TimerHandler。该类提供了多种方法来执行 HTTP 请求，并支持设置请求头和回调函数。
	class Requests : public TimerHandler
	{
	public:
		Requests();
		~Requests();

		/*
			http async-request
		*/
		Request::Status perform(Request* pRequest);
		Request::Status perform(const std::string& url, const Request::Callback& resultCallback, 
			const std::map<std::string, std::string>& headers = std::map<std::string, std::string>());
		Request::Status perform(const std::string& url, const Request::Callback& resultCallback, const std::string& postData,
			const std::map<std::string, std::string>& headers = std::map<std::string, std::string>());

		// 获取上下文指针
		void* pContext() {
			return pContext_;
		}

		virtual void handleTimeout(TimerHandle, void * pUser); //  处理超时

	public:
		int still_running;    // 当前正在运行的请求数
        Request* pRequest;    // 当前请求指针
        TimerHandle timerHandle; // 定时器句柄

	private:
		void* pContext_; //  上下文指针

	};

	// 执行异步 HTTP 请求
	Request::Status perform(Request* pRequest);
	Request::Status perform(const std::string& url, const Request::Callback& resultCallback,
		const std::map<std::string, std::string>& headers = std::map<std::string, std::string>());	//  执行请求，带回调函数和头部信息
	Request::Status perform(const std::string& url, const Request::Callback& resultCallback, const std::string& postData,
		const std::map<std::string, std::string>& headers = std::map<std::string, std::string>());	//  执行请求，带回调函数、头部信息和POST数据
}
}
}

#endif // KBE_HTTP_UTILTY_H


