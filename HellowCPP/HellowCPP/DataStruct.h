#pragma once
#include "stdafxcpp.h"
#include "stdafxwin32.h"
//��������
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

//��ͼ������ʽ
enum enumMapOperator
{
	vPanOperator = 0,		//���������ƽ�ƣ�����
	vChooseOperator = 1,	//ѡ�����
	vDrawOperator = 2,		//���ʲ���
	vPolygonChoose = 3		//��ѡ
};

const long MAP_JS_SCALE = 1000000;
const long MAP_ROW_SCALE = 1000;

//�༭ʹ�õ����ݽṹ
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
		long valueid_;					//�༭������Ժ�
		std::string value_;				//�༭�������ֵ
	};
	struct EditValue_
	{
		long lPointId_;
		std::vector<currentEditValue_> currentEditStore_;				//��ǰ�༭�������Դ�ŵ�
	};
	struct ValueField_							//���ӵĵ������ֵ
	{
		long lPointId_;							//��ı��
		std::vector<std::string> sFieldValue_;	//�õ��Ӧ������ֵ
	};

public:
	std::vector<PointRecorder_> m_PointRecorderVector_;		//��Ƶ��������Ϣ�洢�����ʱ�༭״̬�洢��

	std::list<ValueField_> PointValueField_;			//�����ӵĵ����Ե���ʱ�洢λ�ã�����㣩
	ValueField_ myValue_;								//�������������ʱ��ţ������㣩


	long m_CurrentPointId;								//��ǰ�༭�ĵ�ı��
	EditValue_ currVal_;								//��ǰ�༭�������ֵ��ʱ�洢�㣨�����㣩
	std::list<EditValue_> EditPointStore_;				//�༭���ĵ��δ����ǰ�Ĵ�ŵ�ַ������㣩

	int ChoseLayerNum_;									//��Ƶ��༭��ʱ��ѡ�е�ͼ���
	std::vector<long> ChosePointNum_;					//��Ƶ��༭��ʱ��ѡ�еĵ���
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
//��γ������ת��Ϊ��������
//���ĵ�ľ�γ�ȣ���������
//�ض���ľ�γ�ȣ���������
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
//��������ת��Ϊ��γ��
//���ĵ�ľ�γ�ȣ���������
//�ض���ľ�γ�ȣ���������
void Pixel_to_Projection(const double& xlongtitude, const double& ylatitude, const int& centx, const int& centy,
	double& thelongtitude, double& thelatitude, const int& thepix, const int& thepiy, const double& resolution)
{
	thelongtitude = xlongtitude + (thepix - centx) * resolution;
	thelatitude = ylatitude + (centy - thepiy) * resolution;
}

//��־�ļ�
std::ofstream g_out_file_log;
//����ʱ�ɽ��ܷ�Χ
const long MAX_TIME_COST = 50;
//����cs
//CRITICAL_SECTION g_EnterDrawSection;
