#include "WFSLayer.h"
#include "XmlHelp.h"
void CWFSLayer::GetData()
{
	if(m_UrlPath_ != "")
	{
		// get xml
		std::string theUrl = m_UrlPath_;
		std::string theText = "";
		m_CurlConnect_->GetDataFromByHttp(theUrl, theText);

		// remove all points
		m_PointLayer_.RemoveAllFeature();

		LoadDataByString(theText, m_PointLayer_);
	}
}

void CWFSLayer::LoadDataByString(const std::string& theText, CWFSPointFeatureElementSet& theSet)
{
	//load the file
	TiXmlDocument pDoc;     
	pDoc.Parse(theText.c_str());
	TiXmlElement* pRootElet = pDoc.RootElement();

	WFSPointFeatureElement* theCurrentFeature = NULL;
	for(TiXmlElement * i = pRootElet->FirstChildElement(); i != NULL; i = i->NextSiblingElement())
	{
		//gml::featureMember
		TiXmlElement* pFeature = i->FirstChildElement();
		for (; pFeature != NULL; pFeature = pFeature->NextSiblingElement())
		{
			//app:hzvideo
			std::string theLayerName = pFeature->Value();
			if(theLayerName.compare("app:" + get_LayerName()) == 0)
			{
				theCurrentFeature = new WFSPointFeatureElement();
				// id
				std::string featureid = pFeature->Attribute("gml:id");
				int ifeapos = featureid.find('_');
				featureid = featureid.substr(ifeapos + 1, featureid.length());
				long theFid = atoi(featureid.c_str());

				//field
				TiXmlElement* pfield = pFeature->FirstChildElement();
				while(pfield)
				{
					std::string theFieldName = pfield->Value();
					if(theFieldName.compare("app:geometry") == 0)
					{
						TiXmlElement* pGeo = pfield->FirstChildElement();
						if(pGeo)
						{
							std::string geoStr = pGeo->Value();
							if (geoStr.compare("gml:Point") == 0)
							{
								TiXmlElement* theElement = pGeo->FirstChildElement();
								if(theElement)
								{
									TiXmlNode* theNode = theElement->FirstChild();
									if(theNode)
									{
										std::string thePos = theNode->Value();
										int ipos = thePos.find(" ");
										std::string y = thePos.substr(0, ipos);
										std::string x = thePos.substr(ipos + 1, thePos.length());
										try
										{
											if(m_Width_ <= 0)
												m_Width_ = m_Style_->m_ImageWidth_;
											if(m_Height_ <= 0)
												m_Height_ = m_Style_->m_ImageHeight_;

											int showWidth = m_Width_;
											int showHeight = m_Height_;

											theCurrentFeature->x_ = atof(x.c_str());
											theCurrentFeature->y_ = atof(y.c_str());
											theCurrentFeature->m_rendWidth = showWidth;
											theCurrentFeature->m_rendHeight = showHeight;
										}
										catch (CMemoryException* e)
										{

										}
										catch (CFileException* e)
										{
										}
										catch (CException* e)
										{
										}
									}
								}
							}
						}
					}
					else
					{
						std::string theValueString = "";
						TiXmlNode* theNode = pfield->FirstChild();
						if(theNode)
						{
							theValueString = theNode->Value();
							CString theValueCString;
							StringToCString(theValueString, theValueCString, CP_UTF8);
							CStringToString(theValueCString, theValueString, CP_ACP);
						}
						theCurrentFeature->m_PontFeature[theFieldName] = theValueString;
					}
					pfield = pfield->NextSiblingElement();
				}
				theSet.m_DataSet_[theFid] = theCurrentFeature;
			}
		}
	}
}
/*
	1	3
	0	2
 */
void CWFSLayer::Draw()
{
	m_PointLayer_.RenderManyByOpenGl(m_CenLon, m_CenLat, m_CtrlWidth, m_CtrlHeight, m_Resolution, m_Style_);
}


void CWFSLayer::SearchByFid(const long& fid, CWFSPointFeatureElementSet& theSet)
{
	char bufferRect[512] = {0};
	char* layerName = new char[get_LayerName().length() + 1];
	sprintf(layerName, "%s", get_LayerName().c_str());
	sprintf(bufferRect, "&&featureid=%s_%d", strupr(layerName), fid);
	delete[] layerName;
	std::string theUrl = m_UrlPath_ + bufferRect;
	std::string theText = "";
	m_CurlConnect_->GetDataFromByHttp(theUrl, theText);
	theSet.RemoveAllFeature();
	LoadDataByString(theText, theSet);
}

void CWFSLayer::SearchByRect(std::vector<double>&theRect, CWFSPointFeatureElementSet& theSet)
{
	if(theRect.size() == 4)
	{
		double xbegin = theRect[0];
		double ybegin = theRect[1];
		double xend = theRect[2];
		double yend = theRect[3];
		char bufferRect[512] = {0};
		sprintf(bufferRect, "&BBOX=%f,%f,%f,%f,EPSG:4326", xbegin, ybegin, xend, yend);
		std::string theUrl = m_UrlPath_ + bufferRect;
		std::string theText = "";
		m_CurlConnect_->GetDataFromByHttp(theUrl, theText);
		theSet.RemoveAllFeature();
		LoadDataByString(theText, theSet);
	}
}
