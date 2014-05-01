#pragma once 
#include "BaseLayer.h"
#include "Tile.h"
#include "XmlHelp.h"
#include "CurlConnect.h"

class CServerMapLayer
	: public CBaseLayer
{
public:
	CServerMapLayer()
	{
		m_bExistConfig_ = false;
		m_ReadConfig_ = false;
		m_Grapics3d_ = NULL;
		m_CurlConnect_ = CurlConnect::GetInstance();
	}
	CServerMapLayer(const double& cenX, const double& cenY, const int& js, 
		const std::string& mapname, const std::string& path, const std::string& pictype,
		const long& ViewWidth, const long& ViewHeight, const MapType& theType)
		: CBaseLayer(cenX, cenY, js, mapname, path, pictype, ViewHeight, ViewWidth)
		, m_TileMatrixSetname_("")
	{
		m_bExistConfig_ = false;
		m_MapType = theType;
		m_ReadConfig_ = false;
		m_Grapics3d_ = NULL;
		switch(m_MapType)
		{
		case MapType::ArcgisServerMap:
			m_TileUrlString_ = "%s/tile/%d/%d/%d";
			break;
		case MapType::PGISMap:
			m_TileUrlString_ = "%s/EzMap?Service=getImage&Type=RGB&Col=%d&Row=%d&Zoom=%d&V=0.3";
			break;
		case MapType::DegreeServermap:
			m_TileUrlString_ = "%s?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=%s&STYLE=default&TILEMATRIXSET=%s&TILEMATRIX=%d&TILEROW=%d&TILECOL=%d&FORMAT=image/%s";
			break;
		}
		m_CurlConnect_ = CurlConnect::GetInstance();
	}
	~CServerMapLayer()
	{
		//ɾ����Ƭ����
		std::map<unsigned __int64, Tile*>::iterator theBeg = m_TileStore_.begin();
		std::map<unsigned __int64, Tile*>::iterator theEnd = m_TileStore_.end();
		while (theBeg != theEnd)
		{
			delete theBeg->second;
			theBeg++;
		}
		m_TileStore_.clear();
	}
	//���Ƶ�ͼ
	void Draw();
	//��ȡ��ͼ����
	void GetMapData(const int& js, const double& midX, const double& midY		//��ǰ�����߼������ĵ�
		, const int& windowWidth, const int& windowHeight);
	//��ȡ��ͼ������Ϣ
	void GetMapConfig(const std::string& ConfigPath);
	//��ȡ��Ƭ������  �����߼��� ���ĵ�ľ�γ��
	void GetMapTile(const int& js, const double& midX, const double& midY		//��ǰ�����߼������ĵ�
		, const int& windowWidth, const int& windowHeight);	//���ڵĿ��
	void GetMapTilePGIS(const int& js, const double& midX, const double& midY		//��ǰ�����߼������ĵ�
		, const int& windowWidth, const int& windowHeight);	//���ڵĿ��
	
	//����ArcServer�����ļ�
	void AnalyseArcServerJson(const std::string& theText);

	//��������PGIS��������Ϣ
	void AnalysePGISConfig();

	//
	void AnalyseDegreeMapServerConfig(const std::string& theText);

	pp::Graphics3D* m_Grapics3d_;
	
private:
	bool m_bExistConfig_;
	//���Ͻ�X Y
	//PGIS 0�� 0
	double m_TileOriginX_;
	double m_TileOriginY_;

	//DPI ÿӢ���������
	int m_DPI_;
	bool m_ReadConfig_;			//�Ƿ��Ѿ���ȡ�������ļ�

	//ͼƬ��Ƭ�Ĵ洢λ��  
	//����  ��Ƭ
	std::map<unsigned __int64, Tile*> m_TileStore_;

	//ͼƬ����ʱ�洢����
	struct MapTileStoreTemp
	{
		unsigned __int64 theKey;			//�ؼ���
		std::string MapTileTemp;//��������
	};

	//ͼƬ��ʱ�洢����
	MapTileStoreTemp m_MapTileTempStore_[MAX_MAP_TEXTURE_TEMP];

	//��ЧͼƬ����
	struct MapTileEffectArea
	{
		double m_TopLeftLong;
		double m_TopLeftLat;
		long m_rowCount;		//ͼƬ����
		long m_colCount;
		long m_TileWidth;
		long m_TileHeight;
	};

	//��ͼ��Ч����
	std::map<long, MapTileEffectArea> m_MapTileEffectiveArea_;

	//deegreemap
	std::string m_TileMatrixSetname_;

	struct ContRange
	{
		long theBeignNumber;
		long theEndNumber;
		CServerMapLayer* theMapLayer;
	};
	void DoSingleDecode(const long& theBeignNumber, const long& theEndNumber);			//���߳̽���
	//���̺߳���  
	//Server map thread decode 
	static unsigned int __stdcall DecodeForServerFun(PVOID pM)
	{
		ContRange* theRange = (ContRange*)pM;
		long theBeignNumber = theRange->theBeignNumber;
		long theEndNumber = theRange->theEndNumber;
		CServerMapLayer* theMapLayer = theRange->theMapLayer;
		std::map<unsigned __int64, Tile*>::iterator theEndTile = theMapLayer->m_TileStore_.end();
		std::map<unsigned __int64, Tile*>::iterator theFindTile;
		for(int i = theBeignNumber; i < theEndNumber && i < MAX_MAP_TEXTURE_TEMP; i++)
		{
			theFindTile = theMapLayer->m_TileStore_.find(theRange->theMapLayer->m_MapTileTempStore_[i].theKey);
			if(theFindTile != theEndTile && !theFindTile->second->m_bDecodeSuccess)
			{
				theFindTile->second->m_bDecodeSuccess = theFindTile->second->DeCodeTheTile(theRange->theMapLayer->m_MapTileTempStore_[i].MapTileTemp);	//����
				/*EnterCriticalSection(&g_EnterDrawSection);
				theFindTile->second->FitTheTextureID(false, theMapLayer->m_Grapics3d_);
				LeaveCriticalSection(&g_EnterDrawSection);*/
			}
		}
		return 0;
	}

	//ͼƬ�ַ�����ʽ
	std::string m_TileUrlString_;

	//http connect
	CurlConnect* m_CurlConnect_;
};