#include "ServerMapLayer.h"

void CServerMapLayer::Draw()
{
	//��ȡ��ͼͼƬ���� 	//����
	GetMapData(this->m_js, this->m_CenLon, this->m_CenLat, m_CtrlWidth, m_CtrlHeight);
}

void CServerMapLayer::GetMapData(const int& js, const double& midX, const double& midY		//��ǰ�����߼������ĵ�
	, const int& windowWidth, const int& windowHeight)
{
	//����XML�ļ�
	//.XML�ļ�
	if (!m_ReadConfig_)
	{
		std::string theUrl;
		switch (m_MapType)
		{
		case MapType::ArcgisServerMap:
			//http://10.1.50.184:6080/ArcGIS/rest/services/hangzhou1022/MapServer?f=json
			theUrl = m_MapPath + "?f=json";
			break;
		case MapType::DegreeServermap:
			//http://10.1.50.121:8020/services/wmts?service=WMTS&request=GetCapabilities
			theUrl = m_MapPath + "/wmts?service=WMTS&request=GetCapabilities";
			break;
		case MapType::PGISMap:
			break;
		default:
			break;
		}
		GetMapConfig(theUrl);
	}
	//��ȡ��Ƭ����
	if(m_MapType == PGISMap)
		GetMapTilePGIS(js, midX, midY, windowWidth, windowHeight);
	else if(m_MapType == DegreeServermap)
	{
		m_TileOriginX_ = m_MapTileEffectiveArea_[js].m_TopLeftLong;
		m_TileOriginY_ = m_MapTileEffectiveArea_[js].m_TopLeftLat;
		m_TileCols_ = m_MapTileEffectiveArea_[js].m_TileWidth;
		m_TileRows_ = m_MapTileEffectiveArea_[js].m_TileHeight;
		GetMapTile(js, midX, midY, windowWidth, windowHeight);
	}
	else if(m_MapType == ArcgisServerMap)
		GetMapTile(js, midX, midY, windowWidth, windowHeight);
}
void CServerMapLayer::GetMapTilePGIS(const int& js, const double& midX, const double& midY, const int& windowWidth, const int& windowHeight)
{
	DWORD theTime = clock();	//���ܷ���
	if (js >= 0 && js <= m_MaxLevel)
	{
		//���Ͻǵ����к���
		long topleftCols = 0;
		long topleftRows = 0;
		//���½ǵ����к�
		long bottomrightCols = 0;
		long bottomrightRows = 0;
		//��ǰ������ռ��ͼƬ����
		long currentWindowCols = 0;
		long currentWindowRows = 0;
		//�ؼ����Ͻǵ��������� �� �ñ�ŵ�ͼƬ�����Ͻǵ����������ƫ��
		long leftxTopDiffToCtrl = 0;
		long leftyTopDiffToCtrl = 0;

		//each picture equal longtitude latitude
		double eachTileW = m_TileCols_ * m_MapDataStoreTemp_[js].resolution;
		double eachTileH = m_TileRows_ * m_MapDataStoreTemp_[js].resolution;
		//��ͼ m_TileOriginX_ m_TileOriginY_��Ӧ��ͼƬ������ 0�� 0
		//���ĵ��ͼƬ��� ��ΪͼƬ����Ǵ�0��0��ʼ��
		long midxdiffCols = floor((midX - m_TileOriginX_) / eachTileW);
		long midydiffRows = floor((midY - m_TileOriginY_) / eachTileH);

		//���ĵ�������ڲ����ʼͼƬ���½ǵ����ز�
		long midxtotaldiff = (midX - m_TileOriginX_) / m_MapDataStoreTemp_[js].resolution;
		long midytotaldiff = (midY - m_TileOriginY_) / m_MapDataStoreTemp_[js].resolution;

		//���ĵ�������ͼƬ������ƫ��
		long midxdifpix = midxtotaldiff - midxdiffCols * m_TileCols_;
		long middifypix = midytotaldiff - midydiffRows * m_TileRows_;

		//�������ϽǶ�Ӧ���ͼ�����ͼƬλ�ñ��
		topleftCols = floor((midxtotaldiff - windowWidth / 2.0) / (float)m_TileCols_);
		topleftRows = floor((midytotaldiff + windowHeight / 2.0) / (float)m_TileRows_);			

		//�ؼ����Ͻǵ��������� �� �ñ�ŵ�ͼƬ�����Ͻǵ����������ƫ��
		leftxTopDiffToCtrl = (midxtotaldiff - windowWidth / 2.0) - topleftCols * m_TileCols_;
		leftyTopDiffToCtrl = m_TileRows_ - ((midytotaldiff + windowHeight / 2.0) - topleftRows * m_TileRows_);

		//��ǰ������ռ��ͼƬ����
		currentWindowCols = ceil(windowWidth / (float)m_TileCols_) + 1;
		currentWindowRows = ceil(windowHeight / (float)m_TileRows_) + 1;

		//g_out_file_log << "picture number" << currentWindowRows * currentWindowCols << std::endl;
		//�������½ǵ�ͼƬ���
		bottomrightCols = topleftCols + currentWindowCols;
		bottomrightRows = topleftRows - currentWindowRows;
		{
			//����ȡͼƬ
			unsigned __int64 thekey = 0;
			long xCount = 0;				//�Ѿ�ʹ�ù����ڴ�
			long iUrlCount = 0;				//URLʹ��
			char theUrlChar[1024];
			//������صĲ���
			CURL *          CurlArray[MAX_MAP_TEXTURE_TEMP];
			std::string		theUrl[MAX_MAP_TEXTURE_TEMP];
			unsigned int	timeOut = 3;
			CURLM * curl_m = curl_multi_init();
			//int theZoom = js - m_ZoomOffset_;
			//�� R:topleftRows C:topleftCols ��ʼ ���� currentWindowRows * currentWindowCols
			for (int iRows = topleftRows; iRows > bottomrightRows; iRows--)		//�еı���
			{
				for (int iCols = topleftCols; iCols < bottomrightCols; iCols++)	//�еı���
				{
					//��λͼƬ
					thekey = GetTheTileKey(js, iRows, iCols);
					//����Ѿ����ع���
					std::map<unsigned __int64, Tile*>::iterator theFindTile = m_TileStore_.find(thekey);
					//�Ѿ���������
					if (theFindTile != m_TileStore_.end() && theFindTile->second->m_bDecodeSuccess)
					{
						//��ͼ�ľ��μ���
						theFindTile->second->SetTheVerts(iCols - topleftCols, m_TileCols_, leftxTopDiffToCtrl, 
							topleftRows - iRows, m_TileRows_, leftyTopDiffToCtrl);
						continue;
					}
					else if(theFindTile != m_TileStore_.end())
					{
						delete theFindTile->second;
						m_TileStore_.erase(theFindTile);
					}
					////û��Ҫȥ��������
					//if(iRows < 0 || iCols < 0)
					//{
					//	m_MapTileTempStore_[xCount].theKey = thekey;
					//	m_MapTileTempStore_[xCount].MapTileTemp = "";
					//	xCount++;
					//}
					////��Ҫ�������� 
					//else
					{
						//m_MapPath + "/tile" %s/%d/%d/%d  js r c
						//��URL
						int theSize = sizeof(theUrlChar);
						memset(theUrlChar, 0, sizeof(theUrlChar));
						sprintf(theUrlChar, m_TileUrlString_.c_str(), m_MapPath.c_str(), iCols, iRows, js);
						theUrl[iUrlCount] = theUrlChar;
						//������
						m_MapTileTempStore_[xCount].theKey = thekey;
						m_MapTileTempStore_[xCount].MapTileTemp = "";
						//��ʼ������
						CurlArray[iUrlCount] = m_CurlConnect_->curl_easy_handler(theUrl[iUrlCount], m_MapTileTempStore_[xCount].MapTileTemp, timeOut);
						curl_multi_add_handle(curl_m, CurlArray[iUrlCount]);
						iUrlCount++;
						xCount++;
					}//��Ҫ��������
					Tile* theTile = new Tile();
					//���ο��
					theTile->m_TileWidth_ = m_TileCols_;
					theTile->m_TileHeight_ = m_TileRows_;
					//��ͼ�ľ��μ���
					theTile->SetTheVerts(iCols - topleftCols, m_TileCols_, leftxTopDiffToCtrl, 
						topleftRows - iRows, m_TileRows_, leftyTopDiffToCtrl);
					m_TileStore_[thekey] = theTile;
				}
			}
			m_CurlConnect_->WateForMulitCurl(curl_m);
			// ����Ҫע��cleanup��˳�� //
			for (int idx = 0; idx < iUrlCount; ++idx)
			{
				curl_multi_remove_handle(curl_m, CurlArray[idx]);
			}
			for (int idx = 0; idx < iUrlCount; ++idx)
			{
				curl_easy_cleanup(CurlArray[idx]);
			}
			curl_multi_cleanup(curl_m);
			theTime = clock() - theTime;
			if(theTime > MAX_TIME_COST)
				g_out_file_log << "��ȡͼƬʱ�䣺" << theTime << std::endl;

			theTime = clock();
			if(xCount > m_NumberOfCpuCount * 4 + 2)		//���߳�
			{
				//InitializeCriticalSection(&g_EnterDrawSection);
				int maxHandleNumber = m_NumberOfCpuCount * 2;
				HANDLE* handle = new HANDLE[maxHandleNumber];
				long NumberOfSize = xCount / maxHandleNumber;
				ContRange* theRange = new ContRange[maxHandleNumber];
				for (int i = 0; i < maxHandleNumber - 1; i++)  
				{
					theRange[i].theBeignNumber = i * NumberOfSize;
					theRange[i].theEndNumber = theRange[i].theBeignNumber + NumberOfSize;
					theRange[i].theMapLayer = this;
					handle[i] = (HANDLE)_beginthreadex(NULL, 0, DecodeForServerFun, &theRange[i], 0, NULL); 
				}
				theRange[maxHandleNumber - 1].theBeignNumber = (maxHandleNumber - 1) * NumberOfSize;
				theRange[maxHandleNumber - 1].theEndNumber = xCount;
				theRange[maxHandleNumber - 1].theMapLayer = this;
				handle[maxHandleNumber - 1] = (HANDLE)_beginthreadex(NULL, 0, DecodeForServerFun, &theRange[maxHandleNumber - 1], 0, NULL);
				for (int i = 0; i < maxHandleNumber; i++) 
				{
					WaitForSingleObject(handle[i], 1000);
					CloseHandle(handle[i]);
				}
				delete[] handle;
				delete[] theRange;
				//DeleteCriticalSection(&g_EnterDrawSection);
			}
			else
				DoSingleDecode(0, xCount);	
			theTime = clock() - theTime;
			if(theTime > MAX_TIME_COST)
				g_out_file_log << " the time of decode:" << theTime << std::endl;
			theTime = clock();
		}
		//����
		{
			char buffer[1024];
			std::map<unsigned __int64, Tile*>::iterator theFindTile;
			std::map<unsigned __int64, Tile*>::iterator theEndTile = m_TileStore_.end();
			unsigned __int64 thekey;
			for (int iRows = topleftRows; iRows > bottomrightRows; iRows--)		//�еı���
			{
				for (int iCols = topleftCols; iCols < bottomrightCols; iCols++)	//�еı���
				{
					//��λͼƬ
					thekey = GetTheTileKey(js, iRows, iCols);
					theFindTile = m_TileStore_.find(thekey);
					if(theFindTile != theEndTile && theFindTile->second->m_bDecodeSuccess)
						theFindTile->second->Draw();
					else if(theFindTile != theEndTile)
					{
						sprintf(buffer, "http://10.1.50.58:8080/EzServer6/Maps/hzmap/EzMap?Service=getImage&Type=RGB&Col=%d&Row=%d&Zoom=%d&V=0.3", iCols, iRows, js);
						g_out_file_log << "Draw error" << buffer << std::endl;
					}
				}
			}
		}
		theTime = clock() - theTime;
		if(theTime > MAX_TIME_COST)
			g_out_file_log << "the time of draw:" << theTime << std::endl;
		theTime = clock();
		//���Ŀռ����
		int deleteNumber = currentWindowCols * currentWindowRows;
		if (m_TileStore_.size() > MAX_MAP_TEXTURE_TEMP)
		{
			//ɾ��ǰ��50��
			std::map<unsigned __int64, Tile*>::iterator theBegTile = m_TileStore_.begin();
			std::map<unsigned __int64, Tile*>::iterator theEndTile = m_TileStore_.end();
			//�����ֻ���
			for (int i = 0; theBegTile != theEndTile && i < deleteNumber; i++)
			{
				if(theBegTile->second)
					delete theBegTile->second;
				theBegTile->second = NULL;
				theBegTile = m_TileStore_.erase(theBegTile);
			}
		}
		theTime = clock() - theTime;
		if(theTime > MAX_TIME_COST)
			g_out_file_log << "the time of delete tile:" << theTime << std::endl;
		theTime = clock();
	}
}
void CServerMapLayer::GetMapTile(const int& js, const double& midX, const double& midY, const int& windowWidth, const int& windowHeight)
{
	DWORD theTime;	//���ܷ���
	//�����߼������
	if (js >= 0 && js <= m_MaxLevel)
	{
		//���Ͻǵ����к���
		long topleftCols = 0;
		long topleftRows = 0;
		//���½ǵ����к�
		long bottomrightCols = 0;
		long bottomrightRows = 0;
		//��ǰ������ռ��ͼƬ����
		long currentWindowCols = 0;
		long currentWindowRows = 0;
		//�ؼ����Ͻǵ��������� �� �ñ�ŵ�ͼƬ�����Ͻǵ����������ƫ��
		long leftxTopDiffToCtrl = 0;
		long leftyTopDiffToCtrl = 0;

		//each picture equal longtitude latitude
		double eachTileW = m_TileCols_ * m_MapDataStoreTemp_[js].resolution;
		double eachTileH = m_TileRows_ * m_MapDataStoreTemp_[js].resolution;
		{
			//��ͼ m_TileOriginX_ m_TileOriginY_��Ӧ��ͼƬ������ 0�� 0
			//���ĵ��ͼƬ��� ��ΪͼƬ����Ǵ�0��0��ʼ��
			long midxdiffCols = floor((midX - m_TileOriginX_) / eachTileW);
			long midydiffRows = floor((m_TileOriginY_ - midY) / eachTileH);

			//���ĵ�������ڲ����ʼͼƬ���Ͻǵ����ز�
			long midxtotaldiff = (midX - m_TileOriginX_) / m_MapDataStoreTemp_[js].resolution;
			long midytotaldiff = (m_TileOriginY_ - midY) / m_MapDataStoreTemp_[js].resolution;

			//���ĵ�������ͼƬ������ƫ��
			long midxdifpix = midxtotaldiff - midxdiffCols * m_TileCols_;
			long middifypix = midytotaldiff - midydiffRows * m_TileRows_;

			//�������ϽǶ�Ӧ���ͼ�����ͼƬλ�ñ��
			topleftCols = floor((midxtotaldiff - windowWidth / 2.0) / (float)m_TileCols_);
			topleftRows = floor((midytotaldiff - windowHeight / 2.0) / (float)m_TileRows_);

			//�ؼ����Ͻǵ��������� �� �ñ�ŵ�ͼƬ�����Ͻǵ����������ƫ��
			leftxTopDiffToCtrl = (midxtotaldiff - windowWidth / 2.0) - topleftCols * m_TileCols_;
			leftyTopDiffToCtrl = (midytotaldiff - windowHeight / 2.0) - topleftRows * m_TileRows_;

			//��ǰ������ռ��ͼƬ����
			currentWindowCols = ceil(windowWidth / (float)m_TileCols_) + 1;
			currentWindowRows = ceil(windowHeight / (float)m_TileRows_) + 1;

			//�������½ǵ�ͼƬ���
			bottomrightCols = topleftCols + currentWindowCols;
			bottomrightRows = topleftRows + currentWindowRows;
		}
		{
			//����ȡͼƬ
			unsigned __int64 thekey = 0;
			long xCount = 0;				//�Ѿ�ʹ�ù����ڴ�
			long iUrlCount = 0;				//URLʹ��
			char* theUrlChar = new char[m_MapPath.length() + 250];
			//������صĲ���
			CURL *          CurlArray[MAX_MAP_TEXTURE_TEMP];
			std::string		theUrl[MAX_MAP_TEXTURE_TEMP];
			unsigned int	timeOut = 3;
			CURLM * curl_m = curl_multi_init();

			long MaxBottomRowNumber = bottomrightRows;
			long MaxBottomColNumber = bottomrightCols;
			if(m_MapType == DegreeServermap)
			{
				MaxBottomRowNumber = m_MapTileEffectiveArea_[js].m_rowCount;
				MaxBottomColNumber = m_MapTileEffectiveArea_[js].m_colCount;
			}

			//�� R:topleftRows C:topleftCols ��ʼ ���� currentWindowRows * currentWindowCols
			for (int iRows = topleftRows; iRows < bottomrightRows; iRows++)		//�еı���
			{
				for (int iCols = topleftCols; iCols < bottomrightCols; iCols++)	//�еı���
				{
					//��λͼƬ
					thekey = GetTheTileKey(js, iRows, iCols);
					//����Ѿ����ع���
					std::map<unsigned __int64, Tile*>::iterator theFindTile = m_TileStore_.find(thekey);
					//�Ѿ���������
					if (theFindTile != m_TileStore_.end())
					{
						//��ͼ�ľ��μ���
						theFindTile->second->SetTheVerts(iCols - topleftCols, m_TileCols_, leftxTopDiffToCtrl, 
							iRows - topleftRows, m_TileRows_, leftyTopDiffToCtrl);
						continue;
					}
					//û��Ҫȥ��������
					if(iRows < 0 || iCols < 0)
					{
						m_MapTileTempStore_[xCount].theKey = thekey;
						m_MapTileTempStore_[xCount].MapTileTemp = "";
						xCount++;
					}
					else if(iRows > MaxBottomRowNumber || iCols	 > MaxBottomColNumber)
					{
						m_MapTileTempStore_[xCount].theKey = thekey;
						m_MapTileTempStore_[xCount].MapTileTemp = "";
						xCount++;
					}
					//��Ҫ�������� 
					else
					{
						//m_MapPath + "/tile" %s/%d/%d/%d  js r c
						//��URL
						memset(theUrlChar, 0, m_MapPath.length() + 250);
						if(m_MapType == ArcgisServerMap)
							sprintf(theUrlChar, m_TileUrlString_.c_str(), m_MapPath.c_str(), js, iRows, iCols);
						else if(m_MapType == DegreeServermap)
							sprintf(theUrlChar, m_TileUrlString_.c_str(), m_MapPath.c_str(), m_MapName.c_str(), m_TileMatrixSetname_.c_str(), js, iRows, iCols, m_PicType.c_str());
						theUrl[iUrlCount] = theUrlChar;
						//������
						m_MapTileTempStore_[xCount].theKey = thekey;
						m_MapTileTempStore_[xCount].MapTileTemp = "";
						//��ʼ������
						CurlArray[iUrlCount] = m_CurlConnect_->curl_easy_handler(theUrl[iUrlCount], m_MapTileTempStore_[xCount].MapTileTemp, timeOut);
						curl_multi_add_handle(curl_m, CurlArray[iUrlCount]);
						iUrlCount++;
						xCount++;
					}//��Ҫ��������
					Tile* theTile = new Tile();
					//���ο��
					theTile->m_TileWidth_ = m_TileCols_;
					theTile->m_TileHeight_ = m_TileRows_;
					//��ͼ�ľ��μ���
					theTile->SetTheVerts(iCols - topleftCols, m_TileCols_, leftxTopDiffToCtrl, 
						iRows - topleftRows, m_TileRows_, leftyTopDiffToCtrl);
					m_TileStore_[thekey] = theTile;
				}
			}
			m_CurlConnect_->WateForMulitCurl(curl_m);
			// ����Ҫע��cleanup��˳�� //
			for (int idx = 0; idx < iUrlCount; ++idx)
			{
				curl_multi_remove_handle(curl_m, CurlArray[idx]);
			}
			for (int idx = 0; idx < iUrlCount; ++idx)
			{
				curl_easy_cleanup(CurlArray[idx]);
			}
			curl_multi_cleanup(curl_m);
			delete[] theUrlChar;
			theTime = clock();
			if(xCount > m_NumberOfCpuCount * 4 + 2)		//���߳�
			{
				int maxHandleNumber = m_NumberOfCpuCount * 2;
				HANDLE* handle = new HANDLE[maxHandleNumber];
				long NumberOfSize = xCount / maxHandleNumber;
				ContRange* theRange = new ContRange[maxHandleNumber];
				for (int i = 0; i < maxHandleNumber - 1; i++)  
				{
					theRange[i].theBeignNumber = i * NumberOfSize;
					theRange[i].theEndNumber = theRange[i].theBeignNumber + NumberOfSize;
					theRange[i].theMapLayer = this;
					handle[i] = (HANDLE)_beginthreadex(NULL, 0, DecodeForServerFun, &theRange[i], 0, NULL); 
				}
				theRange[maxHandleNumber - 1].theBeignNumber = (maxHandleNumber - 1) * NumberOfSize;
				theRange[maxHandleNumber - 1].theEndNumber = xCount;
				theRange[maxHandleNumber - 1].theMapLayer = this;
				handle[maxHandleNumber - 1] = (HANDLE)_beginthreadex(NULL, 0, DecodeForServerFun, &theRange[maxHandleNumber - 1], 0, NULL);
				for (int i = 0; i < maxHandleNumber; i++) 
				{
					WaitForSingleObject(handle[i], 1000);
					CloseHandle(handle[i]);
				}
				delete[] handle;
				delete[] theRange;
			}
			else
				DoSingleDecode(0, xCount);	
			theTime = clock() - theTime;
			theTime = clock();
		}
		//����
		{
			std::map<unsigned __int64, Tile*>::iterator theFindTile;
			std::map<unsigned __int64, Tile*>::iterator theEndTile = m_TileStore_.end();
			unsigned __int64 thekey;
			for (int iRows = topleftRows; iRows < bottomrightRows; iRows++)		//�еı���
			{
				for (int iCols = topleftCols; iCols < bottomrightCols; iCols++)	//�еı���
				{
					//��λͼƬ
					thekey = GetTheTileKey(js, iRows, iCols);
					theFindTile = m_TileStore_.find(thekey);
					if(theFindTile != theEndTile && theFindTile->second->m_bDecodeSuccess)
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
			std::map<unsigned __int64, Tile*>::iterator theBegTile = m_TileStore_.begin();
			std::map<unsigned __int64, Tile*>::iterator theEndTile = m_TileStore_.end();
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
void CServerMapLayer::DoSingleDecode(const long& theBeignNumber, const long& theEndNumber)
{
	std::map<unsigned __int64, Tile*>::iterator theEndTile = m_TileStore_.end();
	std::map<unsigned __int64, Tile*>::iterator theFindTile;
	for(int i = theBeignNumber; i < theEndNumber && i < MAX_MAP_TEXTURE_TEMP; i++)
	{
		theFindTile = m_TileStore_.find(m_MapTileTempStore_[i].theKey);
		if(theFindTile != theEndTile && !theFindTile->second->m_bDecodeSuccess)
			theFindTile->second->m_bDecodeSuccess = theFindTile->second->DeCodeTheTile(m_MapTileTempStore_[i].MapTileTemp);	//����
	}
}
void CServerMapLayer::GetMapConfig(const std::string& ConfigPath)
{
	std::string theText = "";
	if(!m_ReadConfig_)
	{
		//���ݷ���
		switch(m_MapType)
		{
		case MapType::ArcgisServerMap:
			m_CurlConnect_->GetDataFromByHttp(ConfigPath, theText);			//����������Ҫ����·��
			AnalyseArcServerJson(theText);
			break;
		case MapType::DegreeServermap:
			m_CurlConnect_->GetDataFromByHttp(ConfigPath, theText);
			AnalyseDegreeMapServerConfig(theText);
			break;
		case MapType::PGISMap:								//�������ݲ���Ҫ��������
			AnalysePGISConfig();
			break;
		default:
			break;
		}
	}
}
/*
"capabilities":"Map,Query,Data",
"copyrightText":"",
"currentVersion":10.1,

"description":"",
"documentInfo":
"Title":"hangzhou",
"Author":"jishunyu",
"Comments":"",
"Subject":"",
"Category":"",
"AntialiasingMode":"None",
"TextAntialiasingMode":"Force",
"Keywords":""

"fullExtent":
"xmin":120.06387173893624,
"ymin":30.19013924910362,
"xmax":120.22948675546374,
"ymax":30.385115528049432,

"initialExtent":
"spatialReference":
"wkid":4326,
"latestWkid":4326}},
"xmin":120.02735331037829,
"ymin":30.240940258441434,
"xmax":120.35833633628928,
"ymax":30.375855697472012,

"layers":
0:
"id":0,
"name":
"HangzhouCity_300_A1.jpg",
"parentLayerId":-1,
"defaultVisibility":true,
"subLayerIds":null,
"minScale":0,
"maxScale":0

"mapName":"Layers",
"serviceDescription":"",
"maxRecordCount":1000,
"maxImageHeight":2048,
"maxImageWidth":2048
"minScale":100000,
"maxScale":1000,

"supportsDynamicLayers":false,

"tables":[],

"spatialReference":
"wkid":4326,
"latestWkid":4326,
"singleFusedMapCache":true,
"supportedImageFormatTypes":"PNG32,PNG24,PNG,JPG,DIB,TIFF,EMF,PS,PDF,GIF,SVG,SVGZ,BMP",
"supportedQueryFormats":"JSON, AMF",

"tileInfo":
"rows":256,
"cols":256,
"dpi":96,
"format":"JPEG",
"compressionQuality":75,
"origin":
"x":120.063872,
"y":30.385116,
"spatialReference":
"wkid":4326,
"latestWkid":4326,
"lods":
{"level":0,"resolution":2.3794610058302802E-4,"scale":100000},
{"level":1,"resolution":1.1897305029151401E-4,"scale":50000},
{"level":2,"resolution":5.9486525145757005E-5,"scale":25000},
{"level":3,"resolution":2.3794610058302804E-5,"scale":10000},
{"level":4,"resolution":1.1897305029151402E-5,"scale":5000},
{"level":5,"resolution":4.75892201166056E-6,"scale":2000},
{"level":6,"resolution":2.37946100583028E-6,"scale":1000}]},

"spatialReference":
"wkid":4326,
"latestWkid":4326,
"units":"esriDecimalDegrees",	
*/
void CServerMapLayer::AnalyseArcServerJson(const std::string& theText)
{
	Json::Reader reader;  
	Json::Value root;  
	if (reader.parse(theText, root))  // reader��Json�ַ���������root��root������Json��������Ԫ��   
	{  
		try
		{
			m_MapDataStoreTemp_.clear();
			m_MapTileEffectiveArea_.clear();
			//��ȡ��Ƭ��С dpi ͼƬ����
			m_TileRows_ = root["tileInfo"]["rows"].asInt();
			m_TileCols_ = root["tileInfo"]["cols"].asInt();
			m_DPI_ = root["tileInfo"]["dpi"].asInt();
			m_PicType = root["tileInfo"]["format"].asString();

			//ÿһ�����Ϣ
			Json::Value lods = root["tileInfo"]["lods"];
			int levelCount = lods.size();
			for (int ilods = 0; ilods < levelCount; ilods++)
			{
				MapDataStore theLevelInfo;
				theLevelInfo.Level = lods[ilods]["level"].asInt();
				theLevelInfo.resolution = lods[ilods]["resolution"].asDouble();
				theLevelInfo.scale = lods[ilods]["scale"].asDouble();
				if(m_MaxLevel < theLevelInfo.Level)
					m_MaxLevel = theLevelInfo.Level;
				m_MapDataStoreTemp_[theLevelInfo.Level] = theLevelInfo;
			}

			//���Ͻ�����
			Json::Value origin = root["tileInfo"]["origin"];
			m_TileOriginX_ = origin["x"].asDouble();
			m_TileOriginY_ = origin["y"].asDouble();

			//double Xmin = ;


		}
		catch(...)
		{

		}
	} 
}

void CServerMapLayer::AnalysePGISConfig()
{
	m_ReadConfig_ = true;
	m_TileOriginX_ = 0;
	m_TileOriginY_ = 0;

	m_MaxLevel = 22;

	MapDataStore theLevelInfo;
	theLevelInfo.Level = 0;
	theLevelInfo.resolution = 2;
	theLevelInfo.scale = 786432000;

	m_MapDataStoreTemp_[0] = theLevelInfo;
	m_TileRows_ = 256;
	m_TileCols_ = 256;

	for(int i = 1; i <= m_MaxLevel; i++)
	{
		theLevelInfo.Level = i;
		theLevelInfo.resolution = m_MapDataStoreTemp_[i - 1].resolution / 2.0;
		theLevelInfo.scale = m_MapDataStoreTemp_[i - 1].scale / 2.0;
		m_MapDataStoreTemp_[i] = theLevelInfo;
	}
}

void CServerMapLayer::AnalyseDegreeMapServerConfig(const std::string& theText)
{
	//http://10.1.50.123:8020/services/wmts?service=WMTS&request=GetCapabilities
	if(theText == "")
		return;
	TiXmlDocument XmlFile;
	XmlFile.Parse(theText.c_str());
	TiXmlElement* theRootElement = XmlFile.RootElement();
	TiXmlElement* theContextElement = theRootElement->FirstChildElement("Contents");
	if(theContextElement)
	{
		TiXmlElement* theLayerElement = theContextElement->FirstChildElement("Layer");
		while(theLayerElement)
		{
			TiXmlNode* theLayerNameNode = theLayerElement->FirstChildElement()->FirstChild();
			if(theLayerNameNode && (theLayerNameNode->Value() == m_MapName))			//������ȷ
			{
				std::string theFormat = theLayerElement->FirstChildElement("Format")->FirstChild()->Value();
				m_PicType = theFormat.substr(theFormat.find("/") + 1, theFormat.length() - 1);
				TiXmlElement* TileMatrixSetLinkElement = theLayerElement->FirstChildElement("TileMatrixSetLink");
				m_TileMatrixSetname_ = TileMatrixSetLinkElement->FirstChildElement("TileMatrixSet")->FirstChild()->Value();
				break;
			}
			theLayerElement = theLayerElement->NextSiblingElement("Layer");
		}
		if(m_TileMatrixSetname_ != "")
		{
			TiXmlElement* TileMatrixSetElement = theLayerElement->NextSiblingElement("TileMatrixSet");
			while(TileMatrixSetElement)
			{
				TiXmlNode* theTileMatrixName = TileMatrixSetElement->FirstChildElement()->FirstChild();
				if(theTileMatrixName && (theTileMatrixName->Value()) == m_TileMatrixSetname_)
				{
					TiXmlElement* theTileMatrixElement = TileMatrixSetElement->FirstChildElement("TileMatrix");
					MapDataStore theLevelInfo;
					MapTileEffectArea theMapEffectiveArea;
					while(theTileMatrixElement)
					{
						try
						{
							theLevelInfo.Level = atoi(theTileMatrixElement->FirstChildElement()->FirstChild()->Value());
							theLevelInfo.scale = atof(theTileMatrixElement->FirstChildElement("ScaleDenominator")->FirstChild()->Value());
							theLevelInfo.resolution = atof(theTileMatrixElement->FirstChildElement("Resolution")->FirstChild()->Value());
							std::string theTopLeft = theTileMatrixElement->FirstChildElement("TopLeftCorner")->FirstChild()->Value();
							int ipos = theTopLeft.find(" ");
							theMapEffectiveArea.m_TopLeftLong = atof(theTopLeft.substr(0, ipos).c_str());
							theMapEffectiveArea.m_TopLeftLat = atof(theTopLeft.substr(ipos + 1, theTopLeft.length() - 1).c_str());
							theMapEffectiveArea.m_rowCount = atoi(theTileMatrixElement->FirstChildElement("MatrixHeight")->FirstChild()->Value());
							theMapEffectiveArea.m_colCount = atoi(theTileMatrixElement->FirstChildElement("MatrixWidth")->FirstChild()->Value());
							theMapEffectiveArea.m_TileHeight = atoi(theTileMatrixElement->FirstChildElement("TileHeight")->FirstChild()->Value());
							theMapEffectiveArea.m_TileWidth = atoi(theTileMatrixElement->FirstChildElement("TileWidth")->FirstChild()->Value());
							if(m_MaxLevel < theLevelInfo.Level)
								m_MaxLevel = theLevelInfo.Level;
						}
						catch(...)
						{

						}
						m_MapDataStoreTemp_[theLevelInfo.Level] = theLevelInfo;
						m_MapTileEffectiveArea_[theLevelInfo.Level] = theMapEffectiveArea;
						theTileMatrixElement = theTileMatrixElement->NextSiblingElement("TileMatrix");
					}
					m_ReadConfig_ = true;
					break;
				}
				TileMatrixSetElement = TileMatrixSetElement->NextSiblingElement("TileMatrixSet");
			}
		}
	}
}