#pragma once

class Scene {
	public:
		Scene();
		~Scene();
		virtual bool init();
		virtual void draw();
		virtual void input();
		virtual void update();
		virtual void close();
};