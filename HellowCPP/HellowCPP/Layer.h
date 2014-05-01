#pragma once
#include "stdafxppapi.h"
#include "DataStruct.h"
class CLayer
{
public:
	CLayer(void)
		: m_CenLon(0)
		, m_CenLat(0)
		, m_CtrlHeight(0)
		, m_CtrlWidth(0)
		, m_Resolution(0)
		, m_bVisible(true)
	{

	}
	CLayer(const double& xcen, const double& ycen
		, const long& height, const long& width)
		: m_CenLon(xcen)
		, m_CenLat(ycen)
		, m_CtrlHeight(height)
		, m_CtrlWidth(width)
	{

	}
	virtual ~CLayer(void) {}
	virtual void Draw() = 0;
	//经纬度转换为像素
	void ProjectionToPix(const double& longtitude, const double& latitude, int& x, int& y)
	{
		x = 0;
		y = 0;
		Projection_to_Pixel(m_CenLon, m_CenLat, m_CtrlWidth / 2, m_CtrlHeight / 2, longtitude, latitude, x, y, m_Resolution);
	}
public:
	VLayerType m_layerType;
	bool m_bVisible;
	//每次重绘均需要重新赋值 // just do change from PIX TO PROJECTION
	double m_CenLon;						//底图的中心点经度坐标
	double m_CenLat;						//纬度
	long m_CtrlHeight;						//空间窗口
	long m_CtrlWidth;
	double m_Resolution;					//单位像素的经纬度
};

