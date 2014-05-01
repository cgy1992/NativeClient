#pragma once
#include "stdafxcpp.h"
#include "MarkerElement.h"
class WFSPointFeatureElement
	: public CMarkerElement
{
public:
	double	x_;				//×ø±ê
	double	y_;
	std::map<std::string, std::string> m_PontFeature;	//ÊôÐÔ
};

class CWFSPointFeatureElementSet
{
private:
	long m_FeatureHandle_;
public:
	CWFSPointFeatureElementSet();
	~CWFSPointFeatureElementSet();

	//delete all feature
	void RemoveAllFeature();

	//add one feature
	long AddOneFeature(WFSPointFeatureElement* pFeature);

	std::map<long, WFSPointFeatureElement*> m_DataSet_;

	void RenderManyByOpenGl(const double& x, const double& y, 
		const long& ctrlWidth, const long& ctrlHeight, 
		const double& resolution,
		CMarkerStyle* theStyle);
};