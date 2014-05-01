#include "DrawEngine.h"
DrawEngine::DrawEngine()
	: m_bRefreshMap_(true)
	, ztranslate(0)
	, xtranslate(0)
	, ytranslate(0) 
	, xrotate(-45)
{
	//gdi+初始化
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;						// GDI+
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
	m_Grapics3d_ = NULL;
	g_out_file_log.open("d:\\MapEngineES_log.txt");
	//函数列表
	m_PicScale_ = 1.0;
	m_Function.clear();
	m_MapOperator = enumMapOperator::vPanOperator;
	m_FreeDrawElementHandle_ = NULL;
	m_Function["OPEN"] = &DrawEngine::OpenMap;							//open the map
	m_Function["REGMARKERSTYLE"] = &DrawEngine::RegistStyle;			//regist an icon
	m_Function["ADDMARKERPOINTLAYER"] = &DrawEngine::LoadTheShapefile;	//open the shapefile
	m_Function["ADDWFSPOINTLAYER"] = &DrawEngine::LoadTheWFSLayer;		// WFSLayer

	m_Function["MOVEVIEW"] = &DrawEngine::ChangeMapView;				//move the map
	m_Function["OPERATORCHANGE"] = &DrawEngine::ChangeOperator;			//change the operator;
	m_Function["HIDELAYER"] = &DrawEngine::HideLayer;					// hide layer
	m_Function["SHOWLAYER"] = &DrawEngine::ShowLayer;
	m_Function["DELETELAYER"] = &DrawEngine::DeleteLayer;

}
DrawEngine::~DrawEngine()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);  
	m_Function.clear();
	g_out_file_log.close();
}

bool DrawEngine::PixToProjection(const int& x, const int& y, double& longtitude, double& latitude)
{
	longtitude = 0;
	latitude = 0;
	if(m_VtronMap.m_BaseLayer_)
	{
		double theResolution;
		m_VtronMap.m_BaseLayer_->GetBaseLayerResolution(m_VtronMap.m_BaseLayer_->m_js, theResolution);
		Pixel_to_Projection(m_VtronMap.m_BaseLayer_->m_CenLon, m_VtronMap.m_BaseLayer_->m_CenLat, m_CtrlWidth / 2, m_CtrlHeight / 2, longtitude, latitude, x, y, theResolution);
		return true;
	}
	return false;
}
bool DrawEngine::ProjectionToPix(const double& longtitude, const double& latitude, int& x, int& y)
{
	x = 0;
	y = 0;
	if(m_VtronMap.m_BaseLayer_)
	{
		double theResolution;
		m_VtronMap.m_BaseLayer_->GetBaseLayerResolution(m_VtronMap.m_BaseLayer_->m_js, theResolution);
		Projection_to_Pixel(m_VtronMap.m_BaseLayer_->m_CenLon, m_VtronMap.m_BaseLayer_->m_CenLat, m_CtrlWidth / 2, m_CtrlHeight / 2, longtitude, latitude, x, y, theResolution);
		return true;
	}
	return false;
}


void DrawEngine::CommandParsing(const std::string& CmdName, const Json::Value& root, Json::Value& returnInfo)
{
	std::map<std::string, HandleFun>::iterator theFunction = m_Function.find(CmdName);
	if(theFunction != m_Function.end())
		(this->*(theFunction->second))(root, returnInfo);
}

