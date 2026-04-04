/*
Práctica 7: Iluminación 1

Modelo de iluminación local de Lambert (sombreado de phong)

IT=IA+ID+IS

Iluminacion Ambiental : afecta a todos los objetos/luces por igual (sol es blanco para que se puedan ver todos los colores del espectro, se puede ajustar la tonalidad/color para que afecte la tonaidad de los objetos)
Iluminacion Difusa: color del objeto y la luz (en el codigo no viene un apartado para asignar color por luz, si ya trae del vertice, uniform color o la textura, eso lo va a tomar, si se le llega a asignar se multiplica por lo que traiga)
Intensidad Especular: brillos de la luz en los objetos (en el codigo está implementado para variar segun la posicion de la cámara, se le llama iluminacion compleja, dependiendo del valor/componente especular puede cambiar el color)
***material relacionado al objeto, opaco(brillo bajo) y brillante (brillo alto)


3 fuentes de luz (maximas 8 o 16):
Infinitamente dstante (direccional): en el proyecto sirve para representar al sol
puntual (posicional): tipo foco, emite luz a su alrededor a partir de un radio o flama de una vela
reflector (spotlight): luz de una linterna, luz está posicionada y tiene un vector de dirección de hacia donde apunta, un ángulo y está limitado su alncance

debe existir por lo menos una luz de cada tipo para que el shader no trate de calcular ago q no existe xd
se sugiere trabajar maximo con 8 luces porque el hardware te limita cuantas puedes usar (16 luces que es el máximo si jaswer es perro)


Super Clase light: (trae datos pre establecidos que le hereda a sus hijos)
				   -Directionl light
				   -Point light
					 --Spotlight (hijo de pointlight¿)

atenuación (Ax2+ Bx +C ): te indica que tan lejos va a llegar esa luz, no debe llegar hasta el infinito


el color de la luz se va a multiplicar con el color del objeto
luz: roja 1,0,0 * objeto: verde 0,1,0 = (0,0,0)

si usas luz blanca y blanca y azul la blanca se come a las demás luces, se recomienda no poner 1,1,1 si no valores cercanos

------------ procesamiento de iluminacion y sombreado al shader ^^^ -------------------


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

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

Model Carro_cuerpo;
Model Carro_cofre;
Model Carro_llanta1;
Model Carro_llanta2;
Model Carro_llanta3;
Model Carro_llanta4;
Model lampara;


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


//función de calculo de normales por promedio de vértices 
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

	GLfloat vertices[] = { //no son los vaores de la normal, son los valores de la direccion la luz va a iluminar al vértice (usualmente es la contraria a la normal pero no siempre)
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

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};	GLfloat cubo_vertices[] = {
			// front
			//x		y		z		S		T			NX		NY		NZ
			-0.5f, -0.5f,  0.5f,	0.26f,  0.34f,		0.0f,	0.0f,	-1.0f,	//0
			0.5f, -0.5f,  0.5f,		0.49f,	0.34f,		0.0f,	0.0f,	-1.0f,	//1
			0.5f,  0.5f,  0.5f,		0.49f,	0.66f,		0.0f,	0.0f,	-1.0f,	//2
			-0.5f,  0.5f,  0.5f,	0.26f,	0.66f,		0.0f,	0.0f,	-1.0f,	//3
			// right
			//x		y		z		S		T
			0.5f, -0.5f,  0.5f,	    0.0f,  0.0f,		-1.0f,	0.0f,	0.0f,
			0.5f, -0.5f,  -0.5f,	1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,
			0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
			0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
			// back
			-0.5f, -0.5f, -0.5f,	0.0f,  0.0f,		0.0f,	0.0f,	1.0f,
			0.5f, -0.5f, -0.5f,		1.0f,	0.0f,		0.0f,	0.0f,	1.0f,
			0.5f,  0.5f, -0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	1.0f,
			-0.5f,  0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	1.0f,

			// left
			//x		y		z		S		T
			-0.5f, -0.5f,  -0.5f,	0.0f,  0.0f,		1.0f,	0.0f,	0.0f,
			-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		1.0f,	0.0f,	0.0f,
			-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
			-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

			// bottom
			//x		y		z		S		T
			-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	1.0f,	0.0f,
			0.5f,  -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	1.0f,	0.0f,
			 0.5f,  -0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,
			-0.5f, -0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,

			//UP
			 //x		y		z		S		T
			 -0.5f, 0.5f,  0.5f,	0.0f,  0.0f,		0.0f,	-1.0f,	0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	-1.0f,	0.0f,
			  0.5f, 0.5f,  -0.5f,	1.0f,	1.0f,		0.0f,	-1.0f,	0.0f,
			 -0.5f, 0.5f,  -0.5f,	0.0f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

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

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

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

	lampara = Model();
	lampara.LoadModel("Models/lampara.obj");


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
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, //color blanco
		0.01f, 0.01f, //intensidad ambiental y difusa
		0.0f, -1.0f, 0.0f); //en qué dirección apunta el sol
	//contador de luces puntuales

	//direccional tratar de estar en valores intermedios 


	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 0.0f,//intensidad ambiental y difusa
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	//------------------------- foco lampara --------------------------
	pointLights[1] = PointLight(
		1.0f, 1.0f, 1.0f,   // color blanco
		0.5f, 0.5f,         // intensidad ambiental y difusa

		9.0f, 7.5f, 3.0f,   // posición

		0.1f, 0.05f, 0.01f  // atenuación (alcance medio) 0.05f, 0.02f, 0.005f,
	);
	pointLightCount++;
	//-----------------------------------------------------------------

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f, //dentro del while se esta ligando a la camara
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.1f, 0.05f, 0.01f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, //intensidad ambiental y difusa
		5.0f, 10.0f, 0.0f, //posicion
		0.0f, -5.0f, 0.0f, //direccion de la luz
		1.0f, 0.0f, 0.0f, //parametros de atenuación
		15.0f); // cono/angulo
	spotLightCount++;

	//se crean mas luces puntuales y spotlight 


	//------------------------- faro carro ----------------------------------
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,   // color azul
		1.0f, 5.0f,         // intensidad ambiental y difusa
		0.0f, 0.0f, 0.0f,   // posición 
		5.0f, 0.0f, 0.0f,  // dirección hacia enfrente
		0.1f, 0.05f, 0.01f,   // atenuación
		25.0f               // angulo
	);
	spotLightCount++;
	//------------------------------------------------------------------

	//------------------------- luz helicoptero ------------------------
	spotLights[3] = SpotLight(
		1.0f, 1.0f, 0.0f,   // color amarillo
		0.5f, 0.8f,          // intensidad
		0.0f, 0.0f, 0.0f,   // posición inicial
		0.0f, -5.0f, 0.0f,  // hacia el piso
		0.1f, 0.05f, 0.01f, // atenuación (alcance medio)
		20.0f               // ángulo (cono)
	);
	spotLightCount++;
	//------------------------------------------------------------------

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
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

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());


		//spotLights[1].SetPos(poscoche + glm::vec(x, y, cofre)); --- esta no sirve

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();


		//--------------------------- carro -----------------------------------------------------------------------

		// ================= cuerpo carro =================
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), 0.0f, -1.5f)); //mover padre mover todo
		modelaux = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_cuerpo.RenderModel();//modelo sin las 4 llantas ni cofre

		// ================= FARO DEL COCHE =================
		glm::mat4 modelFaro = modelaux;

		// aplicar transformación local del faro como si fuera hijo
		modelFaro = glm::translate(modelFaro, glm::vec3(5.5f, 1.5f, -2.0f));

		// obtener posición del faro desde la matriz
		glm::vec3 posFaro = glm::vec3(modelFaro[3]);
		glm::vec3 dirFaro = glm::vec3(1.0f, 0.0f, 0.0f);

		// actualizar spotlight
		spotLights[2].SetFlash(posFaro, dirFaro);


		// ================= cofre =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, 3.0f, 0.0f));

		model = glm::rotate(
			model,
			mainWindow.getarticulacion5() * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f)   // eje Z
		);


		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_cofre.RenderModel();



		// ================= llanta 1 delantera izq =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.5f, 1.0f, -3.0f));

		model = glm::rotate(
			model,
			mainWindow.getarticulacion1() * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f)   // eje Z
		);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_llanta1.RenderModel();


		// ================= llanta2 trasera izquierda =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.5f, 1.0f, -3.0f));

		model = glm::rotate(
			model,
			mainWindow.getarticulacion2() * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f)   // eje Z
		);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_llanta2.RenderModel();


		// ================= llanta3 delantera derecha =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(4.5f, 1.0f, 3.0f));

		model = glm::rotate(
			model,
			mainWindow.getarticulacion3() * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f)   // eje Z
		);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_llanta3.RenderModel();

		// ================= llanta4 trasera derecha =================
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.5f, 1.0f, 3.0f));

		model = glm::rotate(
			model,
			mainWindow.getarticulacion4() * toRadians,
			glm::vec3(0.0f, 0.0f, 1.0f)   // eje Z
		);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro_llanta4.RenderModel();

		//----------------------------------------------------------------------------------------------------------


		//------------------------ helicoptero ------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(
			mainWindow.getmuevex2(),
			5.0f,
			6.0f
		));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//guardar como padre
		glm::mat4 modelHeli = model;

		// render
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();


		// matriz hija
		glm::mat4 modelLuzHeli = modelHeli;

		// offset (debajo del helicóptero)
		modelLuzHeli = glm::translate(modelLuzHeli, glm::vec3(0.0f, -2.0f, 0.0f));

		glm::vec3 posHeli = glm::vec3(modelLuzHeli[3]);
		glm::vec3 dirHeli = glm::vec3(0.0f, -1.0f, 0.0f);

		// actualizar spotlight
		spotLights[3].SetFlash(posHeli, dirHeli);

		//------------------------------------------------------------------
		//------------------------ lampara ---------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.0f, -0.5f, 3.0f));

		// guardar como padre
		glm::mat4 modelLampara = model;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lampara.RenderModel();


		//------------------------------------------------------------------

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		meshList[4]->RenderMesh();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
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

//ejercicio 
//1. traer el coche, ponerle luz spotlight azul para adelante y ponerlo en el frente del coche, la luz se mueve con el coche ligada por jerarquía 
//previo como simular que hay mas luces de las que realmente hay, como se cambian entre arreglos de luces¿¿
// reporte de practica, la lampara ya tiene que ser del universo del personaje 