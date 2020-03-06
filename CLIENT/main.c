#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <Winsock2.h>
#include <io.h>

#define MY_PORT 5000
#define MY_SERVER_IP "127.0.0.1"

void setInitData(WSADATA *WsaData)
{
	int32_t err = WSAStartup(0x0101, WsaData);
	if (err == SOCKET_ERROR) {
		printf("WSAStartup () failed: %ld\n", GetLastError());
		exit(1);
	}
}

SOCKET initSocket()
{
	int32_t s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s < 0) {
		printf("socket () failed: %ld\n", GetLastError());
		exit(1);
	}
	return ((SOCKET)s);
}

SOCKADDR_IN initSockAddrIn(int16_t family, uint16_t port, const char *inetAddr)
{
	SOCKADDR_IN sin;
	memset(&sin, 0, sizeof(sin));

	sin.sin_family = family;
	sin.sin_port = htons(port);
	sin.sin_addr.S_un.S_addr = inet_addr(inetAddr);

	return sin;
}

void my_readSocketAndPrint(SOCKET s, char *recvBuff, uint32_t buffSize)
{
	uint32_t n = 0;
	while ( (n = recv(s, recvBuff, buffSize - 1, 0)) > 0)
	{
		recvBuff[n] = 0;
		if(fputs(recvBuff, stdout) == EOF)
		{
			printf("\n Error : Fputs error\n");
		}
		if (n < buffSize - 1)
			break;
	}
}

void my_connect(SOCKET s, SOCKADDR_IN sin)
{
	if (connect(s, (struct sockaddr*)&sin, sizeof(struct sockaddr)) < 0) {
		printf("connect () failed \n");
		exit(1);
	}
}

void my_writeToSocket(SOCKET socket, char* buff, size_t buffSize, const char* message)
{
	snprintf(buff, buffSize, "%s\r\n", message);
	send(socket, buff, (int32_t)strlen(buff), 0);
}


int32_t main(int32_t argc, char **argv)
{
	char	buff[1024];
	WSADATA	WsaData;
	setInitData(&WsaData);

	SOCKET	masterSocket = initSocket();
	//SOCKADDR_IN sin = initSockAddrIn(AF_INET, 80, "216.239.36.117");
	SOCKADDR_IN sin = initSockAddrIn(AF_INET, MY_PORT, MY_SERVER_IP);

	my_connect(masterSocket, sin);
	my_writeToSocket(masterSocket, buff, sizeof(buff), (argc == 2) ? argv[1] : argv[0]);
	my_readSocketAndPrint(masterSocket, buff, sizeof(buff));
	shutdown(masterSocket, SD_BOTH);
	close(masterSocket);
	return (0);
}
