#pragma once
#include "stdafxppapi.h"
#include "curl/curl.h"
#pragma comment(lib,"libcurl_imp.lib")

//CURL��������������ݵ�ʹ��
size_t curl_writer(void *buffer, size_t size, size_t count, void * stream)
{
	std::string * pStream = static_cast<std::string *>(stream);
	(*pStream).append((char *)buffer, size * count);
	return size * count;
};

class CurlLock
{
private:
	CRITICAL_SECTION m_curl_cs_;
public:
	CurlLock() { InitializeCriticalSection(&m_curl_cs_); }
	~CurlLock() { DeleteCriticalSection(&m_curl_cs_); }
	void Lock() { EnterCriticalSection(&m_curl_cs_); }
	void UnLock() { LeaveCriticalSection(&m_curl_cs_); }
};

class CurlConnect
{
private:
	CurlConnect()
	{
		curl_global_init(CURL_GLOBAL_ALL); 
	}

	~CurlConnect()
	{
		curl_global_cleanup();
	}


	static CurlConnect* m_instance_;

	class DeleteCurlConnect
	{
	public:
		DeleteCurlConnect() 
		{
			int i = 0;
			i++;
		}
		~DeleteCurlConnect()
		{
			if(m_instance_)
				delete m_instance_;
			m_instance_ = NULL;
		}
	};
	
	// just for delete
	static DeleteCurlConnect m_DeleteInstance_;

public:
	static CurlConnect* GetInstance();

	//�������ȡ���ݣ��������ݷ���theText��
	bool GetDataFromByHttp(const std::string& theURL, std::string& theText);

	/**
	* ʹ��select��������multi curl�ļ���������״̬
	* �����ɹ�����0������ʧ�ܷ���-1
	*/
	int curl_multi_select(CURLM * curl_m);

	/**
	* ����һ��easy curl���󣬽���һЩ�򵥵����ò���
	const std::string & sUrl,		//����URL
	std::string & sRsp,			//���ص���Ϣ�����ַ���
	unsigned int& uiTimeout);		//�ȴ�ʱ��
	*/
	CURL* curl_easy_handler(const std::string & sUrl, std::string & sRsp, unsigned int& uiTimeout);

	// waite for data
	void WateForMulitCurl(CURLM* curl_m);
};