#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

std::map<TCPsocket, std::string> logs;
TCPsocket clientReceived = nullptr;

int main(int argc, char* argv[]) {
	char buffer[1024];
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
			int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
			cout << "Client " << buffer << " is connected !" << endl;
			logs[clientSocket] = buffer;

			for (auto client : clients) {
				if (client == clientSocket) break;
				std::string message = logs[client] + " is connected !";
				int bytesSent = SDLNet_TCP_Send(client, message.c_str(), message.length() + 1);
				if (bytesSent < message.length() + 1) {
					cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
					break;
				}
			}
			clientSocket = nullptr;
		}

		if (clients.size() != 0) {
			bool received = false;
			for (auto client : clients) {
				if(received) break;
				SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(clients.size());
				SDLNet_AddSocket(socketSet, reinterpret_cast<SDLNet_GenericSocket>(client));
				if (SDLNet_CheckSockets(socketSet, 10) > 0) {
					int bytesRead = SDLNet_TCP_Recv(client, buffer, sizeof(buffer));
					if (bytesRead > 0) {
						received = true;
						clientReceived = client;
						cout << "Incomming message -> Username : " << logs[client] << " --- Content : " << buffer << endl;

						/*if (strcmp(buffer, "exit") == 0) {
							SDLNet_TCP_Close(client);
							clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
							logs.erase(client);
							for (auto client : clients) {
								std::string message = logs[client] + " is disconnected !";
								int bytesSent = SDLNet_TCP_Send(client, message.c_str(), message.length() + 1);
								if (bytesSent < message.length() + 1) {
									cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
									break;
								}
							}
						} // missing this last part
						else received = true;
						*/
					}
				}
				SDLNet_FreeSocketSet(socketSet);
			}
			if (received) {
				for (auto client : clients) {
					if (client != clientReceived) {

						std::string message = logs[clientReceived]  + " : " + buffer;
						int bytesSent = SDLNet_TCP_Send(client, message.c_str(), message.length() + 1);
						if (bytesSent < message.length() + 1) {
							cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
							break;
						}
					}
				}
			}
		}
	}
	return 0;
}