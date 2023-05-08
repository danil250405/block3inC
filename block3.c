#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <winsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <string.h>
#include <conio.h>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 4096 //makro, kde definujeme velkost prijimacieho buffera 
#define SCREEN_WIDTH 80


// data send with return AisId
int datasendingforaisid(int odpoved, SOCKET consocket, FILE* fw) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE); //Changing the color of the input to the console
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD point;


	if (fw == NULL)
	{
		printf("Error opening file.");
	}

	char sendbuf[4096]; //buffer (v zasade retazec), kam sa budu ukladat data, ktore chcete posielat
	int size = 4096, i = 0;
	char simvol;
	fprintf(fw, "ME: ");
	while (1)
	{
		simvol = _getch(); //Read by character from the console
		GetConsoleScreenBufferInfo(hConsole, &csbi); // --In these three lines we find the current cursor position in the console and assign a point--
		point.Y = csbi.dwCursorPosition.Y;
		point.X = csbi.dwCursorPosition.X;
		if (point.X == 0) //this if is so that if the user presses the backspace on line 2 or more it will jump to the previous line
		{
			point.X = 39;
			(point.Y)--;
			SetConsoleCursorPosition(hConsole, point);
		}
		else if (point.X == 39) // this if is for the user to enter characters up to the center of the console
		{
			point.X = 0;
			(point.Y)++;
			SetConsoleCursorPosition(hConsole, point);
		
		}
		if ((simvol == '\b'))  // this if is to prevent the user from writing it to the Sendbuf when the backspace is clicked
		{
			if (i != 0) //this if is to prevent the user from deleting characters already written in the console and to erase previous characters
			{
				printf_s("\b \b");
				i--;
			}
			continue;
		}
		//fixing the problem when pressing the arrows and tabs
		if (!(GetAsyncKeyState(VK_LEFT)) && !(GetAsyncKeyState(VK_DOWN)) && !(GetAsyncKeyState(VK_RIGHT)) && !(GetAsyncKeyState(VK_UP)) && !(GetAsyncKeyState(VK_TAB)))
		{
			printf("%c", simvol); //Here we display the symbol that the user pressed on the keyboard
			sendbuf[i] = simvol; //and if he doesn't, we'll write everything to the array
		
			i++;
		}

		if (simvol == '\r') //If the user presses the Enter key the loop will end
		{
			sendbuf[i] = '\0';
			point.X = 0;
			(point.Y)++;
			SetConsoleCursorPosition(hConsole, point);
			break;
		}



	}
	
	fprintf(fw, "%s", sendbuf);
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
	for (int i = 0; i < len; i++) //this is a loop to convert our aisId from an array to a single number of type int
	{
		num = num * 10 + (sendbuf[i] - '0');
	}
	return num;

}
//datareception for XOR
int datareceptionforxorsifr(int odpoved, SOCKET consocket, FILE *fw) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN); //Changing the color of the input to the console
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);//// --In this line we find the current cursor position in the console --
	COORD point;

	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN], proverka[13];
	//char* p;
	odpoved = recv(consocket, recvbuf, recvbuflen, 0);     //funkcia na príjimanie

	//p = strchr(recvbuf, '\n'); //calculate the end of the normal array and trim it with \0
	//recvbuf[p - recvbuf + 1] = '\0';

	point.X = 40;
	point.Y = csbi.dwCursorPosition.Y;
	SetConsoleCursorPosition(hConsole, point); //put the cursor in the center of the screen

	printf("Morpheus: \n");
	point.X = 40;
	(point.Y)++;
	SetConsoleCursorPosition(hConsole, point);

	int z = 0, line_len = 0, word_len = 0;
	//From observation, morpheus always says "try again" if you did something wrong,
	//so I read the last 13 characters of what morpheus outputs and if it says "try again" the function will return 1
	fprintf(fw, "MORPHEUS: ");
	for (int i = 0; i < 150; i++)
	{
		recvbuf[i] = recvbuf[i] ^ 55;
		if ((i + 13) >= 150) //read the last 13 characters of what morpheus outputs and i write this in array "proverka"
		{
			proverka[z] = recvbuf[i];
			z++;
		}
		//Vystup
		if (recvbuf[i] == ' ' || recvbuf[i] == '\t') //if the morpheus data element is a space or tabulator, then the word length is zero
		{
			if (((word_len + line_len + 1) > 40))  //if the length of the word + the length of the word already written in the string is more than forty,
				//then go to the next line and reset the length of the string to zero
			{
				(point.Y)++;
				point.X = 39;
				SetConsoleCursorPosition(hConsole, point);
				fprintf(fw, "\n");
				line_len = 0;
				if (recvbuf[i + 1] == ' ') //If the next item is a space, we won't print it on the next line.
				{
					i++;
				}
			}
			word_len = 0;
		}
		else //If the item is not a space, then add 1 to the word length. 
		{
			word_len++;
			if (recvbuf[i + 1] == ' ') //If the next item is a space, then we add 1 to the length of the string.
			{
				line_len++;
			}
		}
		putchar(recvbuf[i]);
		fprintf(fw, "%c", recvbuf[i]);
		//Sleep(1);                
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
	if ((strcmp(proverka, "Try again...") == 0) || (strcmp(proverka, "try again...") == 0) || (strcmp(proverka, "ry again....") == 0))
	{
		intproverka = 1; //If the user entered something incorrectly to the server, the function will return 1
	}
	else intproverka = 0;//if the user entered everything correctly on the server, the function will return 0
	return intproverka;
}