Json::Value DrawEngine::ConstructJsonValue(const std::string& CmdName, const int& CmdID, const int& elementID, 
	const int& layerid, const int& status, const int& styleid, const int& mapLevel)
{
	//ret.Format("{\"Cmdid\":%d,\"CmdName\":\"%s\",\"Status\":%d,\"Layerid\":%d,\"Elementid\":%d,\"Styleid\":%d,\"MapExtern\":[%lf,%lf,%lf,%lf],\"MapLevel\":%d}",cmdid,cmdname,status,layerid,elementid,styleid,x0,y0,x1,y1,js);
	Json::Value theReturnInfo;
	theReturnInfo["CmdName"] = Json::Value(CmdName);
	theReturnInfo["Cmdid"] = Json::Value(CmdID);
	theReturnInfo["Status"] = Json::Value(status);
	theReturnInfo["Layerid"] = Json::Value(layerid);
	theReturnInfo["Elementid"] = Json::Value(elementID);
	{
		if(mapLevel > -1)
			theReturnInfo["mapLevel"] = Json::Value(mapLevel);
		else
		{
			int theMapJs = -1;
			if(m_VtronMap.m_BaseLayer_)
				theMapJs = m_VtronMap.m_BaseLayer_->m_js;
			theReturnInfo["mapLevel"] = Json::Value(theMapJs);
		}	
	}
	theReturnInfo["Styleid"] = Json::Value(styleid);
	double xlefttop;
	double ylefttop;
	double xrightbottom;
	double yrightbottom;
	PixToProjection(0, 0, xlefttop, ylefttop);
	PixToProjection(m_CtrlWidth, m_CtrlHeight, xrightbottom, yrightbottom);
	theReturnInfo["MapExtern"].append(xlefttop);
	theReturnInfo["MapExtern"].append(ylefttop);
	theReturnInfo["MapExtern"].append(xrightbottom);
	theReturnInfo["MapExtern"].append(yrightbottom);
	return theReturnInfo;
}

void DrawEngine::ChangeOperator(const Json::Value& root, Json::Value& returnInfo)
{
	//str = '{"CmdName":"OPERATORCHANGE","OPERATOR":"PAN","Cmdid":21}';
	long LayerHandle = -1;
	std::string theOpeator = root["OPERATOR"].asString();
	if(theOpeator.compare("CHOOSE") == 0)
		m_MapOperator = enumMapOperator::vChooseOperator;
	else if(theOpeator.compare("PAN") == 0)
		m_MapOperator = enumMapOperator::vPanOperator;
	else if(theOpeator.compare("DRAW") == 0)
	{
		//str = '{"CmdName":"OPERATORCHANGE","OPERATOR":"DRAW","Width":10,"ColorR":255,"ColorG":0,"ColorB":0,"ColorA":255,"Cmdid":39}'
		m_MapOperator = enumMapOperator::vDrawOperator;
		//构建一个元素层，用来绘制线条
		if(m_VtronMap.m_FreeDrawLayer_ == NULL)
		{
			m_VtronMap.m_FreeDrawLayer_ = new CElementLayer();
			m_VtronMap.m_LineWidth_ = root["Width"].asInt();
			m_VtronMap.m_LineColorR_ = root["ColorR"].asInt();
			m_VtronMap.m_LineColorG_ = root["ColorG"].asInt();
			m_VtronMap.m_LineColorB_ = root["ColorB"].asInt();
			m_VtronMap.m_LineColorA_ = root["ColorA"].asInt();
			LayerHandle = m_VtronMap.AddLayer(m_VtronMap.m_FreeDrawLayer_);
		}
	}
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, LayerHandle, 0, -1, -1);
}
void DrawEngine::HideLayer(const Json::Value& root, Json::Value& returnInfo)
{
	int LayerHandle = root["Layerid"].asInt();;
	CLayer* layer = m_VtronMap.GetLayerByHandle(LayerHandle);
	if(layer)
		layer->m_bVisible = false;
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, LayerHandle, 0, -1, -1);
}
void DrawEngine::ShowLayer(const Json::Value& root, Json::Value& returnInfo)
{
	int LayerHandle = root["Layerid"].asInt();;
	CLayer* layer = m_VtronMap.GetLayerByHandle(LayerHandle);
	if(layer)
		layer->m_bVisible = true;
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, LayerHandle, 0, -1, -1);
}

void DrawEngine::DeleteLayer(const Json::Value& root, Json::Value& returnInfo)
{
	int LayerHandle = root["Layerid"].asInt();;
	m_VtronMap.RemoveLayerByHandle(LayerHandle);
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, LayerHandle, 0, -1, -1);
}

