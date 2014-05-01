#pragma once
#include "tinyxml\tinyxml.h"
#include <string>
TiXmlElement* GetNodePointerByName(TiXmlElement* pRootEle, const std::string &strNodeName)   
{    
	// ������ڸ��ڵ��������˳�    
	std::string theValue;
	theValue = pRootEle->Value();
	if (strNodeName.compare(theValue) == 0)     
		return pRootEle;    

	TiXmlElement* pEle = pRootEle;      
	for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())      
	{      
		//�ݹ鴦���ӽڵ㣬��ȡ�ڵ�ָ��  
		TiXmlElement* theElement = GetNodePointerByName(pEle, strNodeName);
		if (theElement)
		{
			return theElement;
		}
	} 
	return NULL;
} 