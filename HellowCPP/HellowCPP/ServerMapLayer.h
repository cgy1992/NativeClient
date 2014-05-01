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
		//删除切片数据
		std::map<unsigned __int64, Tile*>::iterator theBeg = m_TileStore_.begin();
		std::map<unsigned __int64, Tile*>::iterator theEnd = m_TileStore_.end();
		while (theBeg != theEnd)
		{
			delete theBeg->second;
			theBeg++;
		}
		m_TileStore_.clear();
	}
	//绘制地图
	void Draw();
	//获取地图数据
	void GetMapData(const int& js, const double& midX, const double& midY		//当前比例尺级别中心点
		, const int& windowWidth, const int& windowHeight);
	//获取地图配置信息
	void GetMapConfig(const std::string& ConfigPath);
	//获取切片的数据  比例尺级别 中心点的经纬度
	void GetMapTile(const int& js, const double& midX, const double& midY		//当前比例尺级别中心点
		, const int& windowWidth, const int& windowHeight);	//窗口的宽高
	void GetMapTilePGIS(const int& js, const double& midX, const double& midY		//当前比例尺级别中心点
		, const int& windowWidth, const int& windowHeight);	//窗口的宽高
	
	//解析ArcServer配置文件
	void AnalyseArcServerJson(const std::string& theText);

	//解析设置PGIS的配置信息
	void AnalysePGISConfig();

	//
	void AnalyseDegreeMapServerConfig(const std::string& theText);

	pp::Graphics3D* m_Grapics3d_;
	
private:
	bool m_bExistConfig_;
	//左上角X Y
	//PGIS 0， 0
	double m_TileOriginX_;
	double m_TileOriginY_;

	//DPI 每英寸的像素数
	int m_DPI_;
	bool m_ReadConfig_;			//是否已经读取过配置文件

	//图片切片的存储位置  
	//索引  切片
	std::map<unsigned __int64, Tile*> m_TileStore_;

	//图片的临时存储区域
	struct MapTileStoreTemp
	{
		unsigned __int64 theKey;			//关键码
		std::string MapTileTemp;//数据内容
	};

	//图片临时存储区域
	MapTileStoreTemp m_MapTileTempStore_[MAX_MAP_TEXTURE_TEMP];

	//有效图片区域
	struct MapTileEffectArea
	{
		double m_TopLeftLong;
		double m_TopLeftLat;
		long m_rowCount;		//图片张数
		long m_colCount;
		long m_TileWidth;
		long m_TileHeight;
	};

	//地图有效区域
	std::map<long, MapTileEffectArea> m_MapTileEffectiveArea_;

	//deegreemap
	std::string m_TileMatrixSetname_;

	struct ContRange
	{
		long theBeignNumber;
		long theEndNumber;
		CServerMapLayer* theMapLayer;
	};
	void DoSingleDecode(const long& theBeignNumber, const long& theEndNumber);			//单线程解码
	//子线程函数  
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
				theFindTile->second->m_bDecodeSuccess = theFindTile->second->DeCodeTheTile(theRange->theMapLayer->m_MapTileTempStore_[i].MapTileTemp);	//解码
				/*EnterCriticalSection(&g_EnterDrawSection);
				theFindTile->second->FitTheTextureID(false, theMapLayer->m_Grapics3d_);
				LeaveCriticalSection(&g_EnterDrawSection);*/
			}
		}
		return 0;
	}

	//图片字符串格式
	std::string m_TileUrlString_;

	//http connect
	CurlConnect* m_CurlConnect_;
};