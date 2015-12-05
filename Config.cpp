#include "Config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
Config::Config()
{
}


Config::~Config()
{
	for (size_t i = 0; i < m_vecMapItemList.size(); i++) {
		if (m_vecMapItemList[i] != NULL) {
			delete m_vecMapItemList[i];
			m_vecMapItemList[i] = NULL;
		}
	}
}

Config::Config(const std::string &sConfigFileName) {
	if (LoadFromFile(sConfigFileName) != 0) {
		printf("Open Config File Failed. File[%s]\n", sConfigFileName.c_str());
		exit(-1);
	}
}

int Config::LoadFromFile(const std::string &sFileName) {

	m_vecMapItemList.clear();

	FILE *pFile = fopen(sFileName.c_str(), "r");
	if (pFile == NULL) {
		printf("open config file[%s] failed\n error[%s]", sFileName.c_str(), strerror(errno));
		return -1;
	}

	char buf[4096];

	while (fgets(buf, sizeof(buf), pFile) != NULL) {
        printf("%s\n", buf);
		int iLen = strlen(buf);
		char *p = buf;
		
		//去掉字符串前面的空格和tab
		while (iLen != 0 && (*p == ' ' || *p == '\t' || *p == '\r')) {
			p++;
			iLen--;
		}

		if (iLen == 0 || *p == '#' || *p == '\n') {
			//空行 或者 注释
			continue;
		}

		if (iLen <= 10) {
			printf("Config file parsing failed\n");
			return -1;
		}

		MapItem *pNewMapItem = new MapItem();
		int ret = pNewMapItem->ParseFromString(p);
		if (ret != 0) {
			delete pNewMapItem;
			return -1;
		}
		
		m_vecMapItemList.push_back(pNewMapItem);
	}
    printf("MapItemSize: %d\n", m_vecMapItemList.size());
    for(int i = 0; i < m_vecMapItemList.size(); i++) {
        MapItem *a = m_vecMapItemList[i];
        printf("%d %s %d\n", a->iLocalPort, a->sDestIP.c_str(), a->iDestPort);
    }
	fclose(pFile);
	return 0;
}

int Config::MapItemSize() const {
	return static_cast<int> (m_vecMapItemList.size());
}

MapItem *Config::GetItem(int index) {
	if (index >= m_vecMapItemList.size()) {
		printf("index over size\n");
		return NULL;
	}
	return m_vecMapItemList[index];
}

//MapItem
int MapItem::ParseFromString(const std::string &str) {
	char buf[64];
	if (sscanf(str.c_str(), "%d%s%d", &iLocalPort, buf, &iDestPort) != 3) {
		printf("Parse From string[%s] Error\n", str.c_str());
		return -1;
	}
	sDestIP = std::string(buf);
	return 0;
}
