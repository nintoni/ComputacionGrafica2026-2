/*
Práctica 6: Texturizado
*/
//para cargar imagen
//checar la exportación en tga en el lobito, como utilizar el crear canal alfa con una goma para dar transparencia, y ajustar las dimensiones de la imagen

//filtro nearest pixeleado, linear blurred

//mipmap, si se le da una textura a un tamaño y te vas aleando de esa textura genera la textura a menor resolución 

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

#include "Window.h"
#include "Mesh.h"
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//todo lo anterior era para definir la textura, aqui es donde se crea
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture genshinTexture;
Texture logofiTexture;

Model Kitt_M;
Model Llanta_M;
Model Dado_M;

Model Carro_cuerpo;
Model Carro_cofre;
Model Carro_llanta1;
Model Carro_llanta2;
Model Carro_llanta3;
Model Carro_llanta4;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";




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
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);


	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
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

	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.65f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.65f,		0.0f,	0.0f,	-1.0f,	//3

		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.51f,   0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.65,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.51f,	0.65,		-1.0f,	0.0f,	0.0f,
		// back 
		// x      y      z      S      T        NX    NY    NZ
		-0.5f, -0.5f, -0.5f,   0.99f,  0.35f,    0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,   0.76f, 0.35f,    0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,   0.76f, 0.65f,    0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,   0.99f,  0.65f,    0.0f,  0.0f,  1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.01f,   0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.24f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.24f,	0.65f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.01f,	0.65f,		1.0f,	0.0f,	0.0f,

		// bottom
		// x      y      z      S      T       
		-0.5f, -0.5f,  0.5f,   0.51f,  0.01f,			0.0f,	-1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,   0.51f,  0.33f,		0.0f,	-1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,   0.73f, 0.33f,		0.0f,	-1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,   0.73f, 0.01f,			0.0f,	-1.0f,  0.0f,

		// up
		// x      y      z      S      T       
		-0.5f,  0.5f,  0.5f,   0.51f,  0.99f,			0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,   0.51f,  0.67f,		0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,   0.73f, 0.67f,		0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,   0.73f, 0.99f,			0.0f,  1.0f,  0.0f,

	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(dado);

}

