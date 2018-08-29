#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <queue>
#include <mutex>
#define BUFFERSIZE 150
using namespace std;
class udpServr
{
public:
	static udpServr* pInstance;
	void vSetup();
	static udpServr* getInstance();
	~udpServr();
	void vExit();
	void testIt();
	void vInitUdpServer();
	std::string vRecvUDP();
	std::queue<std::string> udp_queue;
	std::mutex mutexUDPQueue;
	

private:
	static std::mutex mutexInternal;
	udpServr();
	int sd, rc;
	struct sockaddr_in serveraddr, clientaddr;
	char buffer[BUFFERSIZE];
	socklen_t  clientaddrlen;
	int serveraddrlen;
	char *bufptr;
	int buflen;
};

