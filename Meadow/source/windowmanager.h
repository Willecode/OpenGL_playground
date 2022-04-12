#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include "renderer/renderer.h"
#include <string>
/*
* Manages one window
*/
class WindowManager
{
	typedef void (*GLProc)(void);
public:
	static float width;
	static float height;
public:
	WindowManager();
	~WindowManager();
	bool createWindow(std::string title);
	bool shouldClose();
	void swapBuffers();
	GLProc getProcAddress();
	GLFWwindow* getWindow();
	float getTime();
	void pollEvents();
private:
	GLFWwindow* m_window;
private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};

