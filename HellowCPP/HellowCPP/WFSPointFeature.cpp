#include "WFSPointFeature.h"
#include "stdafxppapi.h"

CWFSPointFeatureElementSet::CWFSPointFeatureElementSet()
	: m_FeatureHandle_(-1)
{
	m_DataSet_.clear();
}

CWFSPointFeatureElementSet::~CWFSPointFeatureElementSet()
{
	RemoveAllFeature();
}
void CWFSPointFeatureElementSet::RemoveAllFeature()
{
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = m_DataSet_.end();
	while(theBegin != theEnd)
	{
		delete theBegin->second;
		theBegin++;
	}
	m_DataSet_.clear();
	m_FeatureHandle_ = -1;
}

long CWFSPointFeatureElementSet::AddOneFeature(WFSPointFeatureElement* pFeature)
{
	m_FeatureHandle_++;
	m_DataSet_[m_FeatureHandle_] = pFeature;
	return m_FeatureHandle_;
}

void CWFSPointFeatureElementSet::RenderManyByOpenGl(const double& x, const double& y, 
	const long& ctrlWidth, const long& ctrlHeight, 
	const double& resolution,
	CMarkerStyle* theStyle)
{
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = m_DataSet_.end();
	Vertex verts[6];
	std::vector<Vertex> theVertexVector;
	while (theBegin != theEnd)
	{
		WFSPointFeatureElement* theCurrentFeature = theBegin->second;
		theCurrentFeature->m_CenLon = x;
		theCurrentFeature->m_CenLat = y;
		theCurrentFeature->m_CtrlHeight = ctrlHeight;
		theCurrentFeature->m_CtrlWidth = ctrlWidth;
		theCurrentFeature->m_Resolution = resolution;

		int pix = 0;
		int piy = 0;
		theCurrentFeature->ProjectionToPix(theCurrentFeature->x_, theCurrentFeature->y_, pix, piy);
		if(pix > 0 && pix < ctrlWidth && piy < ctrlHeight && piy > 0)
		{
			//0
			verts[0].tu = 0;
			verts[0].tv = 0;
			verts[0].loc[0] = pix - theCurrentFeature->m_rendWidth / 2;
			verts[0].loc[1] = piy - theCurrentFeature->m_rendHeight / 2;
			verts[0].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[0]);
			//1
			verts[1].tu = 0;
			verts[1].tv = 1;
			verts[1].loc[0] = pix - theCurrentFeature->m_rendWidth / 2;
			verts[1].loc[1] = piy + theCurrentFeature->m_rendHeight / 2;
			verts[1].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[1]);
			//2
			verts[2].tu = 1;
			verts[2].tv = 0;
			verts[2].loc[0] = pix + theCurrentFeature->m_rendWidth / 2;
			verts[2].loc[1] = piy - theCurrentFeature->m_rendHeight / 2;
			verts[2].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[2]);
			//2
			verts[3].tu = 1;
			verts[3].tv = 0;
			verts[3].loc[0] = pix + theCurrentFeature->m_rendWidth / 2;
			verts[3].loc[1] = piy - theCurrentFeature->m_rendHeight / 2;
			verts[3].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[3]);
			//1
			verts[4].tu = 0;
			verts[4].tv = 1;
			verts[4].loc[0] = pix - theCurrentFeature->m_rendWidth / 2;
			verts[4].loc[1] = piy + theCurrentFeature->m_rendHeight / 2;
			verts[4].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[4]);
			//3
			verts[5].tu = 1;
			verts[5].tv = 1;
			verts[5].loc[0] = pix + theCurrentFeature->m_rendWidth / 2;
			verts[5].loc[1] = piy + theCurrentFeature->m_rendHeight / 2;
			verts[5].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[5]);
		}
		theBegin++;
	}
	//»æÖÆ
	CMarkerElement theElement;
	theElement.m_Style = theStyle;
	theElement.RenderManyUseOgles(theVertexVector);
}