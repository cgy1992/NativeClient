#include "MarkerElement.h"
void CMarkerElement::RenderUseOgles()
{
	//if(m_rendHeight <= 0 || m_rendWidth <= 0)
	//	return;
	////坐标点的描述
	//int theDifx = m_rendWidth / 2;
	//int theDify = m_rendHeight / 2;
	//Vertex verts[4];
	////左上角
	//verts[0].tu = 0;
	//verts[0].tv = 0;
	//verts[0].loc[0] = m_ShowCenterX - theDifx;
	//verts[0].loc[1] = m_ShowCenterY - theDify;
	//verts[0].loc[2] = 1;

	////左下角
	//verts[1].tu = 0;
	//verts[1].tv = 1;
	//verts[1].loc[0] = m_ShowCenterX - theDifx;
	//verts[1].loc[1] = m_ShowCenterY + theDify;
	//verts[1].loc[2] = 1;

	////右上角
	//verts[2].tu = 1;
	//verts[2].tv = 0;
	//verts[2].loc[0] = m_ShowCenterX + theDifx;
	//verts[2].loc[1] = m_ShowCenterY - theDify;
	//verts[2].loc[2] = 1;

	////右下角
	//verts[3].tu = 1;
	//verts[3].tv = 1;
	//verts[3].loc[0] = m_ShowCenterX + theDifx;
	//verts[3].loc[1] = m_ShowCenterY + theDify;
	//verts[3].loc[2] = 1;
	////顶点信息填充
	//glBindBuffer(GL_ARRAY_BUFFER, m_vertex_);
	//glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), (GLvoid*)&verts[0], GL_STATIC_DRAW);

	//glBindTexture(GL_TEXTURE_2D, m_Style->GetTheTextureID());

	//glVertexAttribPointer(g_TextureShaer.m_PositioinLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	//glEnableVertexAttribArray(g_TextureShaer.m_PositioinLoc);

	//glVertexAttribPointer(g_TextureShaer.m_TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	//glEnableVertexAttribArray(g_TextureShaer.m_TexCoord);

	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CMarkerElement::RenderManyUseOgles(std::vector<Vertex>& theVertexVector)
{
	if(theVertexVector.empty())
		return;
	//顶点信息填充
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_);
	glBufferData(GL_ARRAY_BUFFER, theVertexVector.size() * sizeof(Vertex), (GLvoid*)&theVertexVector[0], GL_STATIC_DRAW);

	glBindTexture(GL_TEXTURE_2D, m_Style->GetTheTextureID());

	glVertexAttribPointer(g_TextureShaer.m_PositioinLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_TextureShaer.m_PositioinLoc);

	glVertexAttribPointer(g_TextureShaer.m_TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	glEnableVertexAttribArray(g_TextureShaer.m_TexCoord);

	glDrawArrays(GL_TRIANGLES, 0, theVertexVector.size());
}