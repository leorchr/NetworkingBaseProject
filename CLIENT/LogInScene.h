#pragma once
#include "Scene.h"
#include "ChatScene.h"
#include <vector>
#include <raylib.h>
#include <string>
#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <functional>

class LogInScene : public Scene {
	public:
		LogInScene(std::function<void(Scene*)>changeScene);
		~LogInScene();
		bool init() override;
		void draw() override;
		void input() override;
		void update() override;
		void close() override;

	private:
		std::function<void(Scene*)> changeSceneFunction;
		std::string typing;
		const int width = 400, height = 400;
		std::string ipAdress;
		std::string port;
		std::string username;

		bool isTypingIp = false;
		bool isTypingPort = false;
		bool isTypingUsername = false;

		const int fontSize = 20;
		const int margin = 10;
		const int buttonHeight = 50;
		const int buttonWidth = width - 80;
};