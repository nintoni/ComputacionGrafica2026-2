/*
Animación:
Sesión 1: Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
lo que se hacía por teclado para mover o transformar objetos se hará por banderas que s evan a estar incrementando cada ciclo de reloj


Sesión 2: Compleja: Por medio de funciones y algoritmos. Textura Animada



No son tecnicas de animacion, simplemente es movimiento de los objetos por código


para que el movimiento se considere animacion debe de ser mas de una transformacion geometrica modificada
solo una traslacion como el de una puerta no se considera animación, para hacerla animacion sería girar la perilla o algo por el estilo
o una ventana corrediza bota el seguro,



*/

//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza = 1;

int estado = 0;         // 0=avanza, 1=giro1, 2=regresa, 3=giro2
float rotCoche = 0.0f;  // rotación del carro en Y (para el giro de 180)
float velocidadGiro = 1.0f; // grados por segundo que tarda en girar

float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float angulovaria = 0.0f;
//variables para la siguiente practica

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;



Model Kitt_M;
Model Llanta_M;
Model Pista_M;
Model Nave_M;
Model Ala_M;
Model Carro_cuerpo;
Model Carro_cofre;
Model Carro_llanta1;
Model Carro_llanta2;
Model Carro_llanta3;
Model Carro_llanta4;
Model reloj;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga"); //linea que faltaba xd
	FlechaTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");
	Ala_M = Model();
	Ala_M.LoadModel("Models/ala.obj");

	reloj = Model();
	reloj.LoadModel("Models/reloj.obj");

	Carro_cuerpo = Model();
	Carro_cuerpo.LoadModel("Models/cuerpo.obj");
	Carro_cofre = Model();
	Carro_cofre.LoadModel("Models/cofre.obj");
	Carro_llanta1 = Model();
	Carro_llanta1.LoadModel("Models/llanta1.obj");
	Carro_llanta2 = Model();
	Carro_llanta2.LoadModel("Models/llanta1.obj");
	Carro_llanta3 = Model();
	Carro_llanta3.LoadModel("Models/llanta3.obj");
	Carro_llanta4 = Model();
	Carro_llanta4.LoadModel("Models/llanta3.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	/*
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;
	*/
	//------------------------- faro nave ----------------------------------
	spotLights[1] = SpotLight(
		1.0f, 1.0f, 0.0f,   // color amarillo
		1.0f, 5.0f,         // intensidad ambiental y difusa
		0.0f, 0.0f, 0.0f,   // posición
		0.0f, -5.0f, 0.0f,  // apunta hacia el piso -y
		0.4f, 0.4f, 0.4f,   // atenuación
		25.0f               // angulo
	);
	spotLightCount++;
	//------------------------------------------------------------------
	

	//------------------------- faro carro ----------------------------------

	spotLights[2] = SpotLight(
		0.0f, 0.0f, 1.0f,   // color azul
		1.0f, 5.0f,         // intensidad ambiental y difusa
		0.0f, 0.0f, 0.0f,   // posición
		5.0f, 0.0f, 0.0f,  // dirección hacia enfrente +x
		0.1f, 0.05f, 0.01f,   // atenuación
		25.0f               // angulo
	);
	spotLightCount++;

	//------------------------------------------------------------------

	


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.2f;
	rotllanta = 0.0f;
	rotllantaOffset = 5.0f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);


	struct Waypoint {
		glm::vec3 posicion;
		float rotY;   // giro lado a lado (curvas)
		float rotZ;   // inclinación (rampa)
	};

	std::vector<Waypoint> ruta = {
	{ glm::vec3(2.0f,  -1.0f,  -4.0f),   0.0f,   0.0f },  // inicio
	{ glm::vec3(25.0f,  -1.0f,  -4.0f),   5.0f,   0.0f },  
	{ glm::vec3(45.0f,  -1.0f, -10.0f),  25.0f,   0.0f },  
	{ glm::vec3(56.0f,   0.5f, -11.0f), -30.0f,   7.0f },  
	{ glm::vec3(67.0f,   2.7f,   0.0f), -50.0f,  10.0f }, // 
	{ glm::vec3(75.0f,   4.5f,   6.5f), -20.0f,  17.0f },  //
	{ glm::vec3(85.0f,   8.0f,   7.0f),  15.0f,  20.0f },  //
	{ glm::vec3(103.0f,  16.5f,   2.5f),   7.0f,  25.0f },  //
	{ glm::vec3(120.0f,  23.5f,   3.0f), -10.0f,  15.0f },  //
	{ glm::vec3(140.0f,  26.5f,   8.0f), -18.0f,   6.0f },  //
	{ glm::vec3(160.0f,  27.0f,  14.0f), -18.0f,  -1.0f },  //
	{ glm::vec3(177.0f,  25.0f,  20.0f), -20.0f,  -5.0f },  // fin
	};

	// Waypoints de la nave: sentido invertido, separar de la pista para q flote
	std::vector<Waypoint> rutaNave = {
	{ glm::vec3(177.0f,  28.0f,  20.0f), -20.0f,  -5.0f },  // inicio
	{ glm::vec3(160.0f,  30.0f,  14.0f), -18.0f,  -1.0f },
	{ glm::vec3(140.0f,  29.5f,   8.0f), -18.0f,   6.0f },
	{ glm::vec3(120.0f,  26.5f,   3.0f), -10.0f,  15.0f },
	{ glm::vec3(103.0f,  19.5f,   2.5f),   7.0f,  25.0f },
	{ glm::vec3(85.0f,   11.0f,   7.0f),  15.0f,  20.0f },
	{ glm::vec3(75.0f,   7.5f,   6.5f), -20.0f,  17.0f },
	{ glm::vec3(67.0f,   5.7f,   0.0f), -50.0f,  10.0f },
	{ glm::vec3(56.0f,   3.5f, -11.0f), -30.0f,   7.0f },
	{ glm::vec3(45.0f,  2.0f, -10.0f),  25.0f,   0.0f },
	{ glm::vec3(25.0f,  2.0f,  -4.0f),   5.0f,   0.0f },
	{ glm::vec3(2.0f,  2.0f,  -4.0f),   0.0f,   0.0f },  // aterriza
	};

	// variables carro
	int waypointActual = 0;
	float tRuta = 0.0f;          // 0.0 a 1.0 entre waypoint actual y el siguiente
	float velocidadRuta = 0.02f;  // entre puntos
	bool carroActivo = false;     // se activa con la tecla P

	// variables nave
	int waypointNaveActual = 0;
	float tNave = 0.0f;
	float velocidadNave = 0.015f;
	bool naveActiva = true;  // arranca automáticamente, no se puede repetir
	bool naveTerminada = false;


	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;

		// animación carro
		if (carroActivo && waypointActual < (int)ruta.size() - 1)
		{
			tRuta += velocidadRuta * deltaTime;

			if (tRuta >= 1.0f)
			{
				tRuta = 0.0f;
				waypointActual++;

				// llegó al final, se detiene
				if (waypointActual >= (int)ruta.size() - 1)
				{
					waypointActual = (int)ruta.size() - 1;
					carroActivo = false;
				}
			}
		}

		// animación nave
		if (naveActiva && !naveTerminada && waypointNaveActual < (int)rutaNave.size() - 1)
		{
			tNave += velocidadNave * deltaTime;
			if (tNave >= 1.0f)
			{
				tNave = 0.0f;
				waypointNaveActual++;
				if (waypointNaveActual >= (int)rutaNave.size() - 1)
				{
					waypointNaveActual = (int)rutaNave.size() - 1;
					naveTerminada = true; // se detiene y no se repite
				}
			}
		}

		int siguienteNave = glm::min(waypointNaveActual + 1, (int)rutaNave.size() - 1);
		glm::vec3 posNave = glm::mix(rutaNave[waypointNaveActual].posicion, rutaNave[siguienteNave].posicion, tNave);
		float rotYNave = glm::mix(rutaNave[waypointNaveActual].rotY, rutaNave[siguienteNave].rotY, tNave);

		// Interpolación entre waypoint actual y el siguiente
		int siguiente = glm::min(waypointActual + 1, (int)ruta.size() - 1);
		glm::vec3 posCarro = glm::mix(ruta[waypointActual].posicion, ruta[siguiente].posicion, tRuta);
		float rotYCarro = glm::mix(ruta[waypointActual].rotY, ruta[siguiente].rotY, tRuta);
		float rotZCarro = glm::mix(ruta[waypointActual].rotZ, ruta[siguiente].rotZ, tRuta);
		
		// Llantas giran solo si el carro se mueve
		if (carroActivo) rotllanta += rotllantaOffset * deltaTime;



		//Recibir eventos del usuario
		glfwPollEvents();

		// Tecla P: reinicia y activa la animación
		if (mainWindow.getAnimacionCoche())
		{
			waypointActual = 0;
			tRuta = 0.0f;
			rotllanta = 0.0f;
			carroActivo = true;
			mainWindow.resetAnimacionCoche(); // no reiniciar cada frame
		}

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		//Pista
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.1f, 2.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pista_M.RenderModel();




		// ================= cuerpo carro =================
		model = glm::mat4(1.0);
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // corrección base del modelo hacia -x
		model = glm::translate(model, posCarro);
		model = glm::rotate(model, rotYCarro * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // curvas lado a lado
		model = glm::rotate(model, rotZCarro * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // inclinación arriba abajo
		modelaux = model; // heredar todo lo anterior
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Carro_cuerpo.RenderModel();

		// ================= FARO (heredar dirección del carro) =================
		glm::mat4 modelFaro = modelaux;
		modelFaro = glm::translate(modelFaro, glm::vec3(5.5f, 1.5f, 0.0f));
		glm::vec3 posFaro = glm::vec3(modelFaro[3]);
		glm::vec3 dirFaro = glm::normalize(glm::vec3(modelaux[0])); // eje X local del carro
		spotLights[2].SetFlash(posFaro, dirFaro);


		// ================= cofre =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, 3.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_cofre.RenderModel();

		// ================= llanta1 delantera izquierda =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.5f, 1.0f, -3.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_llanta1.RenderModel();

		// ================= llanta2 trasera izquierda =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.5f, 1.0f, -3.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_llanta2.RenderModel();

		// ================= llanta3 delantera derecha =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.5f, 1.0f, 3.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_llanta3.RenderModel();

		// ================= llanta4 trasera derecha =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.5f, 1.0f, 3.0f));
		model = glm::rotate(model, -rotllanta * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		color = glm::vec3(1.0f, 1.0f, 1.0f); // resetea color para lo siguiente
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_llanta4.RenderModel();


		//Aquí va la nave con jerarquía de modelos, completar
		// ================= nave  =================
		model = glm::mat4(1.0);
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // misma corrección que el carro
		model = glm::translate(model, posNave);
		model = glm::rotate(model, rotYNave * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelNave = model;

		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		Nave_M.RenderModel();

		// ================= ala =================
		model = modelNave;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Ala_M.RenderModel();

		// ================= faro nave (apuntar al piso seguir nave) =================
		glm::mat4 modelFaroNave = modelNave;
		modelFaroNave = glm::translate(modelFaroNave, glm::vec3(0.0f, -1.0f, 0.0f)); // debajo de la nave
		glm::vec3 posFaroNave = glm::vec3(modelFaroNave[3]);
		glm::vec3 dirFaroNave = glm::vec3(0.0f, -1.0f, 0.0f); // q siempre apunte al piso
		spotLights[1].SetFlash(posFaroNave, dirFaroNave);



		// reloj 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//reloj.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
