#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string.h>
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 4096 //makro, kde definujeme velkost prijimacieho buffera
void datasending(int odpoved, SOCKET consocket) {
	char sendbuf[4096]; //buffer (v zasade retazec), kam sa budu ukladat data, ktore chcete posielat
	int size = 4096;
	fgets(sendbuf, size, stdin);
	//printf("%s\n", sendbuf);
	odpoved = send(consocket, sendbuf, (int)strlen(sendbuf), 0);
	if (odpoved == SOCKET_ERROR)
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(consocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", odpoved);     //vypisanie poctu odoslanych dat


}
void datareception(int odpoved, SOCKET consocket) {
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];
	char* p;
	odpoved = recv(consocket, recvbuf, recvbuflen, 0);     //funkcia na príjimanie
	p = strchr(recvbuf, '\n');
	recvbuf[p - recvbuf + 1] = '\0';
	for (int i = 0; i < p - recvbuf + 1; i++)
	{
		printf("%c", recvbuf[i]);
		Sleep(20);
	}
	//printf("%s", recvbuf);
	if (odpoved > 0)
		printf("Bytes received: %d\n", odpoved);     //prisli validne data, vypis poctu
	else if (odpoved == 0)
		printf("Connection closed\n");     //v tomto pripade server ukoncil komunikaciu
	else
		printf("recv failed with error: %d\n", WSAGetLastError());     //ina chyba

}




int main()
{
	SetConsoleOutputCP(CP_UTF8);
	//uvodne nastavovacky
	WSADATA wsaData;    //struktura WSADATA pre pracu s Winsock
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);     //zakladna inicializacia
	if (iResult != 0)     //kontrola, ci nestala chyba
	{
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	struct addrinfo* result = NULL, * ptr = NULL;     //struktura pre pracu s adresami
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;     //pracujeme s protokolom TCP/IP

	// Resolve the server address and port
	iResult = getaddrinfo("147.175.115.34", "777", &hints, &result);
	if (iResult != 0)     //kontrola, ci nenastala chyba
	{
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	else {
		printf("getaddrinfo didnt fail…\n");
	}
	//vytvorenie socketu a pripojenie sa

	SOCKET ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server => pokus o vytvorenie socketu
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET)     //kontrola, ci nenastala chyba
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	else
		printf("Error at socket DIDNT occur…\n");

	// Connect to server. => pokus o pripojenie sa na server

	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)     //kontrola, ci nenastala chyba
		printf("Not connected to server…\n");
	else
		printf("Connected to server!\n");

	if (iResult == SOCKET_ERROR)    //osetrenie chyboveho stavu
	{
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		WSACleanup();
		return 1;
	}

	Sleep(250);

	//posielanie
	printf("Enter anything:");
	datasending(iResult, ConnectSocket);
	//prijimanie
	datareception(iResult, ConnectSocket);


	//-------------------отправка AIS ID--------------------------
	printf("\nEnter yours AIS ID: ");
	datasending(iResult, ConnectSocket);
	datareception(iResult, ConnectSocket);
	//----------------------server want something-------------
	printf("\nAsk him what he wants: ");
	datasending(iResult, ConnectSocket);
	datareception(iResult, ConnectSocket);

	closesocket(ConnectSocket);
	WSACleanup();
	//-----------------------------uloha1---------------------------------------

	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
}