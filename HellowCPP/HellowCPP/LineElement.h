#pragma once
#include "Element.h"
#include "stdafxppapi.h"
class CLineElement :
	public CElement
{
public:
	CLineElement(void);
	CLineElement(const int& r, const int& g, const int& b, const int& a, const int& w);
	~CLineElement(void);

	long AddPoint(const double& x, const double& y);
	void RenderUseOgles();

private:
	std::vector<MapEngineES_Point> m_vctPoint_; 
	int m_r;
	int m_g;
	int m_b;
	int m_a;
	int m_w;
	GLuint m_vboID;
};

