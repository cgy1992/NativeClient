#include "EditLayer.h"
void CEditLayer::Draw()
{
	std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleBegin = m_MarkDataSet_.begin();
	std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleEnd = m_MarkDataSet_.end();
	CMarkerStyle* theStyle = NULL;
	while(theStyleBegin != theStyleEnd)
	{
		CWFSPointFeatureElementSet* theSet = theStyleBegin->second;
		theStyle = theStyleBegin->first;
		if (theStyle)
			theSet->RenderManyByOpenGl(m_CenLon, m_CenLat, m_CtrlWidth, m_CtrlHeight, m_Resolution, theStyle);
		theStyleBegin++;
	}
}

// add a marker element by element handle
long CEditLayer::AddMarkerElement(CMarkerStyle* styleHandle, const double& x, const double& y, 
	const int& rendW, const int& rendH, 
	std::map<std::string, std::string>& theFieldValue)
{
	WFSPointFeatureElement* theCurrentElement = new WFSPointFeatureElement();
	theCurrentElement->x_ = x;
	theCurrentElement->y_ = y;
	theCurrentElement->m_rendWidth = rendW;
	theCurrentElement->m_rendHeight = rendH;
	SetWFSFeature(theCurrentElement, theFieldValue);

	std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theFind = m_MarkDataSet_.find(styleHandle);
	if(theFind != m_MarkDataSet_.end())
	{
		m_ElementHandle_++;
		theFind->second->m_DataSet_[m_ElementHandle_] = theCurrentElement;
	}
	else
	{
		CWFSPointFeatureElementSet* theSet = new CWFSPointFeatureElementSet();
		m_ElementHandle_++;
		theSet->m_DataSet_[m_ElementHandle_] = theCurrentElement;
		m_MarkDataSet_[styleHandle] = theSet;
	}
	return m_ElementHandle_;
}
// delete an marker element by element handle
void CEditLayer::DeleteMarkerElement(const long& elementHandle)
{
	std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleBegin = m_MarkDataSet_.begin();
	std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleEnd = m_MarkDataSet_.end();
	while(theStyleBegin != theStyleEnd)
	{
		std::map<long, WFSPointFeatureElement*>::iterator theDataSetBeg = theStyleBegin->second->m_DataSet_.begin();
		std::map<long, WFSPointFeatureElement*>::iterator theDataSetEnd = theStyleBegin->second->m_DataSet_.end();
		while (theDataSetBeg != theDataSetEnd)
		{
			if(theDataSetBeg->first == elementHandle)
			{	
				delete theDataSetBeg->second;
				theStyleBegin->second->m_DataSet_.erase(theDataSetBeg);
				return;
			}
			theDataSetBeg++;
		}
		theStyleBegin++;
	}
}
void CEditLayer::SetWFSFeature(WFSPointFeatureElement* theElement, std::map<std::string, std::string>& theFieldValue)
{
	std::map<std::string, std::string>::iterator theBeginValue = theFieldValue.begin();
	std::map<std::string, std::string>::iterator theEndValue = theFieldValue.end();
	while(theBeginValue != theEndValue)
	{
		theElement->m_PontFeature[theBeginValue->first] = theBeginValue->second; 
		theBeginValue++;
	}
}

// edit an marker element by element handle
void CEditLayer::EditMarkerElement(const long& elementHandle, std::map<std::string, std::string>& theFieldValue)
{
	WFSPointFeatureElement* theCurrentElement = FindWFSPointFeatureElement(elementHandle);
	if(theCurrentElement)
		SetWFSFeature(theCurrentElement, theFieldValue);
}

