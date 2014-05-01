#pragma once 
#include "BaseLayer.h"
#include "Tile.h"
#include "XmlHelp.h"
class CArcgisMapLayer
	: public CBaseLayer
{
public:
	CArcgisMapLayer()
	{
	}
	CArcgisMapLayer(const double& cenX, const double& cenY, const int& js, 
		const std::string& mapname, const std::string& path, const std::string& pictype,
		int ViewHeight, int ViewWidth)
		: CBaseLayer(cenX, cenY, js, mapname, path, pictype, ViewHeight, ViewWidth)
	{
		m_MapType = MapType::ArcgisMap;
		m_ReadConfig_ = false;
	}
	~CArcgisMapLayer()
	{
		//ɾ����Ƭ����
		std::map<long, Tile*>::iterator theBeg = m_TileStore_.begin();
		std::map<long, Tile*>::iterator theEnd = m_TileStore_.end();
		while (theBeg != theEnd)
		{
			delete theBeg->second;
			theBeg++;
		}
		m_TileStore_.clear();
	}
	//���Ƶ�ͼ
	void Draw();
	//��ȡ��ͼ����
	void GetMapData(const int& js, const double& midX, const double& midY		//��ǰ�����߼������ĵ�
		, const int& windowWidth, const int& windowHeight);
	//��ȡ��ͼ������Ϣ
	void GetMapConfig(std::string ConfigPath);
	//��ȡ��Ƭ������  �����߼��� ���ĵ�ľ�γ��
	void GetMapTile(const int& js, const double& midX, const double& midY		//��ǰ�����߼������ĵ�
					, const int& windowWidth, const int& windowHeight);	//���ڵĿ��

	

private:
	//���Ͻ�X Y
	double m_TileOriginX_;
	double m_TileOriginY_;
	
	//DPI ÿӢ���������
	int m_DPI_;
	bool m_ReadConfig_;			//�Ƿ��Ѿ���ȡ�������ļ�

	//ͼƬ��Ƭ�Ĵ洢λ��  
	//����  ��Ƭ
	std::map<long, Tile*> m_TileStore_;
};