void datasending(int odpoved, SOCKET consocket, FILE *fw) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE); //Changing the color of the input to the console
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		COORD point;

	
		if (fw == NULL)
		{
			printf("Error opening file.");
		}

	char sendbuf[4096]; //buffer (v zasade retazec), kam sa budu ukladat data, ktore chcete posielat
	int size = 4096, i = 0;
	char simvol;
	fprintf(fw, "ME: ");
	while (1)
	{
		simvol = _getch(); //Read by character from the console
		GetConsoleScreenBufferInfo(hConsole, &csbi); // --In these three lines we find the current cursor position in the console and assign a point--
		point.Y = csbi.dwCursorPosition.Y; 
		point.X = csbi.dwCursorPosition.X; 
		if (point.X == 0) //this if is so that if the user presses the backspace on line 2 or more it will jump to the previous line
		{
			point.X = 39;
			(point.Y)--;
			SetConsoleCursorPosition(hConsole, point);
		}
		else if (point.X == 39) // this if is for the user to enter characters up to the center of the console
		{
			point.X = 0;
			(point.Y)++;
			SetConsoleCursorPosition(hConsole, point);
		}
		if ((simvol == '\b'))  // this if is to prevent the user from writing it to the Sendbuf when the backspace is clicked
		{
			if (i != 0) //this if is to prevent the user from deleting characters already written in the console and to erase previous characters
			{
				printf_s("\b \b");
				i--;
			}
			continue;
		}
		//fixing the problem when pressing the arrows and tabs
		if (!(GetAsyncKeyState(VK_LEFT)) && !(GetAsyncKeyState(VK_DOWN)) && !(GetAsyncKeyState(VK_RIGHT)) && !(GetAsyncKeyState(VK_UP)) &&!(GetAsyncKeyState(VK_TAB)))
		{
			printf("%c", simvol); //Here we display the symbol that the user pressed on the keyboard
			sendbuf[i] = simvol; //and if he doesn't, we'll write everything to the array
	
			i++;
		}
	
		if (simvol == '\r') //If the user presses the Enter key the loop will end
		{
			sendbuf[i] = '\0';
			point.X = 0;
			(point.Y)++;
			SetConsoleCursorPosition(hConsole, point);
			break;
		}
	}
	
	fprintf(fw, "%s", sendbuf);// print in file
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



void funcforALERT(char arr[])
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED); //Changing the color of the input to the console
	int size = strlen(arr);

	char* p = (char*)malloc((size + 1) * sizeof(char));

	if (p == NULL) {  // проверка, была ли выделена память
		printf("ERORR\n");
	}
	char letter;
	int q = 0;
	for (int i = 0; i < size; i++)
	{
		int w = i + 1;
		letter = arr[i];
		int is_prime = 1;
		for (int n = 2; n < w; n++)
		{
			if (w % n == 0 && n != w)
			{
				is_prime = 0;
				break;
			}

		}
		if (is_prime == 1 && w != 1)
		{
			p[q] = letter;
			q++;
			p[q + 1] = '\0';
		}
	}
	printf("Enter '%s': ", p);
	free(p);

}



