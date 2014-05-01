#pragma once
#include "stdafxppapi.h"

const long READ_BUFFER_SIZE = 32768;
class GetUrl
{
public:
	static GetUrl* Create(pp::Instance* instance_, const std::string& url);
	//����
	void Start();							//����
	void OnOpen(int32_t result);			//�򿪵Ļص�����
	void ReportResult(const std::string& fname, const std::string& text, bool success);
	void ReadBody();						//��ȡ��������
	void OnRead(int32_t result);			//��ȡ�Ļص�����
	void AppendDataBytes(const char* buffer, int32_t num_bytes);		//����

private:
	GetUrl(pp::Instance* instance, const std::string& url) 
		: m_pInstance_(instance)
		, m_url_(url)
		, m_UrlRequestInfo_(instance)			//����һ���µ�
		, m_UrlLoad_(instance)					//����һ���µ�
		, cc_factory_(this)
	{
		pp::Var theUrl(url.c_str());
		m_UrlRequestInfo_.SetURL(theUrl);
		m_UrlRequestInfo_.SetMethod("GET");
		m_UrlRequestInfo_.SetRecordDownloadProgress(true);
		m_buffer_ = new char[READ_BUFFER_SIZE];
	}
	~GetUrl()
	{
		delete[] m_buffer_;
		m_buffer_ = NULL;
	}
	pp::Instance* m_pInstance_;				//ʵ��ָ��
	std::string m_url_;						//url
	pp::URLLoader m_UrlLoad_;			
	pp::URLRequestInfo m_UrlRequestInfo_;	
	std::string url_response_body_;			
	pp::CompletionCallbackFactory<GetUrl> cc_factory_;
	char* m_buffer_;						//���ݴ洢��
};