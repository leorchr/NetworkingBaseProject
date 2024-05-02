#include "ChatScene.h"
#include "LogInScene.h"
#include <SDL.h>
#include <SDL_net.h>
#include <raylib.h>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
using namespace std;

void changeScene(Scene* scene);


//ChatScene* chat = new ChatScene();ChatScene* chat = new ChatScene();ChatScene* chat = new ChatScene();
Scene* currentScene = new LogInScene(changeScene);;



int main(int argc, char* argv[]) {

	if (currentScene->init()) {
		cout << "Login initialized" << endl;
		while (!WindowShouldClose()) {
			currentScene->input();
			currentScene->update();
			currentScene->draw();
		}
		currentScene->close();
	}
	else {
		cerr << "Scene failed to initialize" << endl;
	}
	return 0;
}

void changeScene(Scene* scene) {
	currentScene->close();
	currentScene = scene;
	currentScene->init();
	cout << "Scene changed" << endl;
}