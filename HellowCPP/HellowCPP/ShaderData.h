#pragma once
#include "stdafxppapi.h"
class ShaderData
{
public:
	GLuint m_VerShader;						//着色器
	GLuint m_FramShader;
	GLuint m_Program;
public:
	ShaderData() 
		: m_VerShader(0)
		, m_FramShader(0)
		, m_Program(0)
	{
	}
	~ShaderData()
	{
		if(m_VerShader > 0)
		{
			glDeleteShader(m_VerShader);
			m_VerShader = 0;
		}
		if (m_FramShader > 0)
		{
			glDeleteShader(m_FramShader);
			m_FramShader = 0;
		}
		if (m_Program > 0)
		{
			glDeleteProgram(m_Program);
			m_Program = 0;
		}
	}
};
class ColorShader
{
public:
	ShaderData m_ShaderData;
	GLint m_PositioinLoc;
	GLint m_ColorLoc;
	GLint m_MvpLoc;
	ColorShader()
		: m_PositioinLoc(-1)
		, m_ColorLoc(-1)
		, m_MvpLoc(-1)
	{}
	~ColorShader(){}

};
class TextureShader
{
public:
	ShaderData m_ShaderData;
	GLint m_PositioinLoc;
	GLint m_MvpLoc;
	GLint m_TexCoord;
	TextureShader()
		: m_PositioinLoc(-1)
		, m_MvpLoc(-1)
		, m_TexCoord(-1)
	{}
	~TextureShader(){}
};
struct Vertex
{
	float tu, tv;
	float loc[3];
	float color[4];
};


ColorShader		g_ColorShader;		//颜色着色器
TextureShader	g_TextureShaer;		//贴图着色器
GLfloat			g_mpv[16];


//地图纹理最大的缓存
const int MAX_MAP_TEXTURE_TEMP = 2000;
//绑定2D纹理
//纹理号码
//图片的宽高
//图片的数据内容
void Bind2DTextureFromBuffer(GLuint& theTexttureID, int theImageWidth, int theImageHeight, GLubyte* theBuffer)
{
	glBindTexture(GL_TEXTURE_2D, theTexttureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, theImageWidth, theImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, theBuffer);
	//过滤模式
	//放大过滤
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	//超过TS坐标时候的采样
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//#define GLORTO2
#define GLHPERSPECTIVEF2


#ifdef GLORTO2
const double g_Zlocation = 1;
#endif

#ifdef GLHPERSPECTIVEF2
const double g_Zlocation = 1.0;
const double g_xrotate = -60;
double g_xtranslate = 0;
double g_ytranslate = 0;
double g_ztranslate = -1000;
#endif


