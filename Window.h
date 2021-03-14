#pragma once
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Core/Timestep.h"
class Game;

class Window {
private:
	static GLFWwindow* m_Window;

	std::string m_Title;
	int m_Width;
	int m_Height;
    
	Game* m_Game;
  
public:
	Window(int width, int height, const std::string title);
    
	void Init();
	void Draw(Timestep timestep);
    
	static GLFWwindow* Get() { return m_Window; }
};