#pragma once
#include "CurlConnect.h"
#include "WFSPointFeature.h"
#include "MarkerLayer.h"
class CEditLayer
	: public CMarkerLayer	
{
public:
	CEditLayer()
		: m_ElementHandle_(-1)
	{
		m_MarkDataSet_.clear();
	}
	~CEditLayer()
	{
		std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleBegin = m_MarkDataSet_.begin();
		std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleEnd = m_MarkDataSet_.end();
		while(theStyleBegin != theStyleEnd)
		{
			delete theStyleBegin->second;
			theStyleBegin++;
		}
	}
	void Draw();

	// pic element set
	std::map<CMarkerStyle*, CWFSPointFeatureElementSet*> m_MarkDataSet_;
	// add a marker element by element handle
	long AddMarkerElement(CMarkerStyle* styleHandle, const double& x, const double& y, 
		const int& rendW, const int& rendH, 
		std::map<std::string, std::string>& theFieldValue);

	// delete an marker element by element handle
	void DeleteMarkerElement(const long& elementHandle);
	// edit an marker element by element handle
	void EditMarkerElement(const long& elementHandle, std::map<std::string, std::string>& theFieldValue);
	void EditMarkerElement(const long& elementHandle, const double& x, const double& y, std::map<std::string, std::string>& theFieldValue);
	// get the element information in rect
	void SearchByRect(std::vector<double>& theVector, CWFSPointFeatureElementSet& returnElementSet);
	// get the element information by fid
	void SearchByFid(const long& fid, CWFSPointFeatureElementSet& returnElementSet);

private:
	long m_ElementHandle_;
	WFSPointFeatureElement* FindWFSPointFeatureElement(const long& elementHandle);
	void SetWFSFeature(WFSPointFeatureElement* theElement, std::map<std::string, std::string>& theFieldValue);
};