#include "LineElement.h"
#include "ShaderData.h"

CLineElement::CLineElement()
{
	glGenBuffers(1, &m_vboID);
	m_r = 255;
	m_g = 0;
	m_b = 0;
	m_a = 100;
	m_w = 1;
	m_Geometry = VGeometryType::vULine;
}
CLineElement::CLineElement(const int& r, const int& g, const int& b, const int& a, const int& w)
{
	glGenBuffers(1, &m_vboID);
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
	m_w = w;
	m_Geometry = VGeometryType::vULine;
}
CLineElement::~CLineElement()
{
	glDeleteBuffers(1, &m_vboID);
}
void CLineElement::RenderUseOgles()
{
	int count = m_vctPoint_.size();
	if(count < 2)
		return;
	if(m_w == 1)
	{
		Vertex* verts = new Vertex[count];
		int pix = 0;
		int piy = 0;
		for (int i = 0; i < count; i++)
		{
			verts[i].tu = 0;
			verts[i].tv = 0;
			ProjectionToPix(m_vctPoint_[i].x, m_vctPoint_[i].y, pix, piy);
			verts[i].loc[0] = pix;
			verts[i].loc[1] = piy;
			verts[i].loc[2] = g_Zlocation;
			verts[i].color[0] = m_r / 255.0;
			verts[i].color[1] = m_g / 255.0;
			verts[i].color[2] = m_b / 255.0;
			verts[i].color[3] = m_a / 255.0;
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
		glBufferData(GL_ARRAY_BUFFER, count* sizeof(Vertex), verts, GL_STATIC_DRAW);

		glVertexAttribPointer(g_ColorShader.m_PositioinLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, loc));
		glEnableVertexAttribArray(g_ColorShader.m_PositioinLoc);

		glVertexAttribPointer(g_ColorShader.m_ColorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(g_ColorShader.m_ColorLoc);

		glDrawArrays(GL_LINE_STRIP, 0, count);//GL_TRIANGLE_STRIP

		delete[] verts;
		verts = NULL;
	}
	else 
	{
		//求出要绘制的线的顶点个数
		int triangleCount = (count - 1) * 2;//每个四边形包含2个三角形，6个顶点id
		int triangleVCount = triangleCount * 3;
		Vertex* verts = new Vertex[triangleVCount];//矩形
		int pWidth = m_w;
		int r = m_r;
		int g = m_g;
		int b = m_b;
		int a = m_a;
		bool isCol = false;			//是否横向
		for (int i = 0; i < count - 1; i++)
		{
			int pix1;
			int piy1;
			ProjectionToPix(m_vctPoint_[i].x, m_vctPoint_[i].y, pix1, piy1);
			int pix2;
			int piy2;
			ProjectionToPix(m_vctPoint_[i + 1].x, m_vctPoint_[i + 1].y, pix2, piy2);
			double x1, y1, x2, y2, x3, y3, x4, y4;
			if (pix1 == pix2)
			{
				x1 = pix1 - pWidth / 2;
				x2 = pix1 + pWidth / 2;
				x3 = pix2 - pWidth / 2;
				x4 = pix2 + pWidth / 2;
				y1 = piy1;
				y2 = piy1;
				y3 = piy2;
				y4 = piy2;
				isCol = false;
			}
			else if (piy1 == piy2)
			{
				x1 = pix1;
				x2 = pix1;
				x3 = pix2;
				x4 = pix2;
				y1 = piy1 + pWidth / 2;
				y2 = piy1 - pWidth / 2;
				y3 = piy2 + pWidth / 2;
				y4 = piy2 - pWidth / 2;
				isCol = true;
			}
			else
			{
				double d = (piy2 - piy1) * 1.0 / (pix1 - pix2);
				y1 = pWidth / 2.0 / sqrt(d * d + 1.0) + piy1;
				x1 = d * (y1 - piy1) + pix1;
				y2=-pWidth/2.0/sqrt(d*d+1.0)+piy1;
				x2=d*(y2-piy1)+pix1;
				y3=pWidth/2.0/sqrt(d*d+1.0)+piy2;
				x3=d*(y3-piy2)+pix2;
				y4=-pWidth/2.0/sqrt(d*d+1.0)+piy2;
				x4=d*(y4-piy2)+pix2;
				//if(isCol)				//横向
				//{
				//	x1 = pix1;
				//	y1 = piy1 + pWidth / 2;
				//	x2 = pix1;
				//	y2 = piy1 - pWidth / 2;
				//	x3 = pix2;
				//	y3 = piy2 + pWidth / 2;
				//	x4 = pix2;
				//	y4 = piy2 - pWidth / 2;
				//}
				//else
				//{
				//	x1 = pix1 - pWidth / 2;
				//	y1 = piy1;
				//	x2 = pix1 + pWidth / 2;;
				//	y2 = piy1;
				//	x3 = pix2 - pWidth / 2;
				//	y3 = piy2;
				//	x4 = pix2 + pWidth / 2;;
				//	y4 = piy2;
				//}
			}
			//顶点数组
			verts[i*6+0].loc[0] = x1;
			verts[i*6+0].loc[1] = y1;
			verts[i*6+0].loc[2] = g_Zlocation;
			verts[i*6+0].color[0] = r/255.0;
			verts[i*6+0].color[1] = g/255.0;
			verts[i*6+0].color[2] = b/255.0;
			verts[i*6+0].color[3] = a/255.0;

			verts[i*6+1].loc[0] = x2;
			verts[i*6+1].loc[1] = y2;
			verts[i*6+1].loc[2] = g_Zlocation;
			verts[i*6+1].color[0] = r/255.0;
			verts[i*6+1].color[1] = g/255.0;
			verts[i*6+1].color[2] = b/255.0;
			verts[i*6+1].color[3] = a/255.0;

			verts[i*6+2].loc[0] = x3;
			verts[i*6+2].loc[1] = y3;
			verts[i*6+2].loc[2] = g_Zlocation;
			verts[i*6+2].color[0] = r/255.0;
			verts[i*6+2].color[1] = g/255.0;
			verts[i*6+2].color[2] = b/255.0;
			verts[i*6+2].color[3] = a/255.0;

			verts[i*6+3].loc[0] = x3;
			verts[i*6+3].loc[1] = y3;
			verts[i*6+3].loc[2] = g_Zlocation;
			verts[i*6+3].color[0] = r/255.0;
			verts[i*6+3].color[1] = g/255.0;
			verts[i*6+3].color[2] = b/255.0;
			verts[i*6+3].color[3] = a/255.0;

			verts[i*6+4].loc[0] = x2;
			verts[i*6+4].loc[1] = y2;
			verts[i*6+4].loc[2] = g_Zlocation;
			verts[i*6+4].color[0] = r/255.0;
			verts[i*6+4].color[1] = g/255.0;
			verts[i*6+4].color[2] = b/255.0;
			verts[i*6+4].color[3] = a/255.0;

			verts[i*6+5].loc[0] = x4;
			verts[i*6+5].loc[1] = y4;
			verts[i*6+5].loc[2] = g_Zlocation;
			verts[i*6+5].color[0] = r/255.0;
			verts[i*6+5].color[1] = g/255.0;
			verts[i*6+5].color[2] = b/255.0;
			verts[i*6+5].color[3] = a/255.0;
		}

		//// 绘制矩形
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
		glBufferData(GL_ARRAY_BUFFER, triangleVCount * sizeof(Vertex), verts,
			GL_STATIC_DRAW);

		glVertexAttribPointer(g_ColorShader.m_PositioinLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
		glEnableVertexAttribArray(g_ColorShader.m_PositioinLoc);

		glVertexAttribPointer(g_ColorShader.m_ColorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glEnableVertexAttribArray(g_ColorShader.m_ColorLoc);


		glDrawArrays(GL_TRIANGLES,0,triangleVCount);
	}
}

long CLineElement::AddPoint(const double& x, const double& y)
{
	MapEngineES_Point point;
	point.x = x;
	point.y = y;
	m_vctPoint_.push_back(point);
	return 0;
}