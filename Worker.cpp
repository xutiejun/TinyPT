#include "Worker.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>


class Context {
public:
	int toClient;
    struct sockaddr_in ClientAddr;
	int  toServer;
	struct sockaddr_in ServerAddr;

	Worker  *pWorker;
    Context() {
        memset(&ClientAddr, 0, sizeof(ClientAddr));
        memset(&ServerAddr, 0, sizeof(ServerAddr));
    }
};

void *ThreadProc(void *pParam) {
	((Worker *)pParam)->Do();
    return NULL;
}

void *ThreadWorker(void *pParam) {
	Context *pCtx = (Context *)pParam;
	pCtx->pWorker->Work(pCtx);
	delete pCtx;
    return NULL;
}

Worker::Worker(MapItem *p) : m_pMapItem(p) 
{

}


Worker::~Worker()
{
}

void Worker::Run() {
    pthread_create(&m_hThread, NULL, ThreadProc, (void *)this); 
}

void Worker::Do() {

	int skServer;
	if ((skServer = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Creat Socket Failed.\n");
		exit(-1);
	}

	struct sockaddr_in addrServer;
	memset(&addrServer, 0, sizeof(addrServer));
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(m_pMapItem->iLocalPort);
    addrServer.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(skServer, (sockaddr *)&addrServer, sizeof(addrServer)) == -1) {
        printf("Bind Error. error:%s\n", strerror(errno));
		exit(-1);
	}

	if (listen(skServer, 5) == -1) {
		printf("Listen Error\n");
		exit(-1);
	}

    printf("this: %p\n", this);
    printf("local: %d dest: %s %d\n",m_pMapItem->iLocalPort ,m_pMapItem->sDestIP.c_str(), m_pMapItem->iDestPort);
	printf("Server is listening port[%d]\n", m_pMapItem->iLocalPort);

	while (1) {
		
		Context *pCtx = new Context();
		socklen_t iAddrLen = sizeof(struct sockaddr_in);
		pCtx->pWorker = this;
		if ((pCtx->toClient = accept(skServer, (sockaddr *)&pCtx->ClientAddr, &iAddrLen)) == -1) {
			printf("Accept Error\n");
			exit(-1);
		}

		printf("Accept from %s.\n", inet_ntoa(pCtx->ClientAddr.sin_addr));

        pthread_t hThread;
        pthread_create(&hThread, NULL, ThreadWorker, pCtx);
		
	}

}

void Worker::Work(Context *pCtx) {

    printf("this: %p\n", this);
    printf("local: %d dest: %s %d\n",m_pMapItem->iLocalPort ,m_pMapItem->sDestIP.c_str(), m_pMapItem->iDestPort);
	int skClientToDest = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in addrClientToDest;
	memset(&addrClientToDest, 0, sizeof(addrClientToDest));
	addrClientToDest.sin_family = AF_INET;
	addrClientToDest.sin_port = htons(m_pMapItem->iDestPort);
	addrClientToDest.sin_addr.s_addr= inet_addr(m_pMapItem->sDestIP.c_str());
	int iClientToDestAddrLen = sizeof(addrClientToDest);

	//connect dest server
    printf("connecting...\n");
	if (connect(skClientToDest, (sockaddr *)&addrClientToDest, iClientToDestAddrLen) == -1) {
		printf("Connect to Dest Server Failed. IP[%s] PORT[%d]\n", m_pMapItem->sDestIP.c_str(), m_pMapItem->iDestPort);
		return;
	}

    printf("connect success\n");
	
	char buf[4097];
	const int BUF_SIZE = 4096;
	while (1) {

		fd_set setCanRead;
		FD_ZERO(&setCanRead);
		FD_SET(pCtx->toClient, &setCanRead);
		FD_SET(skClientToDest, &setCanRead);
		if (select(1024, &setCanRead, NULL, NULL, NULL) != -1) {
			int iBytes = 0;

			if (FD_ISSET(pCtx->toClient, &setCanRead)) {

				//read from client
				if ((iBytes = recv(pCtx->toClient, buf, BUF_SIZE, 0)) == -1) {
					printf("Recv Error\n");
					break;
				}

				if (iBytes == 0) break;

				buf[iBytes] = '\0';

				//printf("Recv Data from Client[%s]:\n%s", inet_ntoa(pCtx->ClientAddr.sin_addr),  buf);

				if ((iBytes = send(skClientToDest, buf, iBytes, 0)) == -1) {
					printf("Send Error\n");
					break;
				}
			}

			if (FD_ISSET(skClientToDest, &setCanRead)) {
				//read from server
				
				if ((iBytes = recv(skClientToDest, buf, BUF_SIZE, 0)) == -1) {
					printf("Recv from Dest Server ERROR\n");
					break;
				}

				if (iBytes == 0) break;

				buf[iBytes] = '\0';

				//printf("Recv Data from Client[%s]:\n%s", inet_ntoa(addrClientToDest.sin_addr),  buf);


				//send to client

				if ((iBytes = send(pCtx->toClient, buf, iBytes, 0)) == -1) {
					printf("Send To Client ERROR\n");
					break;
				}
			}
		}
		else {
			printf("Select Error\n");
			break;
		}

	}

	close(skClientToDest);
	close(pCtx->toClient);
}
