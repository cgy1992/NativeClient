#include "hello_world_test.h"
namespace pp
{
	/// The Module class.  The browser calls the CreateInstance() method to create
	/// an instance of your NaCl module on the web page.  The browser creates a new
	/// instance for each <embed> tag with type="application/x-nacl".
	class HelloTutorialModule : public pp::Module 
	{
	public:
		HelloTutorialModule() : pp::Module() {}
		virtual ~HelloTutorialModule() 
		{
			glTerminatePPAPI();
		}
		virtual pp::Instance* CreateInstance(PP_Instance instance)
		{
			return new HelloTutorialInstance(instance);
		}
		/// Called by the browser when the module is first loaded and ready to run.
		/// This is called once per module, not once per instance of the module on
		/// the page.
		virtual bool Init() 
		{
			return glInitializePPAPI(get_browser_interface()) == GL_TRUE;
		}
	};
	Module* CreateModule() 
	{
		return new HelloTutorialModule();
	}
}  // namespace pp