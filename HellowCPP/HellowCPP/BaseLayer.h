#pragma  once
#include "Layer.h"
//地图的比例级别和比例尺的配套参数
struct MapDataStore
{
	int Level;				///级别
	double scale;			//比例尺
	double resolution;		//一个像素对应于对少经纬度度
};

//local map thread decode and draw
//子线程函数  
unsigned int __stdcall DecodeAndDrawForLocalFun(PVOID pM)  
{
	return 0;  
}  

unsigned __int64 GetTheTileKey(const int& js, const int& row, const int& col)
{
	unsigned __int64 theKey = js;
	theKey = theKey << 28;
	theKey += row;
	theKey = theKey << 29;
	theKey += col;
	return theKey;
}

class CBaseLayer
	: public CLayer
{
public:
	CBaseLayer()
		: CLayer()
		, m_js(0)
		, m_Scale(0)
		, m_MaxLevel(-1)
		//切片的大小
		, m_TileCols_(0)
		, m_TileRows_(0)
	{
		this->m_layerType = VLayerType::vBaseLayer;
		m_NumberOfCpuCount = GetCpuCoreCount();
	}
	CBaseLayer(const double& cenX, const double& cenY, const int& js, 
		const std::string& mapname, const std::string& path, const std::string& pictype
		, const long& ViewHeight, const long& ViewWidth)
		: CLayer(cenX, cenY, ViewHeight, ViewWidth)
		, m_js(js)
		, m_Scale(0)
		, m_MaxLevel(-1)
		//切片的大小
		, m_TileCols_(0)
		, m_TileRows_(0)
	{
		m_PicType = pictype;

		m_MapPath = path;
		m_MapName = mapname;
		this->m_layerType = VLayerType::vBaseLayer;

		m_NumberOfCpuCount = GetCpuCoreCount();
	}
	virtual void Draw() {}
	virtual ~CBaseLayer() {}
	//中心点的配置和获取
	void SetMapCenter(double x, double y, double z, int js);
	void GetMapCenter(double& x, double& y, double& z, double& js);
	//获取配置文件
	virtual void GetBaseLayerResolution(int js, double& resolution);

	int m_js;				//当前比例级别

	std::string m_PicType;			//地图切片类型
	//切片的大小
	int m_TileCols_;
	int m_TileRows_;

	std::string m_MapPath;			//地图路径
	std::string m_MapName;			//地图名字

	double m_Scale;					//当前比例尺

	int m_MaxLevel;					//最大的地图级别
	MapType m_MapType;				//地图类型

	std::map<long, MapDataStore> m_MapDataStoreTemp_;					//地图配置数据的临时存放点

	int m_NumberOfCpuCount;			//CPU的个数
};