#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string.h>
#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 4096 //makro, kde definujeme velkost prijimacieho buffera
#define SCREEN_WIDTH 80


// data send with return AisId
int datasendingforaisid(int odpoved, SOCKET consocket) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
	char sendbuf[4096]; //buffer (v zasade retazec), kam sa budu ukladat data, ktore chcete posielat
	int size = 4096;
	fgets(sendbuf, size, stdin);
	odpoved = send(consocket, sendbuf, (int)strlen(sendbuf), 0);
	if (odpoved == SOCKET_ERROR)
	{

		printf("send failed: %d\n", WSAGetLastError());
		closesocket(consocket);
		WSACleanup();

	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	printf("Bytes Sent: %ld\n", odpoved);     //vypisanie poctu odoslanych dat


	
	int num = 0;
	int len = strlen(sendbuf) - 1; // вычисляем длину массива
	for (int i = 0; i < len; i++) {
		num = num * 10 + (sendbuf[i] - '0');
	}
	return num;

}



void datasending(int odpoved, SOCKET consocket) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		
		COORD point;
	char sendbuf[4096]; //buffer (v zasade retazec), kam sa budu ukladat data, ktore chcete posielat
	int size = 4096;
	char simvol, koniec;
	for ( int i = 0; ; i++)
	{
		GetConsoleScreenBufferInfo(hConsole, &csbi);
		point.Y = csbi.dwCursorPosition.Y;
		point.X = csbi.dwCursorPosition.X;
		if (point.X == 39)
		{
			point.X = 0;
			(point.Y)++;
			SetConsoleCursorPosition(hConsole, point);
		}
		simvol = (char)getch();
		//koniec = getchar();
		printf("%c", simvol);
		if ( simvol == '\r')
		{
			sendbuf[i] = '\0';
			point.X = 0;
			(point.Y)++;
			SetConsoleCursorPosition(hConsole, point);
			break;
		}
		else sendbuf[i] = simvol;
		//printf("%d", i);
	}
	//fgets(sendbuf, size, stdin);
	odpoved = send(consocket, sendbuf, (int)strlen(sendbuf), 0);
	if (odpoved == SOCKET_ERROR)
	{
		
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(consocket);
		WSACleanup();
		
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	printf("Bytes Sent: %ld\n", odpoved);     //vypisanie poctu odoslanych dat
}
int datareception(int odpoved, SOCKET consocket) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	//--------------
	
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	COORD point;

	
	//------------
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN], proverka [13];
	char* p;
	odpoved = recv(consocket, recvbuf, recvbuflen, 0);     //funkcia na príjimanie
	p = strchr(recvbuf, '\n');
	recvbuf[p - recvbuf + 1] = '\0';
	point.X = 40;
	point.Y = csbi.dwCursorPosition.Y;
	SetConsoleCursorPosition(hConsole, point);
	int line_len = 0,
		word_len = 0;
	printf("Morpheus: \n");
	point.X = 40;
	(point.Y)++;
	SetConsoleCursorPosition(hConsole, point);
	int z = 0;
	for (int i = 0; i < p - recvbuf + 1; i++)
	{
		//pole proverki
		if ((i + 13) >= (p - recvbuf + 1))
		{
			proverka[z] = recvbuf[i];
			z++;
		}
		//Vystup
		if (recvbuf[i] == ' ' || recvbuf[i] == '\t')
		{
			if (((word_len + line_len + 1) > 40)) {

				(point.Y)++;
				point.X = 39;
				SetConsoleCursorPosition(hConsole, point);
				line_len = 0;
				if (recvbuf[i + 1] == ' ')
				{
					i++;
				}
			}
			word_len = 0;
		}
		else {
			word_len++;
			if ( recvbuf[i + 1] == ' ')
			{
				line_len++;
			}
		}
	putchar(recvbuf[i]);
	Sleep(1);                
	line_len++;
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	if (odpoved > 0)
		printf("Bytes received: %d\n", odpoved);     //prisli validne data, vypis poctu
	else if (odpoved == 0)
		printf("Connection closed\n");     //v tomto pripade server ukoncil komunikaciu
	else
		printf("recv failed with error: %d\n", WSAGetLastError());     //ina chyba
	proverka[12] = '\0';
	int intproverka;
	if ((strcmp(proverka, "Try again...") == 0) || (strcmp(proverka, "try again...") == 0))
	{
		intproverka = 1; //If the user entered something incorrectly to the server, the function will return 1
	}
	else intproverka = 0;//if the user entered everything correctly on the server, the function will return 0
	return intproverka;
}
int sucetadelenie(int aisid) {
	int prva = aisid / 100000;
	int druha = (aisid / 10000) % 10;
	int tretia = (aisid / 1000) % 10;
	int stvrta = (aisid / 100) % 10;
	int piata = (aisid / 10) % 10;
	int sesta = aisid % 10;
	int odpoved;
	odpoved = (prva + druha + tretia + stvrta + piata) % piata;
	//printf ("%d", odpoved);
	//printf("1-%d 2-%d 3-%d 4-%d 5-%d 6-%d", prva, druha, tretia, stvrta, piata, sesta);
	return odpoved;
}



