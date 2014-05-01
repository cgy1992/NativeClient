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
		this->RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_WHEEL);				//����¼�
		this->RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);								//�����¼�
		
		m_DrawEngine_ = new DrawEngine();
		m_Grapics3d_ = NULL;
		pp::Module* module = pp::Module::Get();
		gles2_if_ = static_cast<const PPB_OpenGLES2*>(module->GetBrowserInterface(PPB_OPENGLES2_INTERFACE));

		//����
		m_MouseDownPoint_.set_x(0);
		m_MouseDownPoint_.set_y(0);
	}

	virtual ~HelloTutorialInstance();

	virtual void Graphics3DContextLost()
	{
		PostMessage("Graphics3DContextLost");
	}

	//��������������������Ϣ
	void HandleMessage(const pp::Var& var_message);

	//����ı�����������С����Ϣ
	void DidChangeView(const pp::Rect& position, const pp::Rect& clip);

	//����ı��Ƿ�ѡ�в������Ϣ
	void DidChangeFocus(bool has_focus);

	//�����¼�����Ϣ
	bool HandleInputEvent (const pp::InputEvent &event);

	//������init����
	bool HandleDocumentLoad (const pp::URLLoader &url_loader);

	//ˢ��
	void CtrlRefresh(pp::Point theBeg, pp::Point theEnd);
private:
	DrawEngine* m_DrawEngine_;					//����
	pp::Graphics3D* m_Grapics3d_;				//3d
	const PPB_OpenGLES2* gles2_if_;
	
	bool m_bFocusMap_;				//�Ƿ�۽�����ͼ
	bool m_bMapMove_;				//��ͼ�Ƿ���Ҫ�ƶ�

	//����ѧϰʹ�õ�
	pp::Point m_MouseDownPoint_;

private:
	pp::CompletionCallbackFactory<HelloTutorialInstance> callback_factory_;
	void Render(int32_t result);
	
};