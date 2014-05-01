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
	//绘制
	void Draw();

	//设置该张贴图的 四角顶点
	void SetTheVerts(const int& ColNumer, const int& ColWidth, const int& ColDif				//列号、列宽、列偏差
		, const int& RowNumber, const int& RowHeight, const int& RowDif);					//行号、行宽、行偏差

	//解码
	bool DeCodeTheTile();

	//字符串解码
	bool DeCodeTheTile(const std::string& theStringData);

	//创建纹理
	bool CreateTheTextFromGdiBitmap(Gdiplus::Bitmap* bitmap);

	//设置纹理缓存
	//是否需要设置
	void FitTheTextureID(bool FitTheTexture, pp::Graphics3D* Grapics3d = NULL);
	
	//解码是否成功
	bool m_bDecodeSuccess;
	int m_TileWidth_;					//切片宽度
	int m_TileHeight_;					//切片高度

	bool m_bFitTexture_;				//是否已经贴好图

private:
	CString m_TileFileName_;			//图片的路径
	Vertex m_verts_[4];					//顶点
	GLubyte* m_pData_;					//图片数据

	GLuint m_texttureID_;				//纹理
	GLuint m_vertID_;					//顶点
};