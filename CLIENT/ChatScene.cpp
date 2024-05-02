#include "ChatScene.h"
#include <cstring>
#include <iostream>
#include <new>
#include <ostream>
#include <string>
#include <vector>
#include "raylib.h"
#include "SDL_net.h"
using namespace std;

ChatScene::ChatScene(string ipAdressP, string portP, string usernameP) :
	ipAdress(ipAdressP),
	port(portP),
	username(usernameP)
{
	typing = "";
	clientSocket = nullptr;
}

ChatScene::~ChatScene() {}

bool ChatScene::init()
{
	if (SDLNet_Init() == -1) {
		cerr << "SDLNet_Init error: " << SDLNet_GetError() << endl;
		return false;
	}

	IPaddress ip = { stoi(ipAdress) };

	if (SDLNet_ResolveHost(&ip, "localhost", stoi(port)) == -1) {
		cerr << "SDLNet_ResolveHost error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return false;
	}

	clientSocket = SDLNet_TCP_Open(&ip);
	if (!clientSocket) {
		cerr << "TCP Open error: " << SDLNet_GetError() << endl;
		SDLNet_Quit();
		return false;
	}

	char buffer[1024];
	strcpy_s(buffer, username.c_str());
	int bytesSent = SDLNet_TCP_Send(clientSocket, buffer, sizeof(buffer));
	if (bytesSent < typing.length() + 1) {
		cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
		SDLNet_TCP_Close(clientSocket);
		SDLNet_Quit();
	}

	InitWindow(width, height, "Chat");
	SetTargetFPS(60);
	return true;
}

void ChatScene::draw() {
	BeginDrawing();
	ClearBackground(GRAY);
	DrawText("Welcome to ChArtFX!", 220, 15, 25, WHITE);
	DrawRectangle(20, 50, width - 40, height - 150, DARKGRAY);

	for (int msg = 0; msg < logs.size(); msg++)
	{
		if(logs[msg].username == "") DrawText(logs[msg].content.c_str(), 30, 75 + (msg * 30), 15, logs[msg].fromMe ? SKYBLUE : PURPLE);
		else DrawText((logs[msg].username + " : " + logs[msg].content).c_str(), 30, 75 + (msg * 30), 15, logs[msg].fromMe ? SKYBLUE : PURPLE);
	}
	DrawRectangle(20, height - 90, width - 40, 50, LIGHTGRAY);
	DrawText(typing.c_str(), 30, height - 75, 25, DARKBLUE);
	EndDrawing();
}

void ChatScene::input() {
	int inputChar = GetCharPressed();
	if (inputChar != 0) //A character is pressed on the keyboard
	{
		typing += static_cast<char>(inputChar);
	}
	if (typing.size() > 0)
	{
		if (IsKeyPressed(KEY_BACKSPACE)) typing.pop_back();
		else if (IsKeyPressed(KEY_ENTER))
		{
			//Send the message typing to the server here!
			logs.push_back(Message{ typing, username, true});
			send(typing);
			typing.clear();
		}
	}
}

void ChatScene::update() {

	//Receive the message from the server here!
	SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
	SDLNet_AddSocket(socketSet, reinterpret_cast<SDLNet_GenericSocket>(clientSocket));
	if (SDLNet_CheckSockets(socketSet, 0) != 0) {

		char buffer[1024];
		Message* receivedData = new Message("", "", false);
		int bytesContent = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
		receivedData->content = buffer;
		logs.push_back(*receivedData);
		cout << "Received: " << bytesContent << " bytes from the server !" << endl;
		cout << "Message received : " << receivedData->content << endl;
	}
	SDLNet_FreeSocketSet(socketSet);

	if(logs.size() > 19) logs.erase(logs.begin());
}

void ChatScene::send(string message) {
	char buffer[1024];
	strcpy_s(buffer, message.c_str());
	int bytesSent = SDLNet_TCP_Send(clientSocket, buffer, sizeof(buffer));
	if (bytesSent < typing.length() + 1) {
		cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
		SDLNet_TCP_Close(clientSocket);
		SDLNet_Quit();
	}
}

void ChatScene::close()
{
	send("exit");
	SDLNet_TCP_Close(clientSocket);
	SDLNet_Quit();
	CloseWindow();
}