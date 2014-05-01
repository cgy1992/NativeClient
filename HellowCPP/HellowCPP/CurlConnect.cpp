#include "CurlConnect.h"

CurlConnect* CurlConnect::m_instance_ = NULL;

CurlConnect* CurlConnect::GetInstance()
{
	static CurlLock theLock;
	theLock.Lock();
	if(m_instance_ == NULL)
		m_instance_ = new CurlConnect();
	theLock.UnLock();
	return m_instance_;
}

bool CurlConnect::GetDataFromByHttp(const std::string& theURL, std::string& theText)
{
	CURL* theCurl;
	CURLcode theReturn;
	theText = "";
	theCurl = curl_easy_init();
	theReturn = curl_easy_setopt(theCurl, CURLOPT_URL, theURL.c_str());
	theReturn = curl_easy_setopt(theCurl, CURLOPT_NOSIGNAL, 1); 
	theReturn = curl_easy_setopt(theCurl, CURLOPT_LOW_SPEED_LIMIT, 1L);
	theReturn = curl_easy_setopt(theCurl, CURLOPT_LOW_SPEED_TIME, 3L);
	theReturn = curl_easy_setopt(theCurl, CURLOPT_WRITEFUNCTION, curl_writer);
	theReturn = curl_easy_setopt(theCurl, CURLOPT_WRITEDATA, &theText);
	theReturn = curl_easy_perform(theCurl);
	curl_easy_cleanup(theCurl);
	if(CURLE_OK == theReturn)
		return true;
	else 
		return false;
}

CURL* CurlConnect::curl_easy_handler(const std::string & sUrl, std::string & sRsp, unsigned int& uiTimeout)
{
	CURL * curl = curl_easy_init();
	sRsp = "";
	curl_easy_setopt(curl, CURLOPT_URL, sUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	// write function //
	//curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
	//curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, uiTimeout);
	//curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, uiTimeout);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sRsp);
	return curl;
}

void CurlConnect::WateForMulitCurl(CURLM* curl_m)
{
	//�ȴ����ݵ����
	/*
	* ����curl_multi_perform����ִ��curl����
	* url_multi_perform����CURLM_CALL_MULTI_PERFORMʱ����ʾ��Ҫ�������øú���ֱ������ֵ����CURLM_CALL_MULTI_PERFORMΪֹ
	* running_handles�����������ڴ����easy curl������running_handlesΪ0��ʾ��ǰû������ִ�е�curl����
	*/
	int running_handles;
	while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(curl_m, &running_handles))
	{
		//cout << running_handles << endl;
	}

	/**
	* Ϊ�˱���ѭ������curl_multi_perform������cpu����ռ�õ����⣬����select�������ļ�������
	*/
	while (running_handles)
	{
		if (-1 == curl_multi_select(curl_m))
		{
			//cerr << "select error" << endl;
			break;
		} 
		else 
		{
			// select�������¼�������curl_multi_perform֪ͨcurlִ����Ӧ�Ĳ��� //
			while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(curl_m, &running_handles))
			{
				//cout << "select: " << running_handles << endl;
			}
		}
		//cout << "select: " << running_handles << endl;
	}
}
int CurlConnect::curl_multi_select(CURLM* curl_m)
{
	int ret = 0;

	struct timeval timeout_tv;
	fd_set  fd_read;
	fd_set  fd_write;
	fd_set  fd_except;
	int     max_fd = -1;

	// ע������һ��Ҫ���fdset,curl_multi_fdset����ִ��fdset����ղ���  //
	FD_ZERO(&fd_read);
	FD_ZERO(&fd_write);
	FD_ZERO(&fd_except);

	// ����select��ʱʱ��  //
	timeout_tv.tv_sec = 1;
	timeout_tv.tv_usec = 0;

	// ��ȡmulti curl��Ҫ�������ļ����������� fd_set //
	curl_multi_fdset(curl_m, &fd_read, &fd_write, &fd_except, &max_fd);

	/**
	* When max_fd returns with -1,
	* you need to wait a while and then proceed and call curl_multi_perform anyway.
	* How long to wait? I would suggest 100 milliseconds at least,
	* but you may want to test it out in your own particular conditions to find a suitable value.
	*/
	if (-1 == max_fd)
	{
		return -1;
	}

	/**
	* ִ�м��������ļ�������״̬�����ı��ʱ�򷵻�
	* ����0���������curl_multi_perform֪ͨcurlִ����Ӧ����
	* ����-1����ʾselect����
	* ע�⣺��ʹselect��ʱҲ��Ҫ����0���������ȥ�������ĵ�˵��
	*/
	int ret_code = ::select(max_fd + 1, &fd_read, &fd_write, &fd_except, &timeout_tv);
	switch(ret_code)
	{
	case -1:
		/* select error */
		ret = -1;
		break;
	case 0:
		/* select timeout */
	default:
		/* one or more of curl's file descriptors say there's data to read or write*/
		ret = 0;
		break;
	}

	return ret;
}
