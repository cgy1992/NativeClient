//element�����ã�����԰�������Ҫ�أ�element���������ߣ��棬Բ���Ժ����չ
//               ���ṩ�ӿڶ�Ҫ�ؽ������ӣ�ɾ�������صȵȲ�������Draw�н���Ҫ�صı������ֱ������Ⱦ
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
	CElement* GetElement(long index);						//��ȡҪ��
	void DeleteAllElement();								//ɾ������Ҫ��
	void Draw();											//��Ⱦ���ڲ��Ǳ�������Ҫ�أ����ֱ����Ҫ�ص�renderuseogles����
	long AddElement(CElement* elementObject);				//���ҪԪ��

private:
	std::map<long, CElement*> m_ElementMap;					//���ݼ�
	long m_ElementHandle_;							//���
};

