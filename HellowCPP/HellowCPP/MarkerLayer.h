#pragma once
#include "stdafxppapi.h"
#include "Layer.h"
#include "WFSPointFeature.h"
class CMarkerLayer
	: public CLayer
{
public:
	CMarkerLayer()
		: m_LayerName_("")
	{

	}
	virtual ~CMarkerLayer()
	{

	}
	// layer name
	void put_LayerName(const std::string& theName) { m_LayerName_ = theName; }
	std::string get_LayerName() { return m_LayerName_; }

	//search
	virtual void SearchByRect(std::vector<double>&theRect, CWFSPointFeatureElementSet& theSet)
	{

	}

	//
	virtual void SearchByFid(const long& fid, CWFSPointFeatureElementSet& theSet)
	{

	}

	virtual void Draw()	{}
private:
	std::string m_LayerName_;
};