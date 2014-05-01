#include "hello_world_test.h"
#include "GetUrl.h"

void HelloTutorialInstance::DidChangeView (const pp::Rect& position, const pp::Rect& clip)
{
	if(m_Grapics3d_ != NULL)
		if(!m_Grapics3d_->is_null())
			return;
	{
		//当前3d环境的适配
		m_DrawEngine_->m_CtrlWidth = position.size().width();
		m_DrawEngine_->m_CtrlHeight = position.size().height();
		//3d设备的属性
		int32_t attribs[] = {
			PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
			PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
			PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8,
			PP_GRAPHICS3DATTRIB_SAMPLES, 0,
			PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
			PP_GRAPHICS3DATTRIB_WIDTH, m_DrawEngine_->m_CtrlWidth,
			PP_GRAPHICS3DATTRIB_HEIGHT, m_DrawEngine_->m_CtrlHeight,
			PP_GRAPHICS3DATTRIB_NONE
		};
		//创建3d设备
		m_Grapics3d_ = new pp::Graphics3D(this, attribs);
		if(!m_Grapics3d_ || m_Grapics3d_->is_null())
			return;
		if(BindGraphics(*m_Grapics3d_) == false)
		{
			delete m_Grapics3d_;
			m_Grapics3d_ = NULL;
			return ;
		}
		//初始化 OpenGlES的东西
		glSetCurrentContextPPAPI(m_Grapics3d_->pp_resource());
		m_DrawEngine_->m_Grapics3d_ = m_Grapics3d_;
		if(!m_DrawEngine_->InitColorShader())				//颜色着色器失败
			return ;
		if(!m_DrawEngine_->InitTextureShader())				//纹理着色器
			return;
		m_DrawEngine_->SetRect(position.size().width(), position.size().height());
	}
}
HelloTutorialInstance::~HelloTutorialInstance()
{
	if (m_DrawEngine_ != NULL)
	{
		delete m_DrawEngine_;
		m_DrawEngine_ = NULL;
	}
	if(m_Grapics3d_ != NULL)
	{
		delete m_Grapics3d_;
		m_Grapics3d_ = NULL;
	}
	/*if(gles2_if_ != NULL)
	{
	delete gles2_if_;
	gles2_if_ = NULL;
	}*/	
}
void HelloTutorialInstance::DidChangeFocus (bool has_focus)
{
}
bool HelloTutorialInstance::HandleInputEvent (const pp::InputEvent &event)
{
	//事件处理
	switch (event.GetType())
	{
	case PP_INPUTEVENT_TYPE_UNDEFINED:
		break;
	case PP_INPUTEVENT_TYPE_MOUSEDOWN:
		{
			//聚焦地图
			m_bFocusMap_ = true;
			pp::MouseInputEvent theInputEvent(event);
			int32_t theClickCount = theInputEvent.GetClickCount();
			//左击事件响应
			if (theInputEvent.GetButton() == PP_INPUTEVENT_MOUSEBUTTON_LEFT)
			{
				m_MouseDownPoint_ = theInputEvent.GetPosition();
				switch(m_DrawEngine_->m_MapOperator)
				{
				case enumMapOperator::vPanOperator:
					{
						//地图平移
						m_bMapMove_ = true;
					}
					break;
				case enumMapOperator::vDrawOperator:
					{
						//画笔绘制
						m_DrawEngine_->CreateTheFreeDrawLineElement();
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	case PP_INPUTEVENT_TYPE_MOUSEUP:
		{
			pp::MouseInputEvent theInputEvent(event);
			int32_t theClickCount = theInputEvent.GetClickCount();
			pp::Point theMouseUpPoint = theInputEvent.GetPosition();
			if (theInputEvent.GetButton() == PP_INPUTEVENT_MOUSEBUTTON_LEFT)
			{
				switch(m_DrawEngine_->m_MapOperator)
				{
				case enumMapOperator::vPanOperator:
					{
						//地图平移
						/*int centerx = m_DrawEngine_->m_CtrlWidth / 2 - (theMouseUpPoint.x() - m_MouseDownPoint_.x());
						int centery = m_DrawEngine_->m_CtrlHeight / 2 - (theMouseUpPoint.y() - m_MouseDownPoint_.y());
						m_DrawEngine_->MapMoveView(centerx, centery, -1);
						CtrlRefresh(m_MouseDownPoint_, theMouseUpPoint);*/
						m_bMapMove_ = false;
					}
					break;
				case enumMapOperator::vDrawOperator:
					{
						//画笔绘制
						m_DrawEngine_->m_FreeDrawElementHandle_ = NULL;
					}
					break;
				default:
					break;
				}
			}
		}
		break;
	case PP_INPUTEVENT_TYPE_MOUSEMOVE:
		{
			pp::MouseInputEvent theInputEvent(event);
			pp::Point theMouseMovePoint = theInputEvent.GetPosition();
			switch(m_DrawEngine_->m_MapOperator)
			{
			case enumMapOperator::vPanOperator:
				{
					if(m_bMapMove_)
					{
						int centerx = m_DrawEngine_->m_CtrlWidth / 2 - (theMouseMovePoint.x() - m_MouseDownPoint_.x());
						int centery = m_DrawEngine_->m_CtrlHeight / 2 - (theMouseMovePoint.y() - m_MouseDownPoint_.y());
						m_DrawEngine_->MapMoveView(centerx, centery, -1);
						CtrlRefresh(m_MouseDownPoint_, theMouseMovePoint);
						m_MouseDownPoint_ = theMouseMovePoint;
					}
				}
				break;
			case enumMapOperator::vDrawOperator:
				{
					if(m_DrawEngine_->m_FreeDrawElementHandle_)
					{
						double x = theMouseMovePoint.x();
						double y = theMouseMovePoint.y();
						m_DrawEngine_->AddTheFreeDrawLinePoint(x, y);
						CtrlRefresh(m_MouseDownPoint_, theMouseMovePoint);
					}
				}
				break;
			default:
				break;
			}
		}
		break;
	case PP_INPUTEVENT_TYPE_MOUSEENTER:
		break;
	case PP_INPUTEVENT_TYPE_MOUSELEAVE:
		m_bFocusMap_ = false;
		break;
	case PP_INPUTEVENT_TYPE_WHEEL:
		{
			pp::WheelInputEvent theInputEvent(event);
			PP_FloatPoint thePointClick = theInputEvent.GetTicks();
			if(m_bFocusMap_)
			{
				CBaseLayer* theBaseLayer = m_DrawEngine_->m_VtronMap.m_BaseLayer_;
				if(theBaseLayer)
				{
					m_DrawEngine_->MapMoveView(m_DrawEngine_->m_CtrlWidth / 2, m_DrawEngine_->m_CtrlHeight / 2, 
						theBaseLayer->m_js + thePointClick.x + thePointClick.y);
					CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
				}
			}
		}
		break;
	case PP_INPUTEVENT_TYPE_RAWKEYDOWN:
		break;
	case PP_INPUTEVENT_TYPE_KEYDOWN:
		break;
	case PP_INPUTEVENT_TYPE_KEYUP:
		break;
	case PP_INPUTEVENT_TYPE_CHAR:
		break;
	default:
		return false;
		break;
	}
	return true;
}

bool HelloTutorialInstance::HandleDocumentLoad (const pp::URLLoader &url_loader)
{
	return true;
}

void HelloTutorialInstance::HandleMessage (const pp::Var& message)
{
	std::string theReceiveMessage = message.AsString();
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(theReceiveMessage.c_str(), root);
	if (parsingSuccessful)
	{
		std::string theCmdName = root["CmdName"].asString();
		if(theCmdName == "CRTLREFRESH")
		{
			CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
		}
		else
		{
			Json::Value theReturnInfo;
			m_DrawEngine_->CommandParsing(theCmdName, root, theReturnInfo);			//处理消息
			pp::Var theReturnMessage(theReturnInfo.toStyledString());				//返回消息
			PostMessage(theReturnMessage);
		}
	}
}

void HelloTutorialInstance::CtrlRefresh(pp::Point theBeg, pp::Point theEnd)
{
	MapEngineES_Point beg;
	MapEngineES_Point end;
	beg.x = theBeg.x();
	beg.y = theBeg.y();
	end.x = theEnd.x();
	end.y = theEnd.y();
	/*glViewport(0, 0, m_DrawEngine_->m_CtrlWidth, m_DrawEngine_->m_CtrlHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);*/
	//{
	//	//测试颜色着色器
	//	glUseProgram(g_ColorShader.m_ShaderData.m_Program);
	//	glUniformMatrix4fv(g_ColorShader.m_MvpLoc, 1, GL_FALSE, (GLfloat*)g_mpv);
	//	m_DrawEngine_->DrawRect(beg, end);
	//}

	//{
	//	//测试贴图着色器
	//	glUseProgram(g_TextureShaer.m_ShaderData.m_Program);
	//	glUniformMatrix4fv(g_TextureShaer.m_MvpLoc, 1, GL_FALSE, (GLfloat*)g_mpv);
	//	m_DrawEngine_->DrawAnPicture("D:/yanjianlongDocument/test/0_0.jpg", (end.x + beg.x) / 2, (end.y + beg.y) / 2, end.x - beg.x, end.y - beg.y);
	//}
	m_DrawEngine_->Draw();
	if(m_Grapics3d_ != NULL)
	{
		pp::CompletionCallback cb = callback_factory_.NewCallback(&HelloTutorialInstance::Render);
		m_Grapics3d_->SwapBuffers(cb);
	}
}
void HelloTutorialInstance::Render(int32_t result)
{
}