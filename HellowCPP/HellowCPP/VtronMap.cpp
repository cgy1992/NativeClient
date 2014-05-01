#include "VtronMap.h"
long CVtronMap::AddLayer(CLayer* layerObject)
{ 
	if (layerObject == NULL)
	{
		return -1;
	}
	long theLayerHandle = 0;
	switch(layerObject->m_layerType)
	{
	//��ͼ�����0
	case VLayerType::vBaseLayer:
		if(m_BaseLayer_)
			delete m_BaseLayer_;
		m_BaseLayer_ = (CBaseLayer*)layerObject;
		break;
	//other layers' handle from 1 to MAX
	default:
		{
			m_LayerMap_.insert(std::map<long, CLayer*>::value_type(m_LayerHandle_, layerObject));
			theLayerHandle = m_LayerHandle_;
			m_LayerHandle_++;
		}
		break;
	}
	return theLayerHandle;
}
bool CVtronMap::RemoveAllLayer()				//����ɾ����ͼ
{
	std::map<long, CLayer*>::iterator theBeg = m_LayerMap_.begin();
	std::map<long, CLayer*>::iterator theEnd = m_LayerMap_.end();
	for (; theBeg != theEnd; theBeg++)
	{
		CLayer* theLayer = theBeg->second;
		if (theLayer != NULL)
		{
			delete theLayer;
			theLayer = NULL;
		}
	}
	m_LayerMap_.clear();
	return true;
}
bool CVtronMap::RemoveLayerByHandle(long layerHandle)
{
	std::map<long, CLayer*>::iterator theFind = m_LayerMap_.find(layerHandle);
	if(theFind != m_LayerMap_.end())
	{
		if (theFind->second)
		{
			delete theFind->second;
			theFind->second = NULL;
		}
		m_LayerMap_.erase(theFind);
		return true;
	}
	return false;
}

long CVtronMap::AddStyle(CStyle* styleObject)
{
	if(styleObject)
	{
		//��ʽ��1��ʼ���
		++m_StyleHandle_;
		m_StyleMap_[m_StyleHandle_] = styleObject;
		return m_StyleHandle_;
	}
	return -1;
}

bool CVtronMap::RemoveStyleByHandle(long StyleHandle)
{
	std::map<long, CStyle*>::iterator theFind = m_StyleMap_.find(StyleHandle);
	if(theFind != m_StyleMap_.end())
	{
		if (theFind->second)
		{
			delete theFind->second;
			theFind->second = NULL;
		}
		m_StyleMap_.erase(theFind);
		return true;
	}
	return false;
}

bool CVtronMap::RemoveAllStyle()
{
	std::map<long, CStyle*>::iterator theBeg = m_StyleMap_.begin();
	std::map<long, CStyle*>::iterator theEnd = m_StyleMap_.end();
	for (; theBeg != theEnd; theBeg++)
	{
		CStyle* theStyle = theBeg->second;
		if (theStyle != NULL)
		{
			delete theStyle;
			theStyle = NULL;
		}
	}
	m_StyleMap_.clear();
	return true;
}

CLayer* CVtronMap::GetLayerByHandle(long layerHandle)
{
	std::map<long, CLayer*>::iterator theFind = m_LayerMap_.find(layerHandle);
	if(theFind != m_LayerMap_.end())
		return theFind->second;
	else
		return NULL;
}

CStyle* CVtronMap::GetStyleByHandle(long StyleHandle)
{
	std::map<long, CStyle*>::iterator theFind = m_StyleMap_.find(StyleHandle);
	if(theFind != m_StyleMap_.end())
		return theFind->second;
	else
		return NULL;
}