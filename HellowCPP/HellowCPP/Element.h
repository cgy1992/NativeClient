#pragma once
#include "DataStruct.h"
class CElement
{
public:
	CElement(void);
	virtual ~CElement(void);
	VGeometryType m_Geometry;					//类型
	virtual void RenderUseOgles() = 0;

	double m_CenLon;						//底图的中心点经度坐标
	double m_CenLat;						//纬度
	long m_CtrlHeight;						//空间窗口
	long m_CtrlWidth;
	double m_Resolution;					//单位像素的经纬度

	bool m_bVisiable_;
	//经纬度转换为像素
	void ProjectionToPix(const double& longtitude, const double& latitude, int& x, int& y)
	{
		x = 0;
		y = 0;
		Projection_to_Pixel(m_CenLon, m_CenLat, m_CtrlWidth / 2, m_CtrlHeight / 2, longtitude, latitude, x, y, m_Resolution);
	}
};

