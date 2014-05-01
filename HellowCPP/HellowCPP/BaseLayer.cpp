#include "BaseLayer.h"
void CBaseLayer::SetMapCenter(double x, double y, double z, int js)
{
	m_CenLon = x;
	m_CenLat = y;
	//m_CentZ = z;
	m_js = js;
}

void CBaseLayer::GetMapCenter(double& x, double& y, double& z, double& js)
{
	x = m_CenLon;
	y = m_CenLat;
	js = m_js;
}

void CBaseLayer::GetBaseLayerResolution(int js, double& resolution)
{
	resolution = 1.0;
	std::map<long, MapDataStore>::iterator theFind = m_MapDataStoreTemp_.find(js);
	if(theFind != m_MapDataStoreTemp_.end())
	{
		resolution = theFind->second.resolution;
	}
}