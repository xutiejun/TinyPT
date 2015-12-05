#pragma once
#include "Config.h"
#include <pthread.h>

class Context;

class Worker
{
public:
	Worker(MapItem *p);
	~Worker();
	void Run();
	void Do();
	void Work(Context *pCtx);

private:
	MapItem *m_pMapItem;
	pthread_t m_hThread;
	int m_dwThreadId;
};

