#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib,"ws2_32.lib")

using namespace std;

int main()
{
	// Winsock init
	WSAData data;
	WORD version = MAKEWORD(2, 2);

	int winsock = WSAStartup(version, &data);

	if (winsock != 0)
	{
		cerr << "Cannot initialize winsock ..." << endl;
		return 1;
	}



	SOCKET listeningSock = socket(AF_INET, SOCK_STREAM, 0);

	if (listeningSock == INVALID_SOCKET)
	{
		cerr << "Cannot create socket..." << endl;
		return 1;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listeningSock, (sockaddr*)&hint, sizeof(hint));


	listen(listeningSock, SOMAXCONN);

	clog << "Listening on port 54000 ..." << endl;

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listeningSock, (sockaddr*)&client, &clientSize);

	if (clientSocket == INVALID_SOCKET) {
		cerr << "client socket invalid ..." << endl;
		return 1;
	}

	char host[NI_MAXHOST];	// client	
	char service[NI_MAXSERV];	

	ZeroMemory(host, NI_MAXHOST); 
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}

	closesocket(listeningSock);

	
	char buffer[4096];

	while (true)
	{
		ZeroMemory(buffer, 4096);

		int bytesReceived = recv(clientSocket, buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << endl;
			break;
		}

		if (bytesReceived == 0)
		{
			cout << "Client disconnected " << endl;
			break;
		}

		cout << string(buffer, 0, bytesReceived) << endl;

		send(clientSocket, buffer, bytesReceived + 1, 0);

	}

	closesocket(clientSocket);
	WSACleanup();



}