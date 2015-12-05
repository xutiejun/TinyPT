#include <stdio.h>
#include "Config.h"
#include "Worker.h"
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
void test_Config() {
	Config config("tinypt.conf");
	printf("test_config:\n");
	printf("size = %d\n", config.MapItemSize());
	for (int i = 0; i < config.MapItemSize(); i++) {
		MapItem *p = config.GetItem(i);
		printf("%d %s %d\n", p->iLocalPort, p->sDestIP.c_str(), p->iDestPort);
	}
	printf("test_config finsh.");
}

void test() {
	printf("Start Run Test...\n");
	test_Config();
	printf("Test Done! Check the result to confirm program is allright");
}


int main() {

	Config config("tinypt.conf");

	std::vector<Worker *> vecWorkerList;
	for (int i = 0; i < config.MapItemSize(); i++) {
		Worker *pWorker = new Worker(config.GetItem(i));
		pWorker->Run();
	}
	
	while (1) {
        sleep(5);
		printf("System runing ...\n");
	}
	return 0;
}