void DrawEngine::ChangeMapView(const Json::Value& root, Json::Value& returnInfo)
{
	//str = '{"CmdName":"MOVEVIEW","Coortype":"GEO","CenX":26000,"CenY":-12000,"Level":'+js+',"Cmdid":2}';
	// '{"CmdName":"MOVEVIEW","Coortype":"PIX","OffsetX":0,"OffsetY":-'+offsetY+',"Level":'+js+',"Cmdid":2}'
	//地图移动类型
	std::string coortype = root["Coortype"].asString();
	if (coortype.compare("PIX") == 0)
	{
		int pixX = root["OffsetX"].asInt();
		int pixY = root["OffsetY"].asInt();
		int js = root["Level"].asInt();
		int centx = m_CtrlWidth / 2 + pixX;
		int centy = m_CtrlHeight / 2 + pixY;
		CBaseLayer *baselayer = m_VtronMap.m_BaseLayer_;
		if (NULL != baselayer)
		{
			if(js < 0)
				js = 0;
			else if(js > baselayer->m_MaxLevel)
				js = baselayer->m_MaxLevel;
			MapMoveView(centx, centy, js);
		}
	}	
	else if (coortype.compare("GEO") == 0)
	{
		double prxX = root["CenX"].asDouble();
		double prxY = root["CenY"].asDouble();
		int js = root["Level"].asInt();
		CBaseLayer *baselayer = m_VtronMap.m_BaseLayer_;
		if (NULL != baselayer)
		{
			int x = 0;
			int y = 0;
			if(js < 0)
				js = 0;
			else if(js > baselayer->m_MaxLevel)
				js = baselayer->m_MaxLevel;
			ProjectionToPix(prxX, prxY, x, y);
			MapMoveView(x, y, js);
		}
	}
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, -1, 0, -1, -1);
}
void DrawEngine::LoadTheShapefile(const Json::Value& root, Json::Value& returnInfo)
{
	//str = '{"CmdName":"ADDMARKERPOINTLAYER","PointFileURL":"","Markerid":0,"Width":32,"Height":32,"Cmdid":4}';
	std::string path = root["PointFileURL"].asString();
	/*CString filename;
	StringToCString(path, filename);*/
	int markerhandle = root["Markerid"].asInt();
	int theWidth = root["Width"].asInt() * m_PicScale_;
	int theHeight = root["Height"].asInt() * m_PicScale_;
	CStyle* theStyle = m_VtronMap.GetStyleByHandle(markerhandle);
	int theLayerHandle = -1;
	if (theStyle)
	{
		CShapefileLayer* theLayer = new CShapefileLayer();
		theLayer->m_Height = theHeight;
		theLayer->m_Width = theWidth;
		theLayer->m_Style = (CMarkerStyle*)theStyle;
		theLayer->OpenShapefile(path);
		theLayerHandle = m_VtronMap.AddLayer(theLayer);
	}
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, theLayerHandle, 0, -1, -1);
}
void DrawEngine::LoadTheWFSLayer(const Json::Value& root, Json::Value& returnInfo)
{
	//str = '{"CmdName":"ADDMARKERPOINTLAYER","PointFileURL":"","Markerid":0,"Width":32,"Height":32,"Cmdid":4}';
	std::string path = root["PointFileURL"].asString();
	/*CString filename;
	StringToCString(path, filename);*/
	int markerhandle = root["Markerid"].asInt();
	int theWidth = root["Width"].asInt() * m_PicScale_;
	int theHeight = root["Height"].asInt() * m_PicScale_;
	CStyle* theStyle = m_VtronMap.GetStyleByHandle(markerhandle);
	std::string theLayerName = root["LayerName"].asString();
	int theLayerHandle = -1;
	if (theStyle)
	{
		CWFSLayer* theLayer = new CWFSLayer();
		theLayer->put_LayerName(theLayerName);
		theLayer->put_Rect(theWidth, theHeight);
		theLayer->put_Style((CMarkerStyle*)theStyle);
		theLayer->put_UrlPath(path);
		theLayer->GetData();
		theLayerHandle = m_VtronMap.AddLayer(theLayer);
	}
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, theLayerHandle, 0, -1, -1);
}

