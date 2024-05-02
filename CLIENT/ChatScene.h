#pragma once
#include "Scene.h"
#include <vector>
#include <raylib.h>
#include <string>
#include <SDL.h>
#include <SDL_net.h>
#include <iostream>

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

class ChatScene : public Scene {
public:
	ChatScene(std::string ipAdressP, std::string portP, std::string usernameP);
	~ChatScene();
	bool init();
	void draw() override;
	void input() override;
	void update() override;
	void send(std::string message);
	void close();
private:
	std::vector<Message> logs{ Message{"Waiting for someone to talk to...", "", false}};
	std::string typing;
	const int width = 500, height = 750;
	TCPsocket clientSocket;
	std::string ipAdress;
	std::string port;
	std::string username;
};