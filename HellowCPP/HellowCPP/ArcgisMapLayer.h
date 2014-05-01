#pragma once 
#include "BaseLayer.h"
#include "Tile.h"
#include "XmlHelp.h"
class CArcgisMapLayer
	: public CBaseLayer
{
public:
	CArcgisMapLayer()
	{
	}
	CArcgisMapLayer(const double& cenX, const double& cenY, const int& js, 
		const std::string& mapname, const std::string& path, const std::string& pictype,
		int ViewHeight, int ViewWidth)
		: CBaseLayer(cenX, cenY, js, mapname, path, pictype, ViewHeight, ViewWidth)
	{
		m_MapType = MapType::ArcgisMap;
		m_ReadConfig_ = false;
	}
	~CArcgisMapLayer()
	{
		//删除切片数据
		std::map<long, Tile*>::iterator theBeg = m_TileStore_.begin();
		std::map<long, Tile*>::iterator theEnd = m_TileStore_.end();
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
	void GetMapConfig(std::string ConfigPath);
	//获取切片的数据  比例尺级别 中心点的经纬度
	void GetMapTile(const int& js, const double& midX, const double& midY		//当前比例尺级别中心点
					, const int& windowWidth, const int& windowHeight);	//窗口的宽高

	

private:
	//左上角X Y
	double m_TileOriginX_;
	double m_TileOriginY_;
	
	//DPI 每英寸的像素数
	int m_DPI_;
	bool m_ReadConfig_;			//是否已经读取过配置文件

	//图片切片的存储位置  
	//索引  切片
	std::map<long, Tile*> m_TileStore_;
};