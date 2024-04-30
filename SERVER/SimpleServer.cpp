#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <vector>
using namespace std;

struct Message
{
public:
	Message(std::string contentP, std::string usernameP, bool fromMeP) :
		content(contentP),
		username(usernameP),
		fromMe(fromMeP)
	{}

	std::string content = "";
	std::string username = "";
	bool fromMe = false;
};

void serialize(Message* data, char* buffer);
void deserialize(char* buffer, Message* data);

int main(int argc, char* argv[]) {

	char buffer[sizeof(Message)];
	memset(buffer, 0, sizeof(Message));

	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return 1;
	}

	cout << "Thank you for using ChArtFX !\n";
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, nullptr, 4242) == -1) {
		cerr<< "Resolve Host error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return 1;
	}

	TCPsocket serverSocket = SDLNet_TCP_Open(&ip);
	if (!serverSocket) {
		cerr << "TCP Open error : " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return 1;
	}
	
	TCPsocket clientSocket = SDLNet_TCP_Accept(serverSocket);
	vector<TCPsocket> clients = {};
	while (true) {
		if (!clientSocket) {
			clientSocket = SDLNet_TCP_Accept(serverSocket);
		}
		else {
			clients.push_back(clientSocket);
			cout << "A new client has connected !" << endl;
			clientSocket = nullptr;
		}
		if (clients.size() != 0) {
			bool received = false;
			for (auto client : clients) {
				if(received) break;
				SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(clients.size());
				SDLNet_AddSocket(socketSet, reinterpret_cast<SDLNet_GenericSocket>(client));
				if (SDLNet_CheckSockets(socketSet, 10) > 0) {
					
					int bytesRead = SDLNet_TCP_Recv(client, buffer, sizeof(Message));
					if (bytesRead > 0) {
						received = true;
						Message* receivedData = new Message("","", false);
						deserialize(buffer, receivedData);
						cout << "Incomming message -> Username : " << receivedData->username << " --- Content : " << receivedData->content << endl;
					}
				}
				SDLNet_FreeSocketSet(socketSet);
			}
			if (received) {
				for (auto client : clients) {
					int bytesSent = SDLNet_TCP_Send(client, buffer, sizeof(Message));
					if (bytesSent < sizeof(Message)) {
						cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
						break;
					}
				}
			}
		}
	}
	return 0;
}

// Serialize function
void serialize(Message* data, char* buffer) {
	memcpy(buffer, data, sizeof(Message));
}

// Deserialize function
void deserialize(char* buffer, Message* data) {
	memcpy(data, buffer, sizeof(Message));
}
