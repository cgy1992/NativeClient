#pragma  once
#include "Layer.h"
//��ͼ�ı�������ͱ����ߵ����ײ���
struct MapDataStore
{
	int Level;				///����
	double scale;			//������
	double resolution;		//һ�����ض�Ӧ�ڶ��پ�γ�ȶ�
};

//local map thread decode and draw
//���̺߳���  
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
		//��Ƭ�Ĵ�С
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
		//��Ƭ�Ĵ�С
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
	//���ĵ�����úͻ�ȡ
	void SetMapCenter(double x, double y, double z, int js);
	void GetMapCenter(double& x, double& y, double& z, double& js);
	//��ȡ�����ļ�
	virtual void GetBaseLayerResolution(int js, double& resolution);

	int m_js;				//��ǰ��������

	std::string m_PicType;			//��ͼ��Ƭ����
	//��Ƭ�Ĵ�С
	int m_TileCols_;
	int m_TileRows_;

	std::string m_MapPath;			//��ͼ·��
	std::string m_MapName;			//��ͼ����

	double m_Scale;					//��ǰ������

	int m_MaxLevel;					//���ĵ�ͼ����
	MapType m_MapType;				//��ͼ����

	std::map<long, MapDataStore> m_MapDataStoreTemp_;					//��ͼ�������ݵ���ʱ��ŵ�

	int m_NumberOfCpuCount;			//CPU�ĸ���
};