void DrawEngine::RegistStyle(const Json::Value& root, Json::Value& returnInfo)
{
	//str = '{"CmdName":"REGMARKERSTYLE","PicURL":"","Cmdid":3}'
	//注册图片的样式
	std::string picpath = root["PicURL"].asString();
	CString theFilePath;
	StringToCString(picpath, theFilePath);
	CMarkerStyle* markerstyle = new CMarkerStyle();
	int theStyleHandle = -1;
	if (markerstyle->Init(theFilePath))
		theStyleHandle = m_VtronMap.AddStyle(markerstyle);
	else if(markerstyle)
		delete markerstyle;
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, -1, 0, theStyleHandle, -1);
}

void DrawEngine::SetRect(const long& width, const long& height)
{
	m_CtrlWidth = width;
	m_CtrlHeight = height;
#ifdef GLHPERSPECTIVEF2

	//create our perspective matrix
	GLfloat mpv[16];
	GLfloat trs[16];
	GLfloat rot[16];

	identity_matrix(&mpv [0]);
	// near and far just mean the distance to view
	glhPerspectivef2(&mpv [0], 45.0f, width * 1.0 / height, 2, 100000);
	ztranslate = -(m_CtrlWidth + m_CtrlHeight) / 2;
	xtranslate = -width / 2;
	ytranslate = -height / 2 - 256 / 2;                                                         // the picture size is 256
	rotate_matrix(xrotate , 0 , 0.0f, rot);                                      // z * y * x
	translate_matrix(xtranslate , ytranslate, ztranslate, trs );
	multiply_matrix(trs , rot, trs);                                                                                                                // trs * rot
	multiply_matrix(&mpv [0], trs, & g_mpv[0]);                                                     // mpv * trs

	////create our perspective matrix
	//GLfloat mpv[16];
	//GLfloat trs[16];
	//GLfloat rot[16];

	//identity_matrix(&mpv[0]);
	//glhPerspectivef2(&mpv[0], 45.0f, width * 1.0 / height, 2, 100000);
	//g_xtranslate = -width / 2;
	//double result = (90 + g_xrotate) * (M_PI/180.0f);
	//result = (1 - sin(result));
	//result *= height / 2;
	//result *= 1.5;
	//g_ytranslate = -result;
	//g_ztranslate = -1000;
	//translate_matrix(g_xtranslate, g_ytranslate, g_ztranslate, trs);
	//rotate_matrix(g_xrotate, 0 , 0.0f, rot);
	//multiply_matrix(trs, rot, trs);
	//multiply_matrix(&mpv[0], trs, &g_mpv[0]);
	return;
#endif

#ifdef GLORTO2
	glhOrtho2(&g_mpv[0], 0, width, 0, height, -2, 2);
#endif
	/*GLfloat theMatrix[16];
	Vector3 theEye;
	Vector3 at;
	Vector3 up;
	theEye.x = 0;
	theEye.y = -2;
	theEye.z = 2;

	at.x = 0;
	at.y = 0;
	at.z = 0;

	up.x = 0;
	up.y = 2;
	up.z = 2;

	GetLookAtMatrix(theEye, at, up, &theMatrix[0]);
	GLfloat theCurrentMatrix[16];
	identity_matrix(&theCurrentMatrix[0]);
	multiply_matrix(&g_mpv[0], &theMatrix[0], &theCurrentMatrix[0]);
	for (int i = 0; i < 16; i++)
	g_mpv[i] = theCurrentMatrix[i];*/
}
void DrawEngine::OpenMap(const Json::Value& root, Json::Value& returnInfo)
{
	//'{"CmdName":"OPEN","MapName":"HZYX","CenX":120.1,"CenY":30.2,"Level":5,"URL":"c:\\\\SmartCity\\\\MAP\\\\HZYX","MapType":"ArcGisMap","Format":"jpg","Cmdid":19}'
	//创建一个底图
	std::string theMapType = root["MapType"].asString();
	std::string theFormat = root["Format"].asString();
	double theCentX = root["CenX"].asDouble();
	double theCentY = root["CenY"].asDouble();
	std::string theUrl = root["URL"].asString();
	std::string theMapName = root["MapName"].asString();
	int theLevel = root["Level"].asInt();
	bool bClearUp = false;
	bClearUp = root["ClearUp"].asBool();
	if(m_VtronMap.m_BaseLayer_)
		delete m_VtronMap.m_BaseLayer_;
	m_VtronMap.m_BaseLayer_ = NULL;
	if (bClearUp)
		m_VtronMap.RemoveAllLayer();
	CBaseLayer* theBaseLayer = NULL;
	int LayerHandle = -1;
	if(theMapType == "VtronMap")
	{

	}
	else if(theMapType == "ArcGisMap")
	{
		CArcgisMapLayer* theMapLayer = new CArcgisMapLayer(theCentX, theCentY, theLevel, theMapName, theUrl, theFormat, m_CtrlWidth, m_CtrlHeight);
		LayerHandle = m_VtronMap.AddLayer(theMapLayer);
	}
	else if(theMapType == "ArcGisCompactMap")
	{

	}
	else if(theMapType == "DeegreeMap")
	{
		CServerMapLayer* theMapLayer = new CServerMapLayer(theCentX, theCentY, theLevel, theMapName, theUrl, theFormat, m_CtrlWidth, m_CtrlHeight, MapType::DegreeServermap);
		LayerHandle = m_VtronMap.AddLayer(theMapLayer);
		if(m_Grapics3d_)
			theMapLayer->m_Grapics3d_ = m_Grapics3d_;
	}
	else if(theMapType == "ArcServerMap")
	{
		CServerMapLayer* theMapLayer = new CServerMapLayer(theCentX, theCentY, theLevel, theMapName, theUrl, theFormat, m_CtrlWidth, m_CtrlHeight, MapType::ArcgisServerMap);
		LayerHandle = m_VtronMap.AddLayer(theMapLayer);
	}
	else if(theMapType == "PGISMap")
	{
		CServerMapLayer* theMapLayer = new CServerMapLayer(theCentX, theCentY, theLevel, theMapName, theUrl, theFormat, m_CtrlWidth, m_CtrlHeight, MapType::PGISMap);
		LayerHandle = m_VtronMap.AddLayer(theMapLayer);
		if(m_Grapics3d_)
			theMapLayer->m_Grapics3d_ = m_Grapics3d_;
	}
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, LayerHandle, 0, -1, theLevel);
}

