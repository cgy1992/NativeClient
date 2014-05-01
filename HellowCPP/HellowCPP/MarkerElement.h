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
	void RenderUseOgles();															//�������
	void RenderManyUseOgles(std::vector<Vertex>& theVertexVector);		//��������

public:
	CMarkerStyle* m_Style;			//��ʽ

	int m_rendWidth;				//��Ⱦ�Ŀ�
	int m_rendHeight;				//��Ⱦ�ĸ߶�

private:
	GLuint m_vertex_;				//����
};