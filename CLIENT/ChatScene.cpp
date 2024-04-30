#include "ChatScene.h"
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
			if(logs.size() > 10) logs.erase(logs.begin());
			send(typing);
		}
	}
}

void ChatScene::update() {

	//Receive the message from the server here!
	SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
	SDLNet_AddSocket(socketSet, reinterpret_cast<SDLNet_GenericSocket>(clientSocket));
	if (SDLNet_CheckSockets(socketSet, 0) != 0) {

		char buffer[sizeof(Message)];
		int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(Message));
		if (bytesRead > 0) {

			Message* receivedData = new Message("", "", false);;
			deserialize(buffer, receivedData);

			char* typingArray = new char[typing.length() + 1];
			strcpy_s(typingArray, typing.length() + 1, typing.c_str());
			if (!(strcmp(receivedData->content.c_str(), typingArray) == 0)) {
				receivedData->fromMe = false;
				logs.push_back(*receivedData);
				cout << "Received: " << bytesRead << " bytes from the server !" << endl;
				cout << "Username: " << receivedData->username << " --- Message :" << receivedData->content << endl;
			}
		}
		typing.clear();
	}
	SDLNet_FreeSocketSet(socketSet);
}

void ChatScene::send(string message) {
	Message msg = Message{ message, username, false };
	char buffer[sizeof(Message)];
	serialize(&msg, buffer);

	int bytesSent = SDLNet_TCP_Send(clientSocket, buffer, sizeof(Message));
	if (bytesSent < typing.length() + 1) {
		cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << endl;
		SDLNet_TCP_Close(clientSocket);
		SDLNet_Quit();
	}
}

void ChatScene::close()
{
	SDLNet_TCP_Close(clientSocket);
	SDLNet_Quit();
	CloseWindow();
}

// Serialize function
void ChatScene::serialize(Message* data, char* buffer) {
	memcpy(buffer, data, sizeof(Message));
}

// Deserialize function
void ChatScene::deserialize(char* buffer, Message* data) {
	memcpy(data, buffer, sizeof(Message));
}
