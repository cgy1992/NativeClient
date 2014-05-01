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
	//根据路径初始化
	bool Init(const CString& fileName = L"");
	//获取纹理的编号
	GLuint GetTheTextureID() const;


	long m_ImageWidth_;				//图片宽
	long m_ImageHeight_;			//图片高
private:
	CString m_ImageFileName_;		//图片全路径名字
	GLubyte* m_pdata_;				//图片数据内容

	GLuint m_texttureID_;			//纹理
};
