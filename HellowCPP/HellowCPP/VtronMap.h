// VtronMap.h : PPAPI����ĵ�ͼ������
#pragma once
#include "BaseLayer.h"
#include "Style.h"
#include "ElementLayer.h"
class  CVtronMap
{
public:
	CVtronMap() 
		: m_LayerHandle_(1)
		, m_BaseLayer_(NULL)
		, m_StyleHandle_(0)
		, m_FreeDrawLayer_(NULL)
	{
		m_LayerMap_.clear();
		m_StyleMap_.clear();
	}
	~CVtronMap()
	{
		RemoveAllLayer();
		RemoveAllStyle();
		if(m_BaseLayer_)
			delete m_BaseLayer_;
	}


	//��ͼ�����������
	std::map<long, CLayer*> m_LayerMap_;
private:
	long m_LayerHandle_;

	//ͼƬ��ʽ
	std::map<long, CStyle*> m_StyleMap_;
	long m_StyleHandle_;

public:
	//���ͼ��
	long AddLayer(CLayer *layerObject);

	//ɾ�����зǵ�ͼ��
	bool RemoveAllLayer();

	//���ݾ��ɾ��ͼ��
	bool RemoveLayerByHandle(long layerHandle);

	//���ݾ���ҵ���
	CLayer* GetLayerByHandle(long layerHandle);

	//�����ʽ
	long AddStyle(CStyle* styleObject); 

	//ɾ��������ʽ
	bool RemoveAllStyle();

	//���ݾ��ɾ����ʽ
	bool RemoveStyleByHandle(long StyleHandle);

	//���ݾ���ҵ���ʽ
	CStyle* GetStyleByHandle(long StyleHandle);

	//��ͼ
	CBaseLayer* m_BaseLayer_;

	//�������ɻ��ƵĲ�
	CElementLayer* m_FreeDrawLayer_;

public:
	int m_LineWidth_;			//������
	int m_LineColorR_;			//����RGBA
	int m_LineColorG_;
	int m_LineColorB_;
	int m_LineColorA_;
};
