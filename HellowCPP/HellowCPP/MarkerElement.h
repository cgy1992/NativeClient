#pragma once
#include "Element.h"
#include "MarkerStyle.h"
#include "ShaderData.h"
class CMarkerElement
	: public CElement
{
public:
	CMarkerElement()
		: m_rendHeight(0)
		, m_rendWidth(0)
		, m_Style(NULL)
	{
		glGenBuffers(1, &m_vertex_);
		m_Geometry = VGeometryType::vUPointMark;
	}
	~CMarkerElement()
	{
		glDeleteBuffers(1, &m_vertex_);
	}
	void RenderUseOgles();															//单点绘制
	void RenderManyUseOgles(std::vector<Vertex>& theVertexVector);		//批量绘制

public:
	CMarkerStyle* m_Style;			//样式

	int m_rendWidth;				//渲染的宽
	int m_rendHeight;				//渲染的高度

private:
	GLuint m_vertex_;				//顶点
};