bool DrawEngine::InitTheProgram(const char* VShaderData, const char* FShaderData, 
	GLuint& vershader, GLuint& frashader, GLuint& textProgram)
{
	vershader = InitTheShader(GL_VERTEX_SHADER, VShaderData);
	frashader = InitTheShader(GL_FRAGMENT_SHADER, FShaderData);
	if(frashader == 0 || vershader == 0)
		return false;
	textProgram = glCreateProgram();					//创建项目连接对象,可执行程序
	glAttachShader(textProgram, vershader);
	glAttachShader(textProgram, frashader);
	//glBindAttribLocation(g_ColorShader.m_PositioinLoc, 0, "vPosition"); 
	glLinkProgram(textProgram);						//连接着色器
	GLint theLink;
	glGetProgramiv(textProgram, GL_LINK_STATUS, &theLink);
	if (!theLink)		//链接错误
	{
		GLint f_infolen = 0;
		glGetProgramiv(textProgram, GL_INFO_LOG_LENGTH, &f_infolen);		//失败日志的长度
		if (f_infolen > 0)
		{
			char* f_infolog = new char[f_infolen];
			glGetProgramInfoLog(textProgram, f_infolen, NULL, f_infolog);	//日志
			delete[] f_infolog;
		}
		glDeleteProgram(textProgram);
		textProgram = 0;
		return false;
	}
	return true;
}
bool DrawEngine::InitColorShader()
{
	//着色器代码
	const char* VShaderData =    
		"uniform mat4 a_MVP;"
		"attribute vec4 a_color;"
		"attribute vec4 a_position;"
		"varying  vec4 v_color;"
		"void main()" 
		"{"
		"gl_Position = a_MVP*a_position;"
		"v_color = a_color;"
		"}";

	const char* FShaderData =    
		"precision mediump float;"
		"varying vec4 v_color;"
		"void main()"
		"{ "
		"gl_FragColor = v_color;"
		"}";
	bool theInitResult = InitTheProgram(VShaderData, FShaderData, 
		g_ColorShader.m_ShaderData.m_VerShader, g_ColorShader.m_ShaderData.m_FramShader, 
		g_ColorShader.m_ShaderData.m_Program);
	if(theInitResult)
	{
		g_ColorShader.m_PositioinLoc = glGetAttribLocation(g_ColorShader.m_ShaderData.m_Program, "a_position");	
		g_ColorShader.m_ColorLoc = glGetAttribLocation(g_ColorShader.m_ShaderData.m_Program, "a_color");			//获取变量的地址
		g_ColorShader.m_MvpLoc = glGetUniformLocation(g_ColorShader.m_ShaderData.m_Program, "a_MVP");				//获取常量的地址
	}

	return theInitResult;
}

