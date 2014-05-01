#pragma once
#include "ShaderData.h"
#include "VtronMap.h"
#include "matrix.h"
#include "MarkerElement.h"
#include "ArcgisMapLayer.h"
#include "ShapefileLayer.h"
#include "ServerMapLayer.h"
#include "LineElement.h"
#include "WFSLayer.h"
class DrawEngine
{
private:
	ULONG_PTR m_gdiplusToken;									// GDI+
	double m_PicScale_;											//图片缩放比	
	
public:
	DrawEngine();
	~DrawEngine();
	bool InitColorShader();		//初始化颜色的着色器
	bool InitTextureShader();	//初始化纹理着色器
	void Draw();				//绘制
	void SetRect(const long& width, const long& height);

public:
	long m_CtrlWidth;				//控件的宽高
	long m_CtrlHeight;
	CVtronMap m_VtronMap;				//地图管理
	enumMapOperator m_MapOperator;		//the map operator
	pp::Graphics3D* m_Grapics3d_;

	//移动地图
	void MapMoveView(int pixcent, int piycent, int js);

	//绘制测试
	void DrawRect(const MapEngineES_Point& pointBeg, const MapEngineES_Point& pointMove);


private:
	//初始化着色器 创建、编译
	GLuint InitTheShader(const GLenum& type, const char* theCode);
	bool InitTheProgram(const char* VShaderData, const char* FShaderData,
						  GLuint& vershader, GLuint& frashader, GLuint& textProgram);



public:
	//像素坐标转换为经纬度坐标
	bool PixToProjection(const int& x, const int& y, double& longtitude, double& latitude);
	//经纬度转换为像素坐标
	bool ProjectionToPix(const double& longtitude, const double& latitude, int& x, int& y);
	//创建线条元素
	void CreateTheFreeDrawLineElement();
	//添加自由绘制的点
	void AddTheFreeDrawLinePoint(const int& x, const int& y);
	//current draw lineelement handle
	CLineElement* m_FreeDrawElementHandle_;

	//命令解析
	void CommandParsing(const std::string& CmdName, const Json::Value& root, Json::Value& returnInfo);
private:
	typedef void (DrawEngine::*HandleFun)(const Json::Value& root, Json::Value& returnInfo);				//函数指针
	std::map<std::string, HandleFun> m_Function;

	void OpenMap(const Json::Value& root, Json::Value& returnInfo);				// 打开地图
	void RegistStyle(const Json::Value& root, Json::Value& returnInfo);			// 注册图标样式
	void LoadTheShapefile(const Json::Value& root, Json::Value& returnInfo);	// 添加shapefile文件
	void LoadTheWFSLayer(const Json::Value& root, Json::Value& returnInfo);		// WFSLayer

	void ChangeMapView(const Json::Value& root, Json::Value& returnInfo);		// 地图移动
	void ChangeOperator(const Json::Value& root, Json::Value& returnInfo);		// 改变操作方式
	void HideLayer(const Json::Value& root, Json::Value& returnInfo);			// HIDELAYER
	void ShowLayer(const Json::Value& root, Json::Value& returnInfo);			// show
	void DeleteLayer(const Json::Value& root, Json::Value& returnInfo);			// Delete


	//构建返回的字符串
	Json::Value ConstructJsonValue(const std::string& CmdName, const int& CmdID, const int& elementID, 
									const int& layerid, const int& status, const int& styleid, const int& mapLevel);

	bool m_bRefreshMap_;														// need refresh map or not;

	long ztranslate;
	long xtranslate;
	long ytranslate;   
	double xrotate;
};