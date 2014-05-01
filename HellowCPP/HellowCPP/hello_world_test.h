#pragma once
#include "DrawEngine.h"
class HelloTutorialInstance 
	: public pp::Instance
	, public pp::Graphics3DClient
{
public:
	/// The constructor creates the plugin-side instance.
	/// @param[in] instance the handle to the browser-side plugin instance.
	explicit HelloTutorialInstance(PP_Instance instance)
		: pp::Instance(instance)
		, pp::Graphics3DClient(this)
		, callback_factory_(this)
		, m_bFocusMap_(false)
		, m_bMapMove_(false)
	{
		this->RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_WHEEL);				//鼠标事件
		this->RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);								//键盘事件
		
		m_DrawEngine_ = new DrawEngine();
		m_Grapics3d_ = NULL;
		pp::Module* module = pp::Module::Get();
		gles2_if_ = static_cast<const PPB_OpenGLES2*>(module->GetBrowserInterface(PPB_OPENGLES2_INTERFACE));

		//测试
		m_MouseDownPoint_.set_x(0);
		m_MouseDownPoint_.set_y(0);
	}

	virtual ~HelloTutorialInstance();

	virtual void Graphics3DContextLost()
	{
		PostMessage("Graphics3DContextLost");
	}

	//处理浏览器发给插件的消息
	void HandleMessage(const pp::Var& var_message);

	//处理改变浏览器插件大小的消息
	void DidChangeView(const pp::Rect& position, const pp::Rect& clip);

	//处理改变是否选中插件的消息
	void DidChangeFocus(bool has_focus);

	//输入事件的消息
	bool HandleInputEvent (const pp::InputEvent &event);

	//。。。init（）
	bool HandleDocumentLoad (const pp::URLLoader &url_loader);

	//刷新
	void CtrlRefresh(pp::Point theBeg, pp::Point theEnd);
private:
	DrawEngine* m_DrawEngine_;					//绘制
	pp::Graphics3D* m_Grapics3d_;				//3d
	const PPB_OpenGLES2* gles2_if_;
	
	bool m_bFocusMap_;				//是否聚焦到地图
	bool m_bMapMove_;				//地图是否需要移动

	//测试学习使用的
	pp::Point m_MouseDownPoint_;

private:
	pp::CompletionCallbackFactory<HelloTutorialInstance> callback_factory_;
	void Render(int32_t result);
	
};