bool DrawEngine::InitTextureShader()
{
	//着色器代码
	const char* VShaderData =    
		"uniform mat4 a_MVP;"				
		"attribute vec2 a_texCoord;"
		"attribute vec4 a_position;"
		"varying  vec2 v_texCoord;"
		"void main()" 
		"{"
		"gl_Position = a_MVP*a_position;"
		"v_texCoord = a_texCoord;"
		"}";

	const char* FShaderData =    
		"precision mediump float;"
		"varying vec2 v_texCoord;"
		"uniform sampler2D s_texture;"
		"void main()"
		"{"
		"gl_FragColor = texture2D(s_texture, v_texCoord);"
		"}";
	bool theInitResult = InitTheProgram(VShaderData, FShaderData, 
		g_TextureShaer.m_ShaderData.m_VerShader, g_TextureShaer.m_ShaderData.m_FramShader, 
		g_TextureShaer.m_ShaderData.m_Program);
	if(theInitResult)
	{
		g_TextureShaer.m_MvpLoc = glGetUniformLocation(g_TextureShaer.m_ShaderData.m_Program, "a_MVP");
		g_TextureShaer.m_PositioinLoc = glGetAttribLocation(g_TextureShaer.m_ShaderData.m_Program, "a_position");
		g_TextureShaer.m_TexCoord = glGetAttribLocation(g_TextureShaer.m_ShaderData.m_Program, "a_texCoord");
	}
	return theInitResult;
}

GLuint DrawEngine::InitTheShader(const GLenum& type, const char* theCode)
{
	const char *shaderStrings[1];
	shaderStrings[0] = theCode;
	GLuint theShader = glCreateShader(type);				//创建着色器
	if (theShader == 0)
		return 0;
	glShaderSource(theShader, 1, shaderStrings, NULL);	//提供着色器代码
	glCompileShader(theShader);							//编译着色器
	GLint theCompiledState;
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &theCompiledState);		//获取着色器的编译结果
	if (!theCompiledState)								//失败
	{
		GLint f_infolen = 0;
		glGetShaderiv(theShader, GL_INFO_LOG_LENGTH, &f_infolen);		//失败日志的长度
		if (f_infolen > 0)
		{
			char* f_infolog = new char[f_infolen];
			glGetShaderInfoLog(theShader, f_infolen, NULL, f_infolog);	//日志
			delete[] f_infolog;
		}
		glDeleteShader(theShader);
		return 0;
	}
	return theShader;
}

