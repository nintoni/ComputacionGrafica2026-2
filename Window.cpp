#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	articulacion1 = 0.0f;
	articulacion2 = 0.0f;
	articulacion3 = 0.0f;
	articulacion4 = 0.0f;
	articulacion5 = 0.0f;
	dir1 = 1;
	dir2 = 1;
	dir3 = 1;
	dir4 = 1;
	dir5 = 1;

	muevex = 2.0f;
	muevex2 = 0.0f;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "PracticaXX:Nombre de la practica", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 1.0;
	}

	// GIRAR LLANTAS HACIA ADELANTE
	if (key == GLFW_KEY_1)
	{
		theWindow->articulacion1 -= 10.0f;

		// copiar mismo valor a todas las llantas
		theWindow->articulacion2 = theWindow->articulacion1;
		theWindow->articulacion3 = theWindow->articulacion1;
		theWindow->articulacion4 = theWindow->articulacion1;
	}

	// GIRAR LLANTAS HACIA ATRÁS
	if (key == GLFW_KEY_2)
	{
		theWindow->articulacion1 += 10.0f;

		theWindow->articulacion2 = theWindow->articulacion1;
		theWindow->articulacion3 = theWindow->articulacion1;
		theWindow->articulacion4 = theWindow->articulacion1;
	}

	if (key == GLFW_KEY_3)
	{
		theWindow->articulacion3 += 5 * theWindow->dir3;

		if (theWindow->articulacion3 >= 45)
			theWindow->dir3 = -1;

		if (theWindow->articulacion3 <= -45)
			theWindow->dir3 = 1;
	}

	// ABRIR COFRE
	if (key == GLFW_KEY_4)
	{
		theWindow->articulacion5 += 5.0f;

		if (theWindow->articulacion5 > 45.0f)
			theWindow->articulacion5 = 45.0f;
	}

	// CERRAR COFRE
	if (key == GLFW_KEY_5)
	{
		theWindow->articulacion5 -= 5.0f;

		if (theWindow->articulacion5 < 0.0f)
			theWindow->articulacion5 = 0.0f;
	}

	// AVANZAR
	if (key == GLFW_KEY_N)
	{
		theWindow->muevex += 0.5f;

		// rotación de llantas hacia adelante
		theWindow->articulacion1 -= 15.0f;

		theWindow->articulacion2 = theWindow->articulacion1;
		theWindow->articulacion3 = theWindow->articulacion1;
		theWindow->articulacion4 = theWindow->articulacion1;
	}

	// RETROCEDER
	if (key == GLFW_KEY_M)
	{
		theWindow->muevex -= 0.5f;

		// rotación de llantas hacia atrás
		theWindow->articulacion1 += 15.0f;

		theWindow->articulacion2 = theWindow->articulacion1;
		theWindow->articulacion3 = theWindow->articulacion1;
		theWindow->articulacion4 = theWindow->articulacion1;
	}

	// HELICÓPTERO ATRÁS
	if (key == GLFW_KEY_J)
	{
		theWindow->muevex2 += 0.5f;
	}

	// HELICÓPTERO ADELANTE 
	if (key == GLFW_KEY_K)
	{
		theWindow->muevex2 -= 0.5f;
	}


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
