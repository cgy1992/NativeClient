#include "GetUrl.h"
#ifdef WIN32
#undef PostMessage
#endif
GetUrl* GetUrl::Create(pp::Instance* instance, const std::string& url) 
{
	return new GetUrl(instance, url);
}
void GetUrl::Start()
{
	if(m_buffer_ == NULL)
		exit(0);
	pp::CompletionCallback theCbFun = cc_factory_.NewCallback(&GetUrl::OnOpen);
	//��
	m_UrlLoad_.Open(m_UrlRequestInfo_, theCbFun);
}
void GetUrl::OnOpen(int32_t result)
{
	if (result == PP_OK)
	{
		//ȷ�����ܵ��ֽ���Ŀ
		int64_t bytes_received = 0;
		int64_t total_bytes_to_be_received = 0;
		//��ѯ���ؽ���
		if (m_UrlLoad_.GetDownloadProgress(&bytes_received, &total_bytes_to_be_received)) 
		{
			if (total_bytes_to_be_received > 0) 
			{
				url_response_body_.reserve(total_bytes_to_be_received);
			}
		}
		//����Ҫ��ѯ���ؽ���
		m_UrlRequestInfo_.SetRecordDownloadProgress(false);
		ReadBody();

	}
	else
	{
		ReportResult(m_url_, "get file", false);
	}
}
void GetUrl::ReportResult(const std::string& fname, const std::string& text, bool success) 
{
	if (m_pInstance_) 
	{
		if (success)
		{
			//�ɹ�
			pp::Var var_result(fname + "\t true" + text);
			m_pInstance_->PostMessage(var_result);
		} 
		else
		{
			//ʧ��
			pp::Var var_result(fname + "\t false" + text);
			m_pInstance_->PostMessage(var_result);
			delete this;
		}
	}
}

void GetUrl::ReadBody()
{
	//��ȡ�Ļص���������
	pp::CompletionCallback cc = cc_factory_.NewCallback(&GetUrl::OnRead);
	int32_t result = PP_OK;
	do
	{
		//��ȡһ����
		result = m_UrlLoad_.ReadResponseBody(m_buffer_, READ_BUFFER_SIZE, cc);
		if (result > 0)
		{
			//��������
			AppendDataBytes(m_buffer_, result);
		}
	}while(result > 0);
}

void GetUrl::AppendDataBytes(const char* buffer, int32_t num_bytes)
{
	if (num_bytes < 0)
	{
	} 
	else
	{
		num_bytes = num_bytes;
		std::string theBuffer(buffer, num_bytes);
		url_response_body_ += theBuffer;
	}
}

void GetUrl::OnRead(int32_t result)
{
	if (result == PP_OK)
	{
		delete[] m_buffer_;
		m_buffer_ = NULL;
		ReportResult(m_url_, url_response_body_, true);
	} 
	else if(result > 0)
	{
		AppendDataBytes(m_buffer_, result);
		ReadBody();
	}
}