void DrawEngine::Draw()
{
	glEnable(GL_BLEND);							//启用色彩混合
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, m_CtrlWidth, m_CtrlHeight);
	//底图
	CBaseLayer* theBaseLayer = m_VtronMap.m_BaseLayer_;
	if(!theBaseLayer)
		return;
	//热点图标绘制
	//计算当前地图信息
	double xcent = theBaseLayer->m_CenLon;
	double ycent = theBaseLayer->m_CenLat;
	double resolution = 1.0;		
	if(m_bRefreshMap_)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);
		//贴图着色器
		glUseProgram(g_TextureShaer.m_ShaderData.m_Program);
		glUniformMatrix4fv(g_TextureShaer.m_MvpLoc, 1, GL_FALSE, (GLfloat*)g_mpv);

		theBaseLayer->Draw();
		theBaseLayer->GetBaseLayerResolution(theBaseLayer->m_js, resolution);
		//遍历绘制
		std::map<long, CLayer*>::iterator theBeginLayer = m_VtronMap.m_LayerMap_.begin();
		std::map<long, CLayer*>::iterator theEndLayer = m_VtronMap.m_LayerMap_.end();
		CLayer* theHotLayer = NULL;
		for (; theBeginLayer != theEndLayer; theBeginLayer++)
		{
			theHotLayer = theBeginLayer->second;
			if(theHotLayer->m_layerType == VLayerType::vHotLayer && theHotLayer->m_bVisible)
			{
				//每次绘制都需要
				theHotLayer->m_CenLat = ycent;
				theHotLayer->m_CenLon = xcent;
				theHotLayer->m_CtrlHeight = m_CtrlHeight;
				theHotLayer->m_CtrlWidth = m_CtrlWidth;
				theHotLayer->m_Resolution = resolution;
				theHotLayer->Draw();
			}
		}
	}
	{
		//color shader
		glUseProgram(g_ColorShader.m_ShaderData.m_Program);
		glUniformMatrix4fv(g_ColorShader.m_MvpLoc, 1, GL_FALSE, (GLfloat*)g_mpv);
		CElementLayer* theTempDrawLayer = m_VtronMap.m_FreeDrawLayer_;
		if(theTempDrawLayer)
		{
			theTempDrawLayer->m_CenLon = xcent;
			theTempDrawLayer->m_CenLat = ycent;
			theTempDrawLayer->m_CtrlWidth = m_CtrlWidth;
			theTempDrawLayer->m_CtrlHeight = m_CtrlHeight;
			theTempDrawLayer->m_Resolution = resolution;
			theTempDrawLayer->Draw();
		}
	}
}
//移动视角
//以该点屏幕为中心点坐标， 比例尺 js < 0 当前比例尺
void DrawEngine::MapMoveView(int pixcent, int piycent, int js)
{
	CBaseLayer* theBaseLayer = m_VtronMap.m_BaseLayer_;
	if(theBaseLayer)
	{
		if(js < 0)
			js = theBaseLayer->m_js;
		else if(js > theBaseLayer->m_MaxLevel)
			js = theBaseLayer->m_MaxLevel;
		double thelongtitude;
		double thelatitude;
		PixToProjection(pixcent, piycent, thelongtitude, thelatitude);
		theBaseLayer->SetMapCenter(thelongtitude, thelatitude, g_Zlocation, js);
	}
	m_bRefreshMap_ = true;
}
void DrawEngine::CreateTheFreeDrawLineElement()
{
	//创建自由绘制的元素层
	if(m_VtronMap.m_FreeDrawLayer_ != NULL)
	{
		//create the line element;
		m_FreeDrawElementHandle_ = new CLineElement(m_VtronMap.m_LineColorR_, m_VtronMap.m_LineColorG_, 
			m_VtronMap.m_LineColorB_, m_VtronMap.m_LineColorA_, m_VtronMap.m_LineWidth_);
		if(m_FreeDrawElementHandle_)
			m_VtronMap.m_FreeDrawLayer_->AddElement(m_FreeDrawElementHandle_);
	}
	m_bRefreshMap_ = false;
}
void DrawEngine::AddTheFreeDrawLinePoint(const int& x, const int& y)
{
	if(m_FreeDrawElementHandle_)
	{
		double ProX;
		double ProY;
		PixToProjection(x, y, ProX, ProY);
		m_FreeDrawElementHandle_->AddPoint(ProX, ProY);
	}
}

