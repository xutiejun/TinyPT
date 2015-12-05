#pragma once
#include <string>
#include <vector>

class MapItem {
public:
	MapItem() {}
	~MapItem() {}

	int ParseFromString(const std::string &str);

	int iLocalPort;             //本地监听端口
	std::string sDestIP;		  //目的地址
	int iDestPort;			  //目的端口
};

class Config
{
public:
	Config();
	Config(const std::string &sConfigFileName);
	~Config();

	int LoadFromFile(const std::string &sFileName);
	int MapItemSize() const;
	MapItem *GetItem(int index);

private:
	int m_iLogLevel;
	std::vector<MapItem*> m_vecMapItemList;

};

