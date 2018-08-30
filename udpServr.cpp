#include "udpServr.h"
#include <thread>
#include <string> 
#include <iostream> 
#include <chrono>
using namespace std;
using namespace chrono;
/* Server's port number, listen at 1234 . */
#define SERVPORT 1234
#define NO_MESSAGE_RECEIVED_YET 0
#define MIN_TIME_BETWEEN_IDENTICAL_MESSAGES_MS 600
#define RING_BUFFER_SIZE_OF_OLD_MESSAGES 15
udpServr* udpServr::pInstance = nullptr;
std::mutex udpServr::mutexInternal;
udpServr::udpServr() : llTimestampOfPreviouslyReceivedMessage(NO_MESSAGE_RECEIVED_YET), strCopyOfReceive(std::to_string(NO_MESSAGE_RECEIVED_YET))
{
	vSetup();
}

void udpServr::vSetup()
{
	/* Variable and structure definitions. */
	clientaddrlen = sizeof(clientaddr);
	serveraddrlen = sizeof(serveraddr);

	bufptr = buffer;
	buflen = sizeof(buffer);

	/* The socket() function returns a socket */
	/* descriptor representing an endpoint. */
	/* The statement also identifies that the */
	/* INET (Internet Protocol) address family */
	/* with the UDP transport (SOCK_DGRAM) will */
	/* be used for this socket. */
	/******************************************/
	/* get a socket descriptor */
	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("UDP server - socket() error");
		exit(-1);
	}
	else
	{
		printf("UDP server - socket() is OK\n");
	}


	printf("UDP server - try to bind...\n");

	/* After the socket descriptor is received, */
	/* a bind() is done to assign a unique name */
	/* to the socket. In this example, the user */
	/* set the s_addr to zero. This allows the */
	/* system to connect to any client that uses */
	/* port 1234. */
	/********************************************/

	/* bind to address */
	memset(&serveraddr, 0x00, serveraddrlen);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVPORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((rc = bind(sd, (struct sockaddr *)&serveraddr, serveraddrlen)) < 0)
	{
		perror("UDP server - bind() error");
		close(sd);
		/* If something wrong with socket(), just exit lol */
		exit(-1);
	}
	else
	{
		printf("UDP server - bind() is OK\n");
	}

	printf("Using IP %s and port %d\n", inet_ntoa(serveraddr.sin_addr), SERVPORT);

}
udpServr * udpServr::getInstance()
{
	mutexInternal.lock();
	if (!pInstance)
	{
		pInstance = new udpServr();
	}
	mutexInternal.unlock();
	return pInstance;
}

udpServr::~udpServr()
{
}
void udpServr::vExit()
{
	close(sd);
	sd = -1;
}

std::string udpServr::vRecvUDP()
{
		
		

		memset(&bufptr[0], 0, buflen);
		/* Wait on client requests. */
		rc = recvfrom(sd, bufptr, buflen, 0, (struct sockaddr *)&clientaddr, &clientaddrlen);
		if (rc < 0)
		{
			perror("UDP Server - recvfrom() error");
			close(sd);
			exit(-1);
		}
		else
		{
			//printf("UDP Server - recvfrom() is OK...\n");
			// DAS IST NUR DRIN WENN UNTIGES DEBUG DEAKTVIERT IST!!!
			//strCopyOfReceive = (std::string)bufptr;
			//return strCopyOfReceive;
		}


		// Push the message in the vector
		long long llCurrentTimestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		if (!vecpairMessageComparer.empty())
		{
			if (vecpairMessageComparer.size() >= RING_BUFFER_SIZE_OF_OLD_MESSAGES)
			{
				vecpairMessageComparer.erase(vecpairMessageComparer.begin());
			}
		}

		vecpairMessageComparer.push_back(std::make_pair(llCurrentTimestamp, (std::string)bufptr));

		// is the message already in the vector and in a critical distnace?
		if (bMessageIsSecurityDoublicate(llCurrentTimestamp, bufptr))
		{
			strCopyOfReceive = IGNORE_MESSAGE_INDICATOR;
		}
		else
		{
			strCopyOfReceive = (std::string)bufptr;
		}
		return strCopyOfReceive;
}

bool udpServr::bMessageIsSecurityDoublicate(long long &llCurrentTimestamp, char* pcMsg)
{
	bool bIsJustADuplicate = false;
	// Delete all messages which are older than  MIN_TIME_BETWEEN_IDENTICAL_MESSAGES_MS
	while ((llCurrentTimestamp - vecpairMessageComparer.front().first) > MIN_TIME_BETWEEN_IDENTICAL_MESSAGES_MS)
	{
		vecpairMessageComparer.erase(vecpairMessageComparer.begin());
	}

	// now check for duplicates
	for (int i = 0; i < vecpairMessageComparer.size(); i++)
	{
		if (vecpairMessageComparer.at(i).second.compare((std::string) pcMsg) == 0)
		{
			// Theese strings are identical
			if (llCurrentTimestamp != vecpairMessageComparer.at(i).first)
			{
				// And the timestamps are not the same -> We saw this exact message already! Discard it!
				bIsJustADuplicate = true;
				return bIsJustADuplicate;
			}
		}
	}
	return bIsJustADuplicate;
}


void udpServr::testIt()
{
	/* Use the recvfrom() function to receive the */
	/* data. The recvfrom() function waits */
	/* indefinitely for data to arrive. */
	/************************************************/
	/* This example does not use flags that control */
	/* the reception of the data. */
	/************************************************/
	/* Wait on client requests. */
	int rc;
	rc = recvfrom(sd, bufptr, buflen, 0, (struct sockaddr *)&clientaddr, &clientaddrlen);
	if (rc < 0)
	{
		perror("UDP Server - recvfrom() error");
		close(sd);
		exit(-1);
	}
	else
	{
		printf("UDP Server - recvfrom() is OK...\n");
	}
	printf("UDP Server received the following:\n \"%s\" message\n", bufptr);
	printf("from port %d and address %s.\n", ntohs(clientaddr.sin_port),inet_ntoa(clientaddr.sin_addr));


	/* Send a reply by using the sendto() function. */
	/* In this example, the system echoes the received */
	/* data back to the client. */
	/************************************************/
	/* This example does not use flags that control */
	/* the transmission of the data */
	/************************************************/
	/* Send a reply, just echo the request */
	printf("UDP Server replying to the stupid UDP client...\n");
	rc = sendto(sd, bufptr, buflen, 0, (struct sockaddr *)&clientaddr, clientaddrlen);
	if (rc < 0)
	{
		perror("UDP server - sendto() error");
		close(sd);
		exit(-1);
	}
	else 
	{
		printf("UDP Server - sendto() is OK...\n");
	}
		
	/* When the data has been sent, close() the */
	/* socket descriptor. */
	/********************************************/
	/* close() the socket descriptor. */
	//close(sd);
	//exit(0);
}

