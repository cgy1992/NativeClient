// MarkerStyle.h : Declaration of the CMarkerStyle
#pragma once
#include "Style.h"
#include "stdafxppapi.h"
class CMarkerStyle
	: public CStyle
{
public:
	CMarkerStyle()
		: m_ImageFileName_(L"")
		, m_ImageHeight_(0)
		, m_ImageWidth_(0)
	{
		m_pdata_ = NULL;
		m_texttureID_ = 0;
	}

	~CMarkerStyle()
	{
		if (m_pdata_)
		{
			delete[] m_pdata_;
			m_pdata_ = NULL;
		}
		if(m_texttureID_ > 0)
		{
			glDeleteTextures(1, &m_texttureID_);
			m_texttureID_ = -1;
		}
	}
	//����·����ʼ��
	bool Init(const CString& fileName = L"");
	//��ȡ����ı��
	GLuint GetTheTextureID() const;


	long m_ImageWidth_;				//ͼƬ��
	long m_ImageHeight_;			//ͼƬ��
private:
	CString m_ImageFileName_;		//ͼƬȫ·������
	GLubyte* m_pdata_;				//ͼƬ��������

	GLuint m_texttureID_;			//����
};
