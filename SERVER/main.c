#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <Winsock2.h>
#include <unistd.h>
#include <time.h>

#define FLAG_CLEAN 0x0
#define FLAG_EXIT 0x1

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

SOCKADDR_IN initSockAddrIn(int16_t family, uint16_t port, int64_t inetAddr)
{

	SOCKADDR_IN sin;
	memset(&sin, 0, sizeof(sin));

	sin.sin_family = family;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inetAddr;

	return sin;
}

void my_readSocketAndPrint(SOCKET s, char *buff, uint32_t buffSize, int16_t *flags)
{
	uint32_t n = 0;
	memset(buff, 0, buffSize);
	while ((n = recv(s, buff, buffSize - 1, 0)) > 0)
	{
		buff[n] = 0;

		if (!strncmp(buff, "exit", sizeof("exit") - 1) && flags) {
			*flags = FLAG_EXIT;
		}
		if(fputs(buff, stdout) == EOF)
		{
			printf("\n Error : Fputs error\n");
		}
		if (n < buffSize - 1)
			break;
	}
}

void my_bind(SOCKET s, SOCKADDR_IN sin)
{
	int32_t err = bind(s, (LPSOCKADDR)&sin, sizeof(sin));
	if (err < 0) {
		printf("bind () failed: %ld\n", GetLastError());
		exit(1);
	}
}

void my_listen(SOCKET masterSocket)
{
	if(listen(masterSocket, SOMAXCONN)) {
		printf("listen () failed: %ld\n", GetLastError());
		exit(1);
	}
}

void my_writeToSocketTimeStamp(SOCKET socket, char* buff, size_t buffSize)
{
	time_t ticks = time(NULL);
	snprintf(buff, buffSize, "Successful connection at %.24s\r\n", ctime(&ticks));
	send(socket, buff, (int32_t)strlen(buff), 0);
}

void my_loop(SOCKET masterSocket)
{
	char		buff[1025];
	SOCKET		s1;
	int16_t		flags = 0;
	SOCKADDR_IN	from;
	int32_t		fromlen = sizeof(from);

	while ((s1 = accept(masterSocket, (struct sockaddr *) &from, &fromlen)) > 0) {
		printf("Successful connection from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port));
		my_readSocketAndPrint(s1, (char *) &buff, sizeof(buff), &flags);
		my_writeToSocketTimeStamp(s1, (char*)&buff, sizeof(buff));
		close(s1);
		if (flags == FLAG_EXIT)
			break;
	}
}

int main()
{
	WSADATA	WsaData;
	setInitData(&WsaData);

	SOCKET masterSocket = initSocket();
	SOCKADDR_IN sin = initSockAddrIn(AF_INET, MY_PORT, htonl(INADDR_ANY));

	my_bind(masterSocket, sin);
	my_listen(masterSocket);
	my_loop(masterSocket);

	shutdown(masterSocket, SD_BOTH);
	WSACleanup();
	return(0);
}
