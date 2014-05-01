#pragma once
#include "ShaderData.h"
class Tile
{
public:
	Tile(CString fileName)
		: m_TileFileName_(fileName)
		, m_TileHeight_(256)
		, m_TileWidth_(256)
		, m_pData_(NULL)
		, m_texttureID_(0)
		, m_bDecodeSuccess(false)
		, m_bFitTexture_(false)
	{
		glGenBuffers(1, &m_vertID_);
	}
	Tile()
		: m_TileFileName_(L"")
		, m_TileHeight_(256)
		, m_TileWidth_(256)
		, m_pData_(NULL)
		, m_texttureID_(0)
		, m_bDecodeSuccess(false)
		, m_bFitTexture_(false)
	{
		glGenBuffers(1, &m_vertID_);
	}
	~Tile()
	{
		if(m_pData_)
			delete[] m_pData_;
		if (m_texttureID_ > 0)
			glDeleteBuffers(1, &m_texttureID_);
		if(m_vertID_)
			glDeleteBuffers(1, &m_vertID_);

	}
	//����
	void Draw();

	//���ø�����ͼ�� �ĽǶ���
	void SetTheVerts(const int& ColNumer, const int& ColWidth, const int& ColDif				//�кš��п���ƫ��
		, const int& RowNumber, const int& RowHeight, const int& RowDif);					//�кš��п���ƫ��

	//����
	bool DeCodeTheTile();

	//�ַ�������
	bool DeCodeTheTile(const std::string& theStringData);

	//��������
	bool CreateTheTextFromGdiBitmap(Gdiplus::Bitmap* bitmap);

	//����������
	//�Ƿ���Ҫ����
	void FitTheTextureID(bool FitTheTexture, pp::Graphics3D* Grapics3d = NULL);
	
	//�����Ƿ�ɹ�
	bool m_bDecodeSuccess;
	int m_TileWidth_;					//��Ƭ���
	int m_TileHeight_;					//��Ƭ�߶�

	bool m_bFitTexture_;				//�Ƿ��Ѿ�����ͼ

private:
	CString m_TileFileName_;			//ͼƬ��·��
	Vertex m_verts_[4];					//����
	GLubyte* m_pData_;					//ͼƬ����

	GLuint m_texttureID_;				//����
	GLuint m_vertID_;					//����
};