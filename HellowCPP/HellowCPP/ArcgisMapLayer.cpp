#include "ArcgisMapLayer.h"
void CArcgisMapLayer::Draw()
{
	//��ȡ��ͼͼƬ���� 	//����
	GetMapData(this->m_js, this->m_CenLon, this->m_CenLat, m_CtrlWidth, m_CtrlHeight);
}

void CArcgisMapLayer::GetMapData(const int& js, const double& midX, const double& midY , const int& windowWidth, const int& windowHeight)
{
	//.XML�ļ�
	if (!m_ReadConfig_)
	{
		std::string theXmlPath = m_MapPath + "\\Layers\\conf.xml";
		GetMapConfig(theXmlPath);
		m_ReadConfig_ = true;
	}
	//��ȡ��Ƭ����
	GetMapTile(js, midX, midY, windowWidth, windowHeight);
	
}
void CArcgisMapLayer::GetMapTile(const int& js, const double& midX, const double& midY , const int& windowWidth, const int& windowHeight)
{
	DWORD theTime = 0;
	if (js >= 0 && js <= m_MaxLevel)
	{
		//��ͼ m_TileOriginX_ m_TileOriginY_��Ӧ��ͼƬ������ 0�� 0
		//each picture equal longtitude latitude
		double eachTileW = m_TileCols_ * m_MapDataStoreTemp_[js].resolution;
		double eachTileH = m_TileRows_ * m_MapDataStoreTemp_[js].resolution;
		
		//���ĵ��ͼƬ��� ��ΪͼƬ����Ǵ�0��0��ʼ��
		int midxdiffCols = floor((midX - m_TileOriginX_) / eachTileW);
		int midydiffRows = floor((m_TileOriginY_ - midY) / eachTileH);

		//���ĵ�������ڲ����ʼͼƬ���Ͻǵ����ز�
		int midxtotaldiff = (midX - m_TileOriginX_) / m_MapDataStoreTemp_[js].resolution;
		int midytotaldiff = (m_TileOriginY_ - midY) / m_MapDataStoreTemp_[js].resolution;

		//���ĵ�������ͼƬ������ƫ��
		int midxdifpix = midxtotaldiff - midxdiffCols * m_TileCols_;
		int middifypix = midytotaldiff - midydiffRows * m_TileRows_;

		//�������ϽǶ�Ӧ���ͼ�����ͼƬλ�ñ��
		int topleftCols = floor((midxtotaldiff - windowWidth / 2.0) / (float)m_TileCols_);
		int topleftRows = floor((midytotaldiff - windowHeight / 2.0) / (float)m_TileRows_);
		
		//�ؼ����Ͻǵ��������� �� �ñ�ŵ�ͼƬ�����Ͻǵ����������ƫ��
		int leftxTopDiffToCtrl = (midxtotaldiff - windowWidth / 2.0) - topleftCols * m_TileCols_;
		int leftyTopDiffToCtrl = (midytotaldiff - windowHeight / 2.0) - topleftRows * m_TileRows_;
		
		//��ǰ������ռ��ͼƬ����
		long currentWindowCols = ceil(windowWidth / (float)m_TileCols_) + 1;
		long currentWindowRows = ceil(windowHeight / (float)m_TileRows_) + 1;

		//�������½ǵ�ͼƬ���
		long bottomrightCols = topleftCols + currentWindowCols;
		long bottomrightRows = topleftRows + currentWindowRows;

		CString theTilePath;
		{
			CString thePath;
			StringToCString(m_MapPath, thePath);
			theTilePath.Format(L"%s\\Layers\\_alllayers\\L%02d\\", thePath, js);
		}
		//�� R:topleftRows C:topleftCols ��ʼ ���� currentWindowRows * currentWindowCols
		for (int iRows = topleftRows; iRows < bottomrightRows; iRows++)		//�еı���
		{
			CString strRow;
			strRow.Format(L"%sR%08x\\", theTilePath,  iRows);
			for (int iCols = topleftCols; iCols < bottomrightCols; iCols++)	//�еı���
			{
				//��λͼƬ
				CString strCol;
				{
					CString thePicType;
					StringToCString(m_PicType, thePicType);
					strCol.Format(L"%sC%08x.%s", strRow,  iCols, thePicType);

					//����ͼƬ����������
					unsigned long theKey = js * MAP_JS_SCALE + iRows * MAP_ROW_SCALE + iCols;
					//����Ѿ����ع���
					std::map<long, Tile*>::iterator theFindTile = m_TileStore_.find(theKey);
					//�Ѿ���������
					if (theFindTile != m_TileStore_.end())
					{
						//��ͼ�ľ��μ���
						theFindTile->second->SetTheVerts(iCols - topleftCols, m_TileCols_, leftxTopDiffToCtrl, 
							iRows - topleftRows, m_TileRows_, leftyTopDiffToCtrl);
					}
					//����ļ�����
					else if(PathFileExists(strCol))
					{
						Tile* theTile = new Tile(strCol);
						//���ο��
						theTile->m_TileWidth_ = m_TileCols_;
						theTile->m_TileHeight_ = m_TileRows_;
						//��ͼ�ľ��μ���
						theTile->SetTheVerts(iCols - topleftCols, m_TileCols_, leftxTopDiffToCtrl, 
											iRows - topleftRows, m_TileRows_, leftyTopDiffToCtrl);
						m_TileStore_[theKey] = theTile;
					}
				}
			}
		}
		theTime = clock();
		//��ȡϵͳCPU����Ŀ
		//���벢�һ���
		for (int iRows = topleftRows; iRows < bottomrightRows; iRows++)		//�еı���
		{
			for (int iCols = topleftCols; iCols < bottomrightCols; iCols++)	//�еı���
			{
				//����ͼƬ����������
				unsigned long theKey = js * MAP_JS_SCALE + iRows * MAP_ROW_SCALE + iCols;
				std::map<long, Tile*>::iterator theFindTile = m_TileStore_.find(theKey);
				if (theFindTile != m_TileStore_.end())
				{
					if (!theFindTile->second->m_bDecodeSuccess)
					{
						theFindTile->second->m_bDecodeSuccess = theFindTile->second->DeCodeTheTile();
					}
					theFindTile->second->Draw();
				}
			}
		}
		theTime = clock() - theTime;
		theTime = clock();
		//���Ŀռ����
		int deleteNumber = currentWindowCols * currentWindowRows;
		if (m_TileStore_.size() > MAX_MAP_TEXTURE_TEMP)
		{
			//ɾ��ǰ��50��
			std::map<long, Tile*>::iterator theBegTile = m_TileStore_.begin();
			std::map<long, Tile*>::iterator theEndTile = m_TileStore_.end();
			//�����ֻ���
			for (int i = 0; theBegTile != theEndTile && i < deleteNumber; i++)
			{
				if(theBegTile->second)
					delete theBegTile->second;
				theBegTile->second = NULL;
				theBegTile = m_TileStore_.erase(theBegTile);
			}
		}
	}
}
void CArcgisMapLayer::GetMapConfig(std::string ConfigPath)
{
	TiXmlDocument doc;
	m_MapDataStoreTemp_.clear();
	if (doc.LoadFile(ConfigPath.c_str()))
	{
		TiXmlElement* theRootElement = doc.RootElement();
		//TileOrigin  x, y			//��ͼ��ͼ��������Ͻǵ������
		std::string theNodeName = "TileOrigin";
		TiXmlElement* theElement = GetNodePointerByName(theRootElement, theNodeName);
		std::string strOriginX = theElement->FirstChildElement()->FirstChild()->Value();
		std::string strOriginY = theElement->FirstChildElement()->NextSiblingElement()->FirstChild()->Value();
		m_TileOriginX_ = atof(strOriginX.c_str());
		m_TileOriginY_ = atof(strOriginY.c_str());

		//TileCols
		theNodeName = "TileCols";
		theElement = GetNodePointerByName(theRootElement, theNodeName);
		std::string strTileCol = theElement->FirstChild()->Value();
		m_TileCols_ = atoi(strTileCol.c_str());

		//TileRows
		theNodeName = "TileRows";
		theElement = GetNodePointerByName(theRootElement, theNodeName);
		strTileCol = theElement->FirstChild()->Value();
		m_TileRows_ = atoi(strTileCol.c_str());

		//DPI
		theNodeName = "DPI";
		theElement = GetNodePointerByName(theRootElement, theNodeName);
		strTileCol = theElement->FirstChild()->Value();
		m_DPI_ = atoi(strTileCol.c_str());

		//LODInfos
		theNodeName = "LODInfos";
		theElement = GetNodePointerByName(theRootElement, theNodeName);
		for(theElement = theElement->FirstChildElement(); theElement; theElement = theElement->NextSiblingElement())
		{
			//�����ж��ٲ�
			MapDataStore theStore;
			for (TiXmlElement* node = theElement->FirstChildElement(); node; node = node->NextSiblingElement())
			{
				//ÿ���������
				std::string theString = node->Value();
				if (theString.compare("LevelID") == 0)
				{
					theString = node->FirstChild()->Value();
					theStore.Level = atoi(theString.c_str());
				} 
				else if (theString.compare("Scale") == 0)
				{
					theString = node->FirstChild()->Value();
					theStore.scale = atof(theString.c_str());
				}
				else if (theString.compare("Resolution") == 0)
				{
					theString = node->FirstChild()->Value();
					theStore.resolution = atof(theString.c_str());
				}
				else
				{
				}
			}
			m_MapDataStoreTemp_[theStore.Level] = theStore;
			if (theStore.Level > m_MaxLevel)
			{
				m_MaxLevel = theStore.Level;
			}
		}
	}
}