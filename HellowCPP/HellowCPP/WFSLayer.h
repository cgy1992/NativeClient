#pragma once
#include "stdafxppapi.h"
#include "MarkerElement.h"
#include "MarkerLayer.h"
#include "CurlConnect.h"
/*
http://10.1.50.131:8080/services/wfs?service=WFS&request=GetFeature&version=1.1.0&TypeName=topp:hzvideo&featureid=HZVIDEO_3
http://10.1.50.131:8080/services/wfs?service=WFS&request=GetFeature&version=1.1.0&TypeName=topp:hzvideo
http://10.1.50.131:8080/services/wfs?service=WFS&request=GetFeature&version=1.1.0&TypeName=topp:hzvideo&BBOX=120.200000,30.3,120.080000,30.500000,EPSG:4326
 */
class CWFSLayer
	: public CMarkerLayer
{
private:
	CWFSPointFeatureElementSet m_PointLayer_;
	CMarkerStyle* m_Style_;
	std::string m_UrlPath_;
	bool m_bGetData_;

	int m_Height_;
	int m_Width_;

	CurlConnect* m_CurlConnect_;

public:
	CWFSLayer()
		: CMarkerLayer()
		, m_Style_(NULL)
		, m_UrlPath_("")
		, m_bGetData_(false)
		, m_Height_(0)
		, m_Width_(0)
		, m_CurlConnect_(NULL)
	{
		m_CurlConnect_ = CurlConnect::GetInstance();
		m_layerType = vHotLayer;
	}

	~CWFSLayer()
	{
	}
	// layer url
	void put_UrlPath(const std::string& theUrl) { m_UrlPath_ = theUrl; }
	
	// style
	void put_Style(CMarkerStyle* theStyle) { m_Style_ = theStyle; }

	// width height
	void put_Rect(const int& width, const int& height) { m_Width_ = width; m_Height_ = height; }

	// get data by http
	void GetData();

	// load file
	void LoadDataByString(const std::string& theText, CWFSPointFeatureElementSet& theSet);

	// draw
	void Draw();

	//search
	void SearchByRect(std::vector<double>&theRect, CWFSPointFeatureElementSet& theSet);

	//
	void SearchByFid(const long& fid, CWFSPointFeatureElementSet& theSet);
};