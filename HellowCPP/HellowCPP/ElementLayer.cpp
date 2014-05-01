#include "ElementLayer.h"

CElementLayer::CElementLayer(void)
{
	m_layerType = vElementLayer;
	m_ElementHandle_ = 1;
}


CElementLayer::~CElementLayer(void)
{
	DeleteAllElement();
}

void CElementLayer::DeleteAllElement()
{
	std::map<long, CElement*>::iterator theBeg = m_ElementMap.begin();
	std::map<long, CElement*>::iterator theEnd = m_ElementMap.end();
	while(theBeg != theEnd)
	{
		CElement* theElement = theBeg->second;
		if (theElement != NULL)
		{
			delete theElement;
			theElement = NULL;
		}
		theBeg++;
	}
	m_ElementMap.clear();
}

long CElementLayer::AddElement(CElement* elementObject)
{
	if(elementObject == NULL)
		return -1;
	m_ElementMap.insert(std::map<long, CElement*>::value_type(m_ElementHandle_, elementObject));
	return m_ElementHandle_++;
}

CElement* CElementLayer::GetElement(long index)
{
	std::map<long, CElement*>::iterator theFind = m_ElementMap.find(index);
	if(theFind != m_ElementMap.end())
		return theFind->second;
	return NULL;
}
void CElementLayer::Draw()
{
	std::map<long, CElement*>::iterator theBeg = m_ElementMap.begin();
	std::map<long, CElement*>::iterator theEnd = m_ElementMap.end();
	for(; theBeg != theEnd; theBeg++)
	{
		CElement* theElement = theBeg->second;
		if (theElement != NULL)
		{
			theElement->m_CenLat = m_CenLat;
			theElement->m_CenLon = m_CenLon;
			theElement->m_CtrlHeight = m_CtrlHeight;
			theElement->m_CtrlWidth = m_CtrlWidth;
			theElement->m_Resolution = m_Resolution;
			theElement->RenderUseOgles();
		}
	}
}