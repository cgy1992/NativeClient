#pragma once
#include "stdafxcpp.h"
#include "stdafxwin32.h"
//几何类型
enum VGeometryType
{	
	vUnknown	= 0,
	vUPoint		= 1,
	vULine		= 2,
	vUPolygon	= 3,
	vUMultiPoint	= 4,
	vUMultiLine		= 5,
	vUMultiPolygon	= 6,
	vUGeometryCollection	= 7,
	vUPointMark	=	8
};

//type of layer
enum VLayerType
{
	vBaseLayer = 0,
	vHotLayer = 1,
	vDynamicLayer = 2,
	vElementLayer = 3,
	vWFSLayer = 4,
	vEditLayer = 5
};
//style type
enum enumStyleType
{	
	vPointstyle	= 0,
	vLinestyle	= 1,
	vPolygonstyle	= 2,
	vTextstyle	= 3
};

//
enum MapType
{
	VtronMapTile = 0,
	VtronMapCompact = 1,
	ArcgisMap = 2,
	ArcgisServerMap = 3,
	DegreeServermap = 4,
	ArcgisCompactMap = 5,
	PGISMap	=	6
};
//
struct MapEngineES_Point
{	
	double x;
	double y;
	int fid;
	int GisID;
};

//地图操作方式
enum enumMapOperator
{
	vPanOperator = 0,		//浏览操作，平移，缩放
	vChooseOperator = 1,	//选择操作
	vDrawOperator = 2,		//画笔操作
	vPolygonChoose = 3		//面选
};

const long MAP_JS_SCALE = 1000000;
const long MAP_ROW_SCALE = 1000;

//编辑使用的数据结构
class PointInfoStore
{
public:
	struct PointRecorder_
	{
		long	iFid_;
		bool	bChanged_;
		bool	bDelete_;
		bool	bAdd_;
		bool	bEdit_;
		double	pointx_;
		double	pointy_;	
	};
	struct currentEditValue_
	{
		long valueid_;					//编辑点的属性号
		std::string value_;				//编辑点的属性值
	};
	struct EditValue_
	{
		long lPointId_;
		std::vector<currentEditValue_> currentEditStore_;				//当前编辑过的属性存放点
	};
	struct ValueField_							//增加的点的属性值
	{
		long lPointId_;							//点的编号
		std::vector<std::string> sFieldValue_;	//该点对应的属性值
	};

public:
	std::vector<PointRecorder_> m_PointRecorderVector_;		//视频点坐标信息存储点和临时编辑状态存储点

	std::list<ValueField_> PointValueField_;			//新增加的点属性的临时存储位置（多个点）
	ValueField_ myValue_;								//新增点的属性临时存放（单个点）


	long m_CurrentPointId;								//当前编辑的点的编号
	EditValue_ currVal_;								//当前编辑点的属性值临时存储点（单个点）
	std::list<EditValue_> EditPointStore_;				//编辑过的点的未保存前的存放地址（多个点）

	int ChoseLayerNum_;									//视频点编辑的时候选中的图层号
	std::vector<long> ChosePointNum_;					//视频点编辑的时候选中的点编号
public:
	PointInfoStore() 
	{
		ResetInfo();
	}
	~PointInfoStore() 
	{
		ResetInfo();
	}
private:
	void ResetInfo()
	{
		m_PointRecorderVector_.clear();
		PointValueField_.clear();
		myValue_.sFieldValue_.clear();
		m_CurrentPointId = -1;
		currVal_.currentEditStore_.clear();
		EditPointStore_.clear();
		ChoseLayerNum_ = -1;
		ChosePointNum_.clear();
	}

};
//经纬度坐标转换为像素坐标
//中心点的经纬度，像素坐标
//特定点的经纬度，像素坐标
void Projection_to_Pixel(const double& xlongtitude, const double& ylatitude, const int& centx, const int& centy,
	const double& thelongtitude, const double& thelatitude, int& thepix, int& thepiy, const double& resolution)
{
	try
	{
		int x = (thelongtitude - xlongtitude) / resolution + centx;
		int y = centy - (thelatitude - ylatitude) / resolution;
		thepix = x;
		thepiy = y;
	}
	catch (...)
	{
	}
}
//像素坐标转换为经纬度
//中心点的经纬度，像素坐标
//特定点的经纬度，像素坐标
void Pixel_to_Projection(const double& xlongtitude, const double& ylatitude, const int& centx, const int& centy,
	double& thelongtitude, double& thelatitude, const int& thepix, const int& thepiy, const double& resolution)
{
	thelongtitude = xlongtitude + (thepix - centx) * resolution;
	thelatitude = ylatitude + (centy - thepiy) * resolution;
}

//日志文件
std::ofstream g_out_file_log;
//最大耗时可接受范围
const long MAX_TIME_COST = 50;
//绘制cs
//CRITICAL_SECTION g_EnterDrawSection;
