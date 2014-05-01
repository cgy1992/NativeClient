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
	double m_PicScale_;											//ͼƬ���ű�	
	
public:
	DrawEngine();
	~DrawEngine();
	bool InitColorShader();		//��ʼ����ɫ����ɫ��
	bool InitTextureShader();	//��ʼ��������ɫ��
	void Draw();				//����
	void SetRect(const long& width, const long& height);

public:
	long m_CtrlWidth;				//�ؼ��Ŀ��
	long m_CtrlHeight;
	CVtronMap m_VtronMap;				//��ͼ����
	enumMapOperator m_MapOperator;		//the map operator
	pp::Graphics3D* m_Grapics3d_;

	//�ƶ���ͼ
	void MapMoveView(int pixcent, int piycent, int js);

	//���Ʋ���
	void DrawRect(const MapEngineES_Point& pointBeg, const MapEngineES_Point& pointMove);


private:
	//��ʼ����ɫ�� ����������
	GLuint InitTheShader(const GLenum& type, const char* theCode);
	bool InitTheProgram(const char* VShaderData, const char* FShaderData,
						  GLuint& vershader, GLuint& frashader, GLuint& textProgram);



public:
	//��������ת��Ϊ��γ������
	bool PixToProjection(const int& x, const int& y, double& longtitude, double& latitude);
	//��γ��ת��Ϊ��������
	bool ProjectionToPix(const double& longtitude, const double& latitude, int& x, int& y);
	//��������Ԫ��
	void CreateTheFreeDrawLineElement();
	//������ɻ��Ƶĵ�
	void AddTheFreeDrawLinePoint(const int& x, const int& y);
	//current draw lineelement handle
	CLineElement* m_FreeDrawElementHandle_;

	//�������
	void CommandParsing(const std::string& CmdName, const Json::Value& root, Json::Value& returnInfo);
private:
	typedef void (DrawEngine::*HandleFun)(const Json::Value& root, Json::Value& returnInfo);				//����ָ��
	std::map<std::string, HandleFun> m_Function;

	void OpenMap(const Json::Value& root, Json::Value& returnInfo);				// �򿪵�ͼ
	void RegistStyle(const Json::Value& root, Json::Value& returnInfo);			// ע��ͼ����ʽ
	void LoadTheShapefile(const Json::Value& root, Json::Value& returnInfo);	// ���shapefile�ļ�
	void LoadTheWFSLayer(const Json::Value& root, Json::Value& returnInfo);		// WFSLayer

	void ChangeMapView(const Json::Value& root, Json::Value& returnInfo);		// ��ͼ�ƶ�
	void ChangeOperator(const Json::Value& root, Json::Value& returnInfo);		// �ı������ʽ
	void HideLayer(const Json::Value& root, Json::Value& returnInfo);			// HIDELAYER
	void ShowLayer(const Json::Value& root, Json::Value& returnInfo);			// show
	void DeleteLayer(const Json::Value& root, Json::Value& returnInfo);			// Delete


	//�������ص��ַ���
	Json::Value ConstructJsonValue(const std::string& CmdName, const int& CmdID, const int& elementID, 
									const int& layerid, const int& status, const int& styleid, const int& mapLevel);

	bool m_bRefreshMap_;														// need refresh map or not;

	long ztranslate;
	long xtranslate;
	long ytranslate;   
	double xrotate;
};