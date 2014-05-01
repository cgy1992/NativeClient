#include "Tile.h"
void Tile::FitTheTextureID(bool FitTheTexture, pp::Graphics3D* Grapics3d)
{
	if(Grapics3d)
		glSetCurrentContextPPAPI(Grapics3d->pp_resource());
	if(FitTheTexture)
	{
		if(!m_bFitTexture_)
		{
			//ͼƬ���ݷ��õ�gles��
			if (m_texttureID_ > 0)
				glDeleteBuffers(1, &m_texttureID_);
			//��������
			glGenBuffers(1, &m_texttureID_);
			Bind2DTextureFromBuffer(m_texttureID_, m_TileWidth_, m_TileHeight_, m_pData_);
			m_bFitTexture_ = true;
		}
	}
}
void Tile::Draw()
{
	FitTheTextureID(true);
	//������Ϣ���
	glBindBuffer(GL_ARRAY_BUFFER, m_vertID_);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), (GLvoid*)&m_verts_[0], GL_STATIC_DRAW);
	//������
	glBindTexture(GL_TEXTURE_2D, m_texttureID_);
	//���������Ϣ
	glVertexAttribPointer(g_TextureShaer.m_PositioinLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, loc));
	glEnableVertexAttribArray(g_TextureShaer.m_PositioinLoc);
	//���������Ϣ
	glVertexAttribPointer(g_TextureShaer.m_TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tu));
	glEnableVertexAttribArray(g_TextureShaer.m_TexCoord);
	//����������
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/*
	��ʱ��		012
				213
 0				2
 1				3
 */
void Tile::SetTheVerts(const int& ColNumer, const int& ColWidth, const int& ColDif				//�кš��п���ƫ��
	, const int& RowNumber, const int& RowHeight, const int& RowDif)					//�кš��п���ƫ��
{
	m_verts_[0].tu = 0;
	m_verts_[0].tv = 0;
	m_verts_[0].loc[0] = ColNumer * ColWidth - ColDif;
	m_verts_[0].loc[1] = RowNumber * RowHeight - RowDif;
	m_verts_[0].loc[2] = g_Zlocation;

	m_verts_[1].tu = 0;
	m_verts_[1].tv = 1;
	m_verts_[1].loc[0] = ColNumer * ColWidth - ColDif;
	m_verts_[1].loc[1] = RowNumber * RowHeight - RowDif + RowHeight;
	m_verts_[1].loc[2] = g_Zlocation;

	m_verts_[2].tu = 1;
	m_verts_[2].tv = 0;
	m_verts_[2].loc[0] = ColNumer * ColWidth - ColDif + ColWidth;
	m_verts_[2].loc[1] = RowNumber * RowHeight - RowDif;
	m_verts_[2].loc[2] = g_Zlocation;

	m_verts_[3].tu = 1;
	m_verts_[3].tv = 1;
	m_verts_[3].loc[0] = ColNumer * ColWidth - ColDif + ColWidth;
	m_verts_[3].loc[1] = RowNumber * RowHeight - RowDif + RowHeight;
	m_verts_[3].loc[2] = g_Zlocation;
}
bool Tile::DeCodeTheTile()
{
	//����·���ҵ��ļ�
	if (PathFileExists(m_TileFileName_))
	{
		try
		{
			//�ļ����ݽ���
			Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(m_TileFileName_);	//BITMAP
			CreateTheTextFromGdiBitmap(bitmap);
			DeleteObject(bitmap);
			return true;
		}
		catch (CMemoryException* e)
		{
			
		}
		catch (CFileException* e)
		{
		}
		catch (CException* e)
		{
		}
		return false;
	}
	return false;
}

bool Tile::CreateTheTextFromGdiBitmap(Gdiplus::Bitmap* bitmap)
{
	try
	{
		HBITMAP gdibitmap;
		bitmap->GetHBITMAP(Gdiplus::Color::White, &gdibitmap);					//GDIBITMAP
		if(m_pData_)
			delete[] m_pData_;
		DWORD bitmapSize = bitmap->GetHeight() * bitmap->GetWidth();
		if(bitmapSize == 0)
			return false;
		m_pData_ = new GLubyte[bitmapSize * 4];									//Create Memery
		CBitmap* cbitmap = CBitmap::FromHandle(gdibitmap);						//Get the handle;
		cbitmap->GetBitmapBits(bitmapSize * 4, m_pData_);		//get the picture data
		//from bgr to rgb
		for (int i = 0; i < bitmapSize; i++)
		{
			int tempBit = m_pData_[i * 4];
			m_pData_[i * 4] = m_pData_[i * 4 + 2];
			m_pData_[i * 4 + 2] = tempBit;
		}
		cbitmap->DeleteObject();
		DeleteObject(gdibitmap);
		return true;
	}
	catch (CMemoryException* e)
	{
		
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	return false;
}
bool Tile::DeCodeTheTile(const std::string& theStringData)
{
	if(theStringData.length() == 0)
		return false;
	//�ڴ����
	HGLOBAL global = ::GlobalAlloc(GMEM_MOVEABLE, theStringData.length());
	if(global == NULL)
		return false;
	bool theResult = false;
	void* theBuffer = GlobalLock(global);
	if(theBuffer != NULL)
	{
		memcpy(theBuffer, theStringData.c_str(), theStringData.length());
		::GlobalUnlock(global);
		CComPtr<IStream> pstm = NULL;
		CreateStreamOnHGlobal(global, TRUE, &pstm);
		//GDI
		Gdiplus::Bitmap* Image = Gdiplus::Bitmap::FromStream(pstm);
		theResult = CreateTheTextFromGdiBitmap(Image);
		DeleteObject(Image);
	}
	GlobalFree(global);
	return theResult;
}