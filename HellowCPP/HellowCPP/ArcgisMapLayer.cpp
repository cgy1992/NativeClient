#include "ArcgisMapLayer.h"
void CArcgisMapLayer::Draw()
{
	//获取地图图片数据 	//绘制
	GetMapData(this->m_js, this->m_CenLon, this->m_CenLat, m_CtrlWidth, m_CtrlHeight);
}

void CArcgisMapLayer::GetMapData(const int& js, const double& midX, const double& midY , const int& windowWidth, const int& windowHeight)
{
	//.XML文件
	if (!m_ReadConfig_)
	{
		std::string theXmlPath = m_MapPath + "\\Layers\\conf.xml";
		GetMapConfig(theXmlPath);
		m_ReadConfig_ = true;
	}
	//获取切片数据
	GetMapTile(js, midX, midY, windowWidth, windowHeight);
	
}
void CArcgisMapLayer::GetMapTile(const int& js, const double& midX, const double& midY , const int& windowWidth, const int& windowHeight)
{
	DWORD theTime = 0;
	if (js >= 0 && js <= m_MaxLevel)
	{
		//地图 m_TileOriginX_ m_TileOriginY_对应的图片号码是 0， 0
		//each picture equal longtitude latitude
		double eachTileW = m_TileCols_ * m_MapDataStoreTemp_[js].resolution;
		double eachTileH = m_TileRows_ * m_MapDataStoreTemp_[js].resolution;
		
		//中心点的图片编号 因为图片编号是从0，0开始的
		int midxdiffCols = floor((midX - m_TileOriginX_) / eachTileW);
		int midydiffRows = floor((m_TileOriginY_ - midY) / eachTileH);

		//中心点距离所在层的起始图片左上角的像素差
		int midxtotaldiff = (midX - m_TileOriginX_) / m_MapDataStoreTemp_[js].resolution;
		int midytotaldiff = (m_TileOriginY_ - midY) / m_MapDataStoreTemp_[js].resolution;

		//中心点在所在图片的像素偏差
		int midxdifpix = midxtotaldiff - midxdiffCols * m_TileCols_;
		int middifypix = midytotaldiff - midydiffRows * m_TileRows_;

		//窗口左上角对应与地图区域的图片位置编号
		int topleftCols = floor((midxtotaldiff - windowWidth / 2.0) / (float)m_TileCols_);
		int topleftRows = floor((midytotaldiff - windowHeight / 2.0) / (float)m_TileRows_);
		
		//控件左上角的像素坐标 和 该编号的图片的左上角的像素坐标的偏差
		int leftxTopDiffToCtrl = (midxtotaldiff - windowWidth / 2.0) - topleftCols * m_TileCols_;
		int leftyTopDiffToCtrl = (midytotaldiff - windowHeight / 2.0) - topleftRows * m_TileRows_;
		
		//当前窗口所占的图片张数
		long currentWindowCols = ceil(windowWidth / (float)m_TileCols_) + 1;
		long currentWindowRows = ceil(windowHeight / (float)m_TileRows_) + 1;

		//窗口右下角的图片编号
		long bottomrightCols = topleftCols + currentWindowCols;
		long bottomrightRows = topleftRows + currentWindowRows;

		CString theTilePath;
		{
			CString thePath;
			StringToCString(m_MapPath, thePath);
			theTilePath.Format(L"%s\\Layers\\_alllayers\\L%02d\\", thePath, js);
		}
		//从 R:topleftRows C:topleftCols 开始 共计 currentWindowRows * currentWindowCols
		for (int iRows = topleftRows; iRows < bottomrightRows; iRows++)		//行的遍历
		{
			CString strRow;
			strRow.Format(L"%sR%08x\\", theTilePath,  iRows);
			for (int iCols = topleftCols; iCols < bottomrightCols; iCols++)	//列的遍历
			{
				//定位图片
				CString strCol;
				{
					CString thePicType;
					StringToCString(m_PicType, thePicType);
					strCol.Format(L"%sC%08x.%s", strRow,  iCols, thePicType);

					//生成图片的索引号码
					unsigned long theKey = js * MAP_JS_SCALE + iRows * MAP_ROW_SCALE + iCols;
					//如果已经加载过了
					std::map<long, Tile*>::iterator theFindTile = m_TileStore_.find(theKey);
					//已经解析过了
					if (theFindTile != m_TileStore_.end())
					{
						//贴图的矩形计算
						theFindTile->second->SetTheVerts(iCols - topleftCols, m_TileCols_, leftxTopDiffToCtrl, 
							iRows - topleftRows, m_TileRows_, leftyTopDiffToCtrl);
					}
					//如果文件存在
					else if(PathFileExists(strCol))
					{
						Tile* theTile = new Tile(strCol);
						//矩形宽高
						theTile->m_TileWidth_ = m_TileCols_;
						theTile->m_TileHeight_ = m_TileRows_;
						//贴图的矩形计算
						theTile->SetTheVerts(iCols - topleftCols, m_TileCols_, leftxTopDiffToCtrl, 
											iRows - topleftRows, m_TileRows_, leftyTopDiffToCtrl);
						m_TileStore_[theKey] = theTile;
					}
				}
			}
		}
		theTime = clock();
		//获取系统CPU的数目
		//解码并且绘制
		for (int iRows = topleftRows; iRows < bottomrightRows; iRows++)		//行的遍历
		{
			for (int iCols = topleftCols; iCols < bottomrightCols; iCols++)	//列的遍历
			{
				//生成图片的索引号码
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
		//最大的空间控制
		int deleteNumber = currentWindowCols * currentWindowRows;
		if (m_TileStore_.size() > MAX_MAP_TEXTURE_TEMP)
		{
			//删除前面50个
			std::map<long, Tile*>::iterator theBegTile = m_TileStore_.begin();
			std::map<long, Tile*>::iterator theEndTile = m_TileStore_.end();
			//清理部分缓存
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
		//TileOrigin  x, y			//地图有图区域的左上角的坐标点
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
			//遍历有多少层
			MapDataStore theStore;
			for (TiXmlElement* node = theElement->FirstChildElement(); node; node = node->NextSiblingElement())
			{
				//每个层的数据
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