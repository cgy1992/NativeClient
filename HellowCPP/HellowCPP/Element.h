#pragma once
#include "DataStruct.h"
class CElement
{
public:
	CElement(void);
	virtual ~CElement(void);
	VGeometryType m_Geometry;					//����
	virtual void RenderUseOgles() = 0;

	double m_CenLon;						//��ͼ�����ĵ㾭������
	double m_CenLat;						//γ��
	long m_CtrlHeight;						//�ռ䴰��
	long m_CtrlWidth;
	double m_Resolution;					//��λ���صľ�γ��

	bool m_bVisiable_;
	//��γ��ת��Ϊ����
	void ProjectionToPix(const double& longtitude, const double& latitude, int& x, int& y)
	{
		x = 0;
		y = 0;
		Projection_to_Pixel(m_CenLon, m_CenLat, m_CtrlWidth / 2, m_CtrlHeight / 2, longtitude, latitude, x, y, m_Resolution);
	}
};