void DrawEngine::DrawRect(const MapEngineES_Point& pStart, const MapEngineES_Point& pEnd)
{
	if(pStart.x < 0 || pStart.y < 0 || pEnd.x < 0 || pEnd.y < 0)
		return;
	long pix1 = pStart.x;
	long piy1 = pStart.y;
	long pix2 = pEnd.x;
	long piy2 = pEnd.y;

	Vertex* verts = new Vertex[4];

	verts[0].tu = 0;
	verts[0].tv = 0;
	verts[0].loc[0] = pix1;
	verts[0].loc[1] = piy1;
	verts[0].loc[2] = 1;
	verts[0].color[0] = 255/255.0;
	verts[0].color[1] = 0/255.0;
	verts[0].color[2] = 0/255.0;
	verts[0].color[3] = 100/255.0;

	verts[1].tu = 0;
	verts[1].tv = 0;
	verts[1].loc[0] = pix1;
	verts[1].loc[1] = piy2;
	verts[1].loc[2] = 1;
	verts[1].color[0] = 255/255.0;
	verts[1].color[1] = 0/255.0;
	verts[1].color[2] = 0/255.0;
	verts[1].color[3] = 100/255.0;

	verts[2].tu = 0;
	verts[2].tv = 0;
	verts[2].loc[0] = pix2;
	verts[2].loc[1] = piy1;
	verts[2].loc[2] = 1;
	verts[2].color[0] = 255/255.0;
	verts[2].color[1] = 0/255.0;
	verts[2].color[2] = 0/255.0;
	verts[2].color[3] = 100/255.0;

	verts[3].tu = 0;
	verts[3].tv = 0;
	verts[3].loc[0] = pix2;
	verts[3].loc[1] = piy2;
	verts[3].loc[2] = 1;
	verts[3].color[0] = 255/255.0;
	verts[3].color[1] = 0/255.0;
	verts[3].color[2] = 0/255.0;
	verts[3].color[3] = 100/255.0;
	{
		//VBO  顶点 缓存 对象
		GLuint vboid;
		glGenBuffers(1, &vboid);						//创建缓冲区对象
		glBindBuffer(GL_ARRAY_BUFFER, vboid);			//设置为当前缓冲区对象
		//顶点矩阵初始化
		glBufferData(GL_ARRAY_BUFFER,					
			4* sizeof(Vertex),					//缓冲区字节数
			verts,								//缓冲区的指针
			GL_STATIC_DRAW);
		delete[] verts;
		verts = NULL;
		////缓冲区数据跟新
		//glBufferSubData(GL_ARRAY_BUFFER, );
		//顶点数组
		glVertexAttribPointer(g_ColorShader.m_PositioinLoc,		//属性的索引
			3,								//坐标是个3维的向量
			GL_FLOAT,						//类型
			GL_FALSE,						//是否归一化
			sizeof(Vertex),					//字节数
			(void*)offsetof(Vertex, loc));	//指针
		glEnableVertexAttribArray(g_ColorShader.m_PositioinLoc);	

		glVertexAttribPointer(g_ColorShader.m_ColorLoc, 
			4, 
			GL_FLOAT, 
			GL_FALSE, 
			sizeof(Vertex), 
			(void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(g_ColorShader.m_ColorLoc);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);					//数据从客户内存拷贝到绘图内存
		////线宽
		//
		//GLubyte indices[6] = {0, 1, 2, 1, 2, 3};
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
		/*GLfloat width[2];
		glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, width);
		glLineWidth(width[1]);
		glDrawArrays(GL_LINE_LOOP, 0, 4);*/

		glDeleteBuffers(1, &vboid);
	}
}