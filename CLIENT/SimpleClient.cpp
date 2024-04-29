#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
//#include <string>
using namespace std;

int main(int argc, char* argv[]) {

	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return 1;
	}

	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, "localhost", 4242) == -1) {
		cerr << "SDLNet_ResolveHost error: " << SDLNet_GetError() << endl;
	}

	TCPsocket clientSocket = SDLNet_TCP_Open(&ip);
	if (!clientSocket) {
		cerr << "TCP Open error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return 1;
	}

	string message = "Hello there !";

	int bytesSent = SDLNet_TCP_Send(clientSocket, message.c_str(), message.length() + 1);
	if (bytesSent < message.length() + 1) {
		cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
		SDLNet_TCP_Close(clientSocket);
		SDLNet_Quit();
		return 1;
	}

	cout << "Sent: " << bytesSent << " bytes to the server !" << endl;

	char buffer[1024];
	int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
	if (bytesRead <= 0) {
		cerr << "SDL Net TCP Recv error: " << SDLNet_GetError() << endl;
		SDLNet_TCP_Close(clientSocket);
		SDLNet_Quit();
		return 1;
			
	}
	else {
		cout << "Received: " << bytesRead << " bytes from the server !" << endl;
		cout << "Message: " << buffer << endl;
	}

	SDLNet_TCP_Close(clientSocket);
	SDLNet_Quit();
	return 0;
}