WFSPointFeatureElement* CEditLayer::FindWFSPointFeatureElement(const long& elementHandle)
{
	std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleBegin = m_MarkDataSet_.begin();
	std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleEnd = m_MarkDataSet_.end();
	while(theStyleBegin != theStyleEnd)
	{
		std::map<long, WFSPointFeatureElement*>::iterator theDataSetBeg = theStyleBegin->second->m_DataSet_.begin();
		std::map<long, WFSPointFeatureElement*>::iterator theDataSetEnd = theStyleBegin->second->m_DataSet_.end();
		while (theDataSetBeg != theDataSetEnd)
		{
			if(theDataSetBeg->first == elementHandle)
				return theDataSetBeg->second;
			theDataSetBeg++;
		}
		theStyleBegin++;
	}
	return NULL;
}
void CEditLayer::EditMarkerElement(const long& elementHandle, const double& x, const double& y, std::map<std::string, std::string>& theFieldValue)
{
	WFSPointFeatureElement* theCurrentElement = FindWFSPointFeatureElement(elementHandle);
	if(theCurrentElement)
	{
		theCurrentElement->x_ = x;
		theCurrentElement->y_ = y;
		SetWFSFeature(theCurrentElement, theFieldValue);
	}
}
// get the element information in rect
void CEditLayer::SearchByRect(std::vector<double>& theVector, CWFSPointFeatureElementSet& returnElementSet)
{
	if(theVector.size() == 2)
	{
		int x = 0;
		int y = 0;			// click location
		std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleBegin = m_MarkDataSet_.begin();
		std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleEnd = m_MarkDataSet_.end();
		Projection_to_Pixel(m_CenLon, m_CenLat, m_CenLon, m_CenLat, theVector[0], theVector[1], x, y, m_Resolution);
		while(theStyleBegin != theStyleEnd)
		{
			std::map<long, WFSPointFeatureElement*>::iterator theDataSetBeg = theStyleBegin->second->m_DataSet_.begin();
			std::map<long, WFSPointFeatureElement*>::iterator theDataSetEnd = theStyleBegin->second->m_DataSet_.end();
			int xrender = 0;
			int yrender = 0;
			while (theDataSetBeg != theDataSetEnd)
			{
				WFSPointFeatureElement* theCurrentElement = theDataSetBeg->second;
				long widthdif = theCurrentElement->m_rendWidth / 2;
				long heightdif = theCurrentElement->m_rendHeight / 2;
				Projection_to_Pixel(m_CenLon, m_CenLat, m_CenLon, m_CenLat, theCurrentElement->x_, theCurrentElement->y_, xrender, yrender, m_Resolution);
				if(theCurrentElement->m_bVisiable_ && x > (xrender - widthdif) && x < (xrender + widthdif) && y > (yrender - heightdif) && y < (heightdif + yrender))
				{
					/*外面CWFSPointFeatureElementSet析构释放*/
					WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
					// memcpy(newCurrentElement, theCurrentElement, sizeof(WFSPointFeatureElement));
					newCurrentElement->x_ = theCurrentElement->x_;
					newCurrentElement->y_ = theCurrentElement->y_;
					newCurrentElement->m_PontFeature.clear();
					newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());
					returnElementSet.m_DataSet_[theDataSetBeg->first] = newCurrentElement;
				}
				theDataSetBeg++;
			}
			theStyleBegin++;
		}
	}
	else if(theVector.size() == 4)
	{
		int xbegin;
		int ybegin;
		int xend;
		int yend;
		std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleBegin = m_MarkDataSet_.begin();
		std::map<CMarkerStyle*, CWFSPointFeatureElementSet*>::iterator theStyleEnd = m_MarkDataSet_.end();
		Projection_to_Pixel(m_CenLon, m_CenLat, m_CenLon, m_CenLat, theVector[0], theVector[1], xbegin, ybegin, m_Resolution);
		Projection_to_Pixel(m_CenLon, m_CenLat, m_CenLon, m_CenLat, theVector[2], theVector[3], xend, yend, m_Resolution);
		while(theStyleBegin != theStyleEnd)
		{
			std::map<long, WFSPointFeatureElement*>::iterator theDataSetBeg = theStyleBegin->second->m_DataSet_.begin();
			std::map<long, WFSPointFeatureElement*>::iterator theDataSetEnd = theStyleBegin->second->m_DataSet_.end();
			int xrender = 0;
			int yrender = 0;
			while (theDataSetBeg != theDataSetEnd)
			{
				WFSPointFeatureElement* theCurrentElement = theDataSetBeg->second;
				Projection_to_Pixel(m_CenLon, m_CenLat, m_CenLon, m_CenLat, theCurrentElement->x_, theCurrentElement->y_, xrender, yrender, m_Resolution);
				if(theCurrentElement->m_bVisiable_ && xrender > xbegin && xrender < xend && yrender > ybegin && yrender < yend)
				{
					/*外面CWFSPointFeatureElementSet析构释放*/
					WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
					//memcpy(newCurrentElement, theCurrentElement, sizeof(WFSPointFeatureElement));
					newCurrentElement->x_ = theCurrentElement->x_;
					newCurrentElement->y_ = theCurrentElement->y_;
					newCurrentElement->m_PontFeature.clear();
					newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());
					returnElementSet.m_DataSet_[theDataSetBeg->first] = newCurrentElement;
				}
				theDataSetBeg++;
			}
			theStyleBegin++;
		}
	}
}

void CEditLayer::SearchByFid(const long& fid, CWFSPointFeatureElementSet& returnElementSet)
{
	WFSPointFeatureElement* theCurrentElement = FindWFSPointFeatureElement(fid);
	returnElementSet.m_DataSet_.clear();
	if(theCurrentElement)
	{
		/*外面CWFSPointFeatureElementSet析构释放*/
		WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
		// 不然复制了地址，会在外面析构的
		// memcpy(newCurrentElement, theCurrentElement, sizeof(WFSPointFeatureElement));
		newCurrentElement->m_PontFeature.clear();
		std::map<std::string, std::string>::iterator theFeatureBeg = theCurrentElement->m_PontFeature.begin();
		std::map<std::string, std::string>::iterator theFeatureEnd = theCurrentElement->m_PontFeature.end();
		while(theFeatureBeg != theFeatureEnd)
		{
			newCurrentElement->m_PontFeature[theFeatureBeg->first] = theFeatureBeg->second;
			theFeatureBeg++;
		}
		returnElementSet.m_DataSet_[fid] = newCurrentElement;
	}
}