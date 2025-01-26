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
	bool initialize(); //  ��ʼ��Httpģ��
	void finalise(); //  �ر�Httpģ��

	// ���ֽ�ת��Ϊʮ�������ַ�
    inline uint8 toHex(const uint8 &x)
    {
        return x > 9 ? x -10 + 'A': x + '0';
    }

    inline uint8 fromHex(const uint8 &x)
    {
        return isdigit(x) ? x-'0' : x-'A'+10;
    }
 
	// ���ַ������� URL ����
    inline std::string URLEncode(const std::string &sIn)
    {
        std::string sOut;
        
        for( size_t ix = 0; ix < sIn.size(); ix++ )
        {      
            uint8 buf[4];
            memset( buf, 0, 4 );
            
            if( isalnum( (uint8)sIn[ix] ) ) //  ����ַ�����ĸ�����֣���ֱ����ӵ�buf��
            {      
                buf[0] = sIn[ix];
            }
            //else if ( isspace( (uint8)sIn[ix] ) ) //ò�ưѿո�����%20����+������
            //{
            //    buf[0] = '+';
            //}
            else //  ���򣬽��ַ�ת��Ϊ16���ƣ�����ӵ�buf��
            {
                buf[0] = '%';
                buf[1] = toHex( (uint8)sIn[ix] >> 4 );
                buf[2] = toHex( (uint8)sIn[ix] % 16);
            }
            
            sOut += (char *)buf; //  ��bufת��Ϊ�ַ�������ӵ�sOut��
        }
        
        return sOut;
    };

	// ���ַ������� URL ����
    inline std::string URLDecode(const std::string &sIn)
    {
        std::string sOut;
        
        for( size_t ix = 0; ix < sIn.size(); ix++ ) //  ���������ַ���
        {
            uint8 ch = 0;
            if(sIn[ix]=='%') //  �����ǰ�ַ���%����н���
            {
                ch = (fromHex(sIn[ix+1])<<4); //  ��%��������ַ�ת��Ϊʮ��������
                ch |= fromHex(sIn[ix+2]);
                ix += 2; //  �����Ѿ�����������ַ�
            }
            else if(sIn[ix] == '+') //  �����ǰ�ַ���+����ת��Ϊ�ո�
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

	// HTTP ������
	class Request
	{
	public:
		enum Status {
			OK = 0, //  �ɹ�
			INVALID_OPT = 1, //  ��Чѡ��
			PERFORM_ERROR = 2, //  ִ�д���
			OPENFILE_ERROR = 3, //  ���ļ�����
			INIT_ERROR = 4, //  ��ʼ������
		};

		/* 
			success, data
		*/
		typedef std::function<void(bool, const Request&, const std::string&)> Callback;

	public:
		Request();
		~Request();

		// �������� URL
		Status setURL(const std::string& url);

		/* example.com is redirected, so we tell libcurl to follow redirection */
		// �����Ƿ�����ض�������ض������
		Status setFollowURL(int maxRedirs);

		Status setPostData(const void* data, unsigned int size); //  ����POST����

		Status setTimeout(uint32 time); //  ���ó�ʱʱ��

		Status setHeader(const std::vector<std::string>& headers); //  ����HTTP����ͷ
		Status setHeader(const std::map<std::string, std::string>& headers);
		Status setHeader(const std::string& header);

		// ���� SSL ֤����Ϣ
		Status setCAInfo(const std::string& cainfo);
		Status setSSLCert(const std::string& sslCert);
		Status setSSLKey(const std::string& sslKey);
		Status setSSLKeyPassword(const std::string& sslKeyPwd);

		// set to check cert and host
		Status setSSLVerifyPeer(long v); //  ���ü��֤�������
		Status setSSLVerifyHost(long v);

		Status setProxy(const std::string& proxyIP, long proxyPort); //  ���ô���

		Status setCallback(const Callback& resultCallback) {
			resultCallback_ = resultCallback;
			return OK;
		}

		void callCallback(bool success);

		// ִ������
		Status perform();

		// ��ȡ HTTP ״̬��
		int getHttpCode() const { return httpCode_; }
		// ��ȡ���յ�����Ӧͷ
		const char* getReceivedHeader() const { return receivedHeader_.c_str(); }
		// ��ȡ���յ�����Ӧ����
		const char* getReceivedContent() const { return receivedContent_.c_str(); }
		// ��ȡ������Ϣ
		const char* getError() const { return (const char*)&error_[0]; }

		// ������Ӧͷ�Ļص�����
		static size_t receiveHeaderFunction(char *buffer, size_t size, size_t nitems, void *userdata);
		// ������Ӧ���ݵĻص�����
		static size_t receiveContentFunction(char *ptr, size_t size, size_t nmemb, void *userdata);

		// ��ȡ������ָ��
        void* pContext() {
            return pContext_;
        }

        // �����û�����
        void setUserargs(void* v) {
            userargs_ = v;
        }

        // ��ȡ�û�����
        void* getUserargs() const {
            return userargs_;
        }

        // ��ȡ���� URL
        const char* url() const {
            return url_.c_str();
        }

        // ���� HTTP ״̬��
        bool updateHttpCode();

	private:
		void* pContext_;         // ������ָ��
        void* headers_;          // ����ͷָ��
        MemoryStream* postData_; // POST ����ָ��
        long httpCode_;          // HTTP ״̬��
        char error_[MAX_BUF];    // ������Ϣ������
        bool hasSetRedirect_;    // �Ƿ������ض���
        int retryTimes_;         // ���Դ���

        std::string receivedContent_; // ���յ�����Ӧ����
        std::string receivedHeader_;  // ���յ�����Ӧͷ

        Callback resultCallback_;     // �ص�����
        bool called_;                 // �Ƿ��ѵ��ûص�����

        bool setVerifySSL_;           // �Ƿ����� SSL ��֤
        void* userargs_;              // �û�����

        std::string url_;             // ���� URL
	};

	//һ�����ڴ��� HTTP �첽������࣬���̳��� TimerHandler�������ṩ�˶��ַ�����ִ�� HTTP ���󣬲�֧����������ͷ�ͻص�������
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

		// ��ȡ������ָ��
		void* pContext() {
			return pContext_;
		}

		virtual void handleTimeout(TimerHandle, void * pUser); //  ����ʱ

	public:
		int still_running;    // ��ǰ�������е�������
        Request* pRequest;    // ��ǰ����ָ��
        TimerHandle timerHandle; // ��ʱ�����

	private:
		void* pContext_; //  ������ָ��

	};

	// ִ���첽 HTTP ����
	Request::Status perform(Request* pRequest);
	Request::Status perform(const std::string& url, const Request::Callback& resultCallback,
		const std::map<std::string, std::string>& headers = std::map<std::string, std::string>());	//  ִ�����󣬴��ص�������ͷ����Ϣ
	Request::Status perform(const std::string& url, const Request::Callback& resultCallback, const std::string& postData,
		const std::map<std::string, std::string>& headers = std::map<std::string, std::string>());	//  ִ�����󣬴��ص�������ͷ����Ϣ��POST����
}
}
}

#endif // KBE_HTTP_UTILTY_H


