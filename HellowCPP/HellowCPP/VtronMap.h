// VtronMap.h : PPAPI插件的地图管理类
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


	//底图以外的其他层
	std::map<long, CLayer*> m_LayerMap_;
private:
	long m_LayerHandle_;

	//图片样式
	std::map<long, CStyle*> m_StyleMap_;
	long m_StyleHandle_;

public:
	//添加图层
	long AddLayer(CLayer *layerObject);

	//删除所有非底图层
	bool RemoveAllLayer();

	//根据句柄删除图层
	bool RemoveLayerByHandle(long layerHandle);

	//根据句柄找到层
	CLayer* GetLayerByHandle(long layerHandle);

	//添加样式
	long AddStyle(CStyle* styleObject); 

	//删除所有样式
	bool RemoveAllStyle();

	//根据句柄删除样式
	bool RemoveStyleByHandle(long StyleHandle);

	//根据句柄找到样式
	CStyle* GetStyleByHandle(long StyleHandle);

	//底图
	CBaseLayer* m_BaseLayer_;

	//用来自由绘制的层
	CElementLayer* m_FreeDrawLayer_;

public:
	int m_LineWidth_;			//线条宽
	int m_LineColorR_;			//线条RGBA
	int m_LineColorG_;
	int m_LineColorB_;
	int m_LineColorA_;
};