int datareception(int odpoved, SOCKET consocket, int numberofcase, FILE* fw) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN); //Changing the color of the input to the console
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);//// --In this line we find the current cursor position in the console --
	COORD point;

	int recvbuflen = DEFAULT_BUFLEN ;
	char recvbuf[DEFAULT_BUFLEN], proverka [13];
	char* p;
	odpoved = recv(consocket, recvbuf, recvbuflen, 0);     //funkcia na príjimanie

	p = strchr(recvbuf, '\n'); //calculate the end of the normal array and trim it with \0
	recvbuf[p - recvbuf + 1] = '\0';

	point.X = 40;
	point.Y = csbi.dwCursorPosition.Y;
	SetConsoleCursorPosition(hConsole, point); //put the cursor in the center of the screen

	printf("Morpheus: \n");
	point.X = 40;
	(point.Y)++;
	SetConsoleCursorPosition(hConsole, point);
	fprintf(fw, "MORPHEUS: ");// writing in file
	int z = 0, line_len = 0, word_len = 0;
		//From observation, morpheus always says "try again" if you did something wrong,
		//so I read the last 13 characters of what morpheus outputs and if it says "try again" the function will return 1
	for (int i = 0; i < p - recvbuf + 1; i++)
	{
		if ((i + 13) >= (p - recvbuf + 1)) //read the last 13 characters of what morpheus outputs and i write this in array "proverka"
		{
			proverka[z] = recvbuf[i];
			z++;
		}
		//Vystup
		if (recvbuf[i] == ' ' || recvbuf[i] == '\t') //if the morpheus data element is a space or tabulator, then the word length is zero
		{
			if (((word_len + line_len + 1) > 40))  //if the length of the word + the length of the word already written in the string is more than forty,
												   //then go to the next line and reset the length of the string to zero
			{
				(point.Y)++;
				point.X = 39;
				SetConsoleCursorPosition(hConsole, point);
				line_len = 0;
				fprintf(fw, "\n");
				if (recvbuf[i + 1] == ' ') //If the next item is a space, we won't print it on the next line.
				{
					i++;
				}
			}
			word_len = 0;
		}
		else //If the item is not a space, then add 1 to the word length. 
		{
			word_len++;
			if ( recvbuf[i + 1] == ' ') //If the next item is a space, then we add 1 to the length of the string.
			{
				line_len++;
			}
		}

	putchar(recvbuf[i]);
	fprintf(fw, "%c", recvbuf[i]);
	//Sleep(1);                
	line_len++;
	}


	

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
	if (odpoved > 0)
		printf("Bytes received: %d\n", odpoved);     //prisli validne data, vypis poctu
	else if (odpoved == 0)
		printf("Connection closed\n");     //v tomto pripade server ukoncil komunikaciu
	else
		printf("recv failed with error: %d\n", WSAGetLastError());     //ina chyba
	if (numberofcase == 12)
	{
		funcforALERT(recvbuf);

	}
	proverka[12] = '\0';
	int intproverka;
	if ((strcmp(proverka, "Try again...") == 0) || (strcmp(proverka, "try again...") == 0) || (strcmp(proverka, "ry again....") == 0))
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
	return odpoved;
}





int main()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	FILE* fw = fopen("output.txt", "w");
	//size console
	COORD size = { 80, 40 };
	SetConsoleScreenBufferSize(hConsole, size);
	SMALL_RECT windowSize = { 0, 0, size.X - 1, size.Y - 1 };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
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

	for (int w = 0; 1; w++)
	{

		switch (w)
		{
		case 0:printf("Enter anything:");
			break;
		case 1:printf("\nEnter yours AIS ID: ");
			int aisid = datasendingforaisid(iResult, ConnectSocket, fw);
			break;
		case 2:printf("\nAsk him what he wants: ");
			break;
		case 3:printf("\nEnter 8484848 : ");
			break;
		case 4:printf("\nEnter 753422 : ");
			break;
		case 5:	printf("\nEnter the number %d ( this is the sum of\nthe first 5 numbers and the remainder\nof the division by the 5th digit of\nyour AISAID(123611) ) ", sucetadelenie(aisid));
			break;
		case 6:printf("\nEnter 333222333 : ");
			break;
		case 7:printf("\nEnter 123 : ");
			datasending(iResult, ConnectSocket, fw);
			datareceptionforxorsifr(iResult, ConnectSocket, fw);
			break;
		case 8:printf("In this coordinates Statue of liberty: ");
			break;
		case 9:printf("Enter '40': ");
			break;
		case 10:printf("Enter '-74': ");
			break;
		case 11:printf("Enter 'S.O.L.': ");
			break;
		case 12:printf("Enter 'PRIMENUMBER': ");
			break;
		case 13:printf("");
			break;
		case 14:printf("Enter 'Trinity': ");
			break;
		case 15:printf("Enter 'half-duplex': ");
			break;
		case 16:printf("Enter 'baud rate': ");
			break;
		case 17:return 0;
			break;
		default: printf("Something wrong((");
			break;
		}
		if (w!=1 && w!=7)
		{
			datasending(iResult, ConnectSocket, fw);
		}
		if (w!=7)
		{
			if (datareception(iResult, ConnectSocket, w, fw) == 1)
			{
				w--;
			}
		}
	}
	SetConsoleTextAttribute(hConsole, 7);
	closesocket(ConnectSocket);
	WSACleanup();
	fclose(fw);
	//-----------------------------uloha1---------------------------------------

	//---------------------------------------------------------------------
	//---------------------------------------------------------------------

	return 0;
}