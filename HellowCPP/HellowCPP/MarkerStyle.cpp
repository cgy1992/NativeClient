// MarkerStyle.cpp : Implementation of CMarkerStyle
#include "MarkerStyle.h"
//// CMarkerStyle
bool CMarkerStyle::Init(const CString& fileName)
{
	if (fileName.Compare(L"") != 0)
		m_ImageFileName_ = fileName;
	if(m_ImageFileName_.Compare(L"") == 0)
		return false;
	// Read the texture bits
	//����λͼ����
	Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(m_ImageFileName_);
	m_ImageWidth_ = bmp->GetWidth();
	m_ImageHeight_ = bmp->GetHeight();

	//����ͼƬ����
	if(m_pdata_ != NULL)
	{
		delete[] m_pdata_;
		m_pdata_ = NULL;
	}
	m_pdata_ = new BYTE[m_ImageWidth_ * m_ImageHeight_ * 4];
	
	//����λͼ���󴴽�GDIλͼ����
	HBITMAP hBitMap;
	bmp->GetHBITMAP(Gdiplus::Color::White, &hBitMap);
	
	//����GDIλͼ��ȡ�����CBitmao
	CBitmap* cbmp = CBitmap::FromHandle(hBitMap);
	if (cbmp == NULL)
		return false;

	//��ȡͼ������
	DWORD size = cbmp->GetBitmapBits(m_ImageWidth_ * m_ImageHeight_ * 4, m_pdata_);
	for (int i = 0; i < m_ImageWidth_ * m_ImageHeight_; i++)
	{
		BYTE tmp = (m_pdata_[i * 4]);
		m_pdata_[i * 4 + 0] = (m_pdata_[i * 4 + 2]); //r
		m_pdata_[i * 4 + 2] = tmp;//b
	}

	cbmp->DeleteObject();
	DeleteObject(hBitMap);
	DeleteObject(bmp);


	//��������
	if(m_texttureID_ > 0)
	{
		glDeleteTextures(1, &m_texttureID_);
		m_texttureID_ = -1;
	}
	//������ͼ����
	glGenTextures(1, &m_texttureID_);
	//����ͼ����
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //����һλ����
	glBindTexture(GL_TEXTURE_2D, m_texttureID_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_ImageWidth_, m_ImageHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pdata_);
	//���²�������
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 100, 100, 56, 56, GL_RGBA, GL_UNSIGNED_BYTE, m_pdata_);
	//����ģʽ
	//�Ŵ����
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	//����TS����ʱ��Ĳ���
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return true;
}

GLuint CMarkerStyle::GetTheTextureID() const
{
	return m_texttureID_;
}
