#pragma once
#include "stdafxcpp.h"
#include "stdafxwin32.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/url_loader.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/url_request_info.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/audio.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/graphics_3d_client.h"
#include "ppapi/gles2/gl2ext_ppapi.h"
#include "ppapi/cpp/point.h"

void CStringToString(CString theCString, std::string& theString, UINT theCode = CP_ACP)
{
	int Length = WideCharToMultiByte(theCode, 0, theCString, -1, NULL, 0, NULL, 0);
	char* theChar = new char[Length + 1];
	WideCharToMultiByte(theCode, 0, theCString, -1, theChar, Length, NULL, 0);
	theChar[Length] = 0;
	std::string theReturn(theChar);
	theString = theReturn;
}

void StringToCString(std::string theString, CString& theCString, UINT theCode = CP_ACP)
{
	int len = MultiByteToWideChar(theCode, 0, theString.c_str(), -1, NULL, 0);
	WCHAR* pCxml = new WCHAR[len + 1];
	len = MultiByteToWideChar(theCode, 0, theString.c_str(), theString.length(), pCxml, len);
	pCxml[len] = 0;
	CString tString(pCxml);
	theCString = tString;
	delete[] pCxml;
}

//获取CPU的数量
unsigned GetCpuCoreCount()  
{  
	unsigned count = 1; // 至少一个  
	SYSTEM_INFO si;  
	GetSystemInfo(&si);  
	count = si.dwNumberOfProcessors; 
	return count;  
}