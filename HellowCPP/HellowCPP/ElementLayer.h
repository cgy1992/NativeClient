//element层作用：层可以包含多种要素（element），比如线，面，圆及以后的扩展
//               层提供接口对要素进行增加，删除，隐藏等等操作，在Draw中进行要素的遍历并分别进行渲染
#pragma once
#include "Layer.h"
#include "LineElement.h"
#include "DataStruct.h"
class CElementLayer :
	public CLayer
{
public:
	CElementLayer(void);
	~CElementLayer(void);
	CElement* GetElement(long index);						//获取要素
	void DeleteAllElement();								//删除所有要素
	void Draw();											//渲染：内部是遍历各个要素，并分别调用要素的renderuseogles函数
	long AddElement(CElement* elementObject);				//添加要元素

private:
	std::map<long, CElement*> m_ElementMap;					//数据集
	long m_ElementHandle_;							//句柄
};