void CrearOctaedro()
{
	unsigned int indices[] = {
		0,1,2,   3,4,5,   6,7,8,   9,10,11,
		12,13,14, 15,16,17, 18,19,20, 21,22,23
	};

	GLfloat vertices[] = {

		// -------- SUPERIOR --------

		// cara 1 (arriba) numero 5
		0.0f,0.7f,0.0f,    0.75f,1.0f,   0.0f,0.0f,0.0f,
		-0.5f,0.0f,0.5f,   0.5f,0.75f,   0.0f,0.0f,0.0f,
		0.5f,0.0f,0.5f,    1.0f,0.75f,   0.0f,0.0f,0.0f,

		// cara 2 numero 3
		0.0f,0.7f,0.0f,    0.75f,0.0f,   0.0f,0.0f,0.0f,
		0.5f,0.0f,0.5f,    1.0f,0.25f,   0.0f,0.0f,0.0f,
		0.5f,0.0f,-0.5f,   0.5f,0.25f,   0.0f,0.0f,0.0f,

		// cara 3 numero 2
		0.0f,0.7f,0.0f,    0.0f,0.25f,   0.0f,0.0f,0.0f,
		0.5f,0.0f,-0.5f,   0.5f,0.25f,   0.0f,0.0f,0.0f,
		-0.5f,0.0f,-0.5f,  0.25f,0.5f,   0.0f,0.0f,0.0f,

		// cara 4 numero 8
		0.0f,0.7f,0.0f,    0.0f,0.75f,   0.0f,0.0f,0.0f,
		-0.5f,0.0f,-0.5f,  0.25f,0.5f,   0.0f,0.0f,0.0f,
		-0.5f,0.0f,0.5f,   0.5f,0.75f,   0.0f,0.0f,0.0f,


		// -------- INFERIOR --------

		// cara 5 numero 7
		0.0f,-0.7f,0.0f,   0.75f,0.5f,   0.0f,0.0f,0.0f,
		0.5f,0.0f,0.5f,    1.0f,0.75f,   0.0f,0.0f,0.0f,
		-0.5f,0.0f,0.5f,   0.5f,0.75f,   0.0f,0.0f,0.0f,

		// cara 6 numero 6
		0.0f,-0.7f,0.0f,   0.75f,0.5f,   0.0f,0.0f,0.0f,
		0.5f,0.0f,-0.5f,   0.5f,0.25f,   0.0f,0.0f,0.0f,
		0.5f,0.0f,0.5f,    1.0f,0.25f,   0.0f,0.0f,0.0f,

		// cara 7 numero 4
		0.0f,-0.7f,0.0f,   0.75f,0.5f,   0.0f,0.0f,0.0f,
		-0.5f,0.0f,-0.5f,  0.25f,0.5f,   0.0f,0.0f,0.0f,
		0.5f,0.0f,-0.5f,   0.5f,0.25f,   0.0f,0.0f,0.0f,

		// cara 8 numero 1
		0.0f,-0.7f,0.0f,   0.75f,0.5f,   0.0f,0.0f,0.0f,
		-0.5f,0.0f,0.5f,   0.5f,0.75f,   0.0f,0.0f,0.0f,
		-0.5f,0.0f,-0.5f,  0.25f,0.5f,   0.0f,0.0f,0.0f

	};

	calcAverageNormals(indices, 24, vertices, 192, 8, 5);

	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(vertices, indices, 192, 24);
	meshList.push_back(octaedro);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CrearOctaedro();
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
	dadoTexture = Texture("Textures/dado_animales.tga");
	dadoTexture.LoadTextureA();

	genshinTexture = Texture("Textures/dado_genshin.tga"); //textura dado------------------
	genshinTexture.LoadTextureA();

	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();
	
	

	Dado_M = Model();
	Dado_M.LoadModel("Models/Dado_Animales.obj");

	//acá llamar a los modelos de los ejercicios-------------------------
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

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
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
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();

		
		
		//Dado de Opengl
		//Ejercicio 1: Texturizar su cubo con la imagen dado_animales ya optimizada por ustedes
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 3.0f, 2.0f));
		model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshList[4]->RenderMesh(); //creada la textura se le aplica a cierto objeto
		
		//Ejercicio 2:Importar el cubo texturizado en el programa de modelado con 
		//la imagen dado_animales ya optimizada por ustedes
		
		//Dado importado *********ESte es para el de blender 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 5.0f, -2.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Dado_M.RenderModel();
		
		
		
		/*Reporte de práctica :
		Ejercicio 1: Crear un dado de 8 caras y texturizarlo por medio de código
		
		Ejercicio 2: Importar el modelo de su coche con sus 4 llantas acomodadas
		y tener texturizadas las 4 llantas (diferenciar caucho y rin)  y 
		texturizar el logo de la Facultad de ingeniería en el cofre de su propio modelo de coche
		
		Ejercicio 3: Texturizar la cara del personaje de la imagen tipo cars en el espejo (ojos) y 
		detalles en cofre y parrilla de su propio modelo de coche
		*/

		//--------------- Ejercicio 1 practica ----------------

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.0f, 5.0f, 2.0f)); // posición diferente
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		genshinTexture.UseTexture(); // textura
		meshList[5]->RenderMesh(); // el octaedro

		//------------------------------------------------------


		//----------------------------- Ejercicio 2 practica ------------------------------
		
		// ================= cuerpo carro =================


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), -1.0f, -1.5f)); //mover padre mover todo

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		Carro_cuerpo.RenderModel();//modelo sin las 4 llantas ni cofre

		modelaux = model;



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


		//******************************************************************************************************************

		glUseProgram(0);

		mainWindow.swapBuffers();
	}
	
	return 0;
}

//por ahora no se van a agregar, son para cuando se quiere agregar canal alpha 
/*
//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		logofiTexture.UseTexture(); //textura con transparencia o traslucidez
		FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		glDisable(GL_BLEND);
*/