#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>


class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	GLfloat getrotay() { return rotay; }
	GLfloat getrotax() { return rotax; }
	GLfloat getrotaz() { return rotaz; }
	GLfloat getarticulacion1() { return articulacion1; }
	GLfloat getarticulacion2() { return articulacion2; }
	GLfloat getarticulacion3() { return articulacion3; }
	GLfloat getarticulacion4() { return articulacion4; }
	GLfloat getarticulacion5() { return articulacion5; }
	GLfloat getarticulacion6() { return articulacion6; }
	
	//ejercicio 1 grua
	/*
	GLfloat getrueda1() { return rueda1; }
	GLfloat getrueda2() { return rueda2; }
	GLfloat getrueda3() { return rueda3; }
	GLfloat getrueda4() { return rueda4; }
	*/

	//ejercicio2 gato
	GLfloat getart1() { return art1; }
	GLfloat getart2() { return art2; }
	GLfloat getart3() { return art3; }
	GLfloat getart4() { return art4; }
	GLfloat getart5() { return art5; }
	GLfloat getart6() { return art6; }
	GLfloat getart7() { return art7; }
	GLfloat getart8() { return art8; }
	GLfloat getart9() { return art9; }
	GLfloat getart10() { return art10; }

	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	GLfloat rotax,rotay,rotaz, articulacion1, articulacion2, articulacion3, articulacion4, articulacion5, articulacion6;
	
	//ejercicio1
	/*
	GLfloat rueda1;
	GLfloat rueda2;
	GLfloat rueda3;
	GLfloat rueda4;
	*/

	//ejercicio 2 gato
	GLfloat art1, art2, art3, art4;
	GLfloat art5, art6, art7, art8;
	GLfloat art9, art10;

	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;
	void createCallbacks();
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};