int main()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	//определенный размер консоли
	COORD size = { 80, 40 };
	SetConsoleScreenBufferSize(hConsole, size);
	SMALL_RECT windowSize = { 0, 0, size.X - 1, size.Y - 1 };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

	//int idais = 123611;
	//sucetadelenie(idais);
	SetConsoleTextAttribute(hConsole, 7);
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


	
	
	for (int w = 0; w < 20; w++)
	{

		switch (w)
		{
		case 0:printf("Enter anything:");
			break;
		case 1:printf("\nEnter yours AIS ID: ");
			int aisid = datasendingforaisid(iResult, ConnectSocket);
			//printf("%c", aisid);
			break;
		case 2:printf("\nAsk him what he wants: ");
			break;
		case 3:printf("\nEnter 8484848 : ");
			break;
		case 4:printf("\nEnter 753422 : ");
			break;
		case 5:	printf("\nEnter the number %d ( this is the sum of\nthe first 5 numbers and the remainder\nof the division by the 5th digit of\nyour AISAID(123611) ) ", sucetadelenie(aisid));



		default:
			break;
		
		}
		if (w!=1)
		{
			datasending(iResult, ConnectSocket);
		}
		if (GetAsyncKeyState(VK_F10)) return 0;
		if (datareception(iResult, ConnectSocket) == 1)
		{
			w--;
		}
	}
	//posielanie
	
	//datasending(iResult, ConnectSocket);
	//prijimanie
	//datareception(iResult, ConnectSocket);
	SetConsoleTextAttribute(hConsole, 7);
	while (1)
	{
	
	}
	/*//-------------------отправка AIS ID--------------------------
	printf("\nEnter yours AIS ID: ");
	datasending(iResult, ConnectSocket);
	datareception(iResult, ConnectSocket);
	//----------------------server want something-------------
	printf("\nAsk him what he wants: ");
	datasending(iResult, ConnectSocket);
	datareception(iResult, ConnectSocket);
	//---------------------------send some numbers(uloha with Sleep and UTF-8)------------------------------------------
	printf("\nEnter 8484848 : ");
	datasending(iResult, ConnectSocket);
	datareception(iResult, ConnectSocket);
	//---------------------------send some numbers(uloha with colors)------------------------------------------
	printf("\nEnter 753422 : ");
	datasending(iResult, ConnectSocket);
	datareception(iResult, ConnectSocket);
	//---------------------------sucet aisid a delenie------------------------------------------
	
	printf("\nEnter the number %d ( this is the sum of the first 5 numbers and the remainder of the division by the 5th digit of your AISAID(123611) ) ", sucetadelenie(idais));
	datasending(iResult, ConnectSocket);
	datareception(iResult, ConnectSocket);

	*/
	closesocket(ConnectSocket);
	WSACleanup();
	//-----------------------------uloha1---------------------------------------

	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
}