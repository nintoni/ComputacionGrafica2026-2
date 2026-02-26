//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
//clases para dar orden y limpieza al código
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"
//Dimensiones de la ventana
const float toRadians = 3.14159265f/180.0; //grados a radianes
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";
//shaders nuevos se crearían acá

float angulo = 0.0f;

//color café en RGB : 0.478, 0.255, 0.067

//Pirámide triangular regular
void CreaPiramide()
{
	unsigned int indices[] = { 
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3
		
	};
	GLfloat vertices[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);
}

//Vértices de un cubo
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

GLfloat cubo_vertices[] = {
	// front
	-0.5f, -0.5f,  0.5f,
	0.5f, -0.5f,  0.5f,
	0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	// back
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f
};
Mesh *cubo = new Mesh();
cubo->CreateMesh(cubo_vertices, cubo_indices,24, 36);
meshList.push_back(cubo);
}

void CrearPiramideBaseCuadrada()
{
	unsigned int indices[] = {
		// base
		0, 1, 2,
		2, 3, 0,

		// lados
		0, 1, 4,
		1, 2, 4,
		2, 3, 4,
		3, 0, 4
	};

	GLfloat vertices[] = {
		-0.5f, 0.0f,  0.5f,   // 0
		 0.5f, 0.0f,  0.5f,   // 1
		 0.5f, 0.0f, -0.5f,   // 2
		-0.5f, 0.0f, -0.5f,   // 3
		 0.0f, 0.7f,  0.0f    // 4 (punta)
	};

	Mesh* piramide = new Mesh();
	piramide->CreateMesh(vertices, indices, 15, 18);
	meshList.push_back(piramide);
}

void CrearLetrasyFiguras()
{
	GLfloat vertices_letras[] = {	
			//X			Y			Z			R		G		B
			-1.0f,	-1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
			1.0f,	-1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
			1.0f,	1.0f,		0.5f,			0.0f,	0.0f,	1.0f,
			/*1.0f,	1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
			-1.0f,  1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
			-1.0f,	-1.0f,		0.5f,			1.0f,	0.0f,	0.0f,*/
			
	};
	MeshColor *letras = new MeshColor();
	letras->CreateMeshColor(vertices_letras,18);
	meshColorList.push_back(letras);

	// TRIANGULO ROJO

	GLfloat vertices_triangulorojo[] = {
		//X			Y			Z			R		G		B
		-1.0f,	-1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
		1.0f,	-1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
		0.0f,	1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
		
	};

	MeshColor* triangulorojo = new MeshColor();
	triangulorojo->CreateMeshColor(vertices_triangulorojo, 18);
	meshColorList.push_back(triangulorojo);


	// CUADRADO VERDE 
	GLfloat vertices_cuadradoverde[] = {
		//X			Y			Z			R		G		B
		-0.5f,	-0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		0.5f,	-0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		-0.5f,	-0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		0.5f,	0.5f,		0.5f,			0.0f,	1.0f,	0.0f,
		-0.5f,	0.5f,		0.5f,			0.0f,	1.0f,	0.0f,

	};

	MeshColor* cuadradoverde = new MeshColor();
	cuadradoverde->CreateMeshColor(vertices_cuadradoverde, 36);
	meshColorList.push_back(cuadradoverde);

	// NUEVAS FIGURAS ------------------------------------------------

	// 1. TRIÁNGULO AZUL 
	GLfloat vertices_trianguloazul[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // vértice 0: azul
		0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,   // vértice 1: azul
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,    // vértice 2: azul
	};
	MeshColor* trianguloazul = new MeshColor();
	trianguloazul->CreateMeshColor(vertices_trianguloazul, 18);
	meshColorList.push_back(trianguloazul);  // índice 3

	// 2. TRIÁNGULO VERDE OSCURO 
	GLfloat vertices_trianguloverde[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f,  // vértice 0: verde oscuro
		0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f,   // vértice 1: verde oscuro
		0.0f, 0.5f, 0.0f, 0.0f, 0.5f, 0.0f,    // vértice 2: verde oscuro
	};
	MeshColor* trianguloverde = new MeshColor();
	trianguloverde->CreateMeshColor(vertices_trianguloverde, 18);
	meshColorList.push_back(trianguloverde);  // índice 4

	// 3. CUADRADO ROJO
	GLfloat vertices_cuadradorojo[] = {
		// Triángulo 1
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // vértice 0: rojo
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // vértice 1: rojo
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // vértice 2: rojo
		// Triángulo 2
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // vértice 3: rojo
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // vértice 4: rojo
		-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // vértice 5: rojo
	};
	MeshColor* cuadradorojo = new MeshColor();
	cuadradorojo->CreateMeshColor(vertices_cuadradorojo, 36);
	meshColorList.push_back(cuadradorojo);  // índice 5

	// CUADRADO CAFÉ 
	GLfloat vertices_cuadradocafe[] = {
		// Triángulo 1
		-0.5f, -0.5f, 0.0f, 0.478f, 0.255f, 0.067f,  // vértice 0: café
		0.5f, -0.5f, 0.0f, 0.478f, 0.255f, 0.067f,   // vértice 1: café
		0.5f, 0.5f, 0.0f, 0.478f, 0.255f, 0.067f,    // vértice 2: café
		// Triángulo 2
		-0.5f, -0.5f, 0.0f, 0.478f, 0.255f, 0.067f,  // vértice 3: café
		0.5f, 0.5f, 0.0f, 0.478f, 0.255f, 0.067f,    // vértice 4: café
		-0.5f, 0.5f, 0.0f, 0.478f, 0.255f, 0.067f,   // vértice 5: café
	};
	MeshColor* cuadradocafe = new MeshColor();
	cuadradocafe->CreateMeshColor(vertices_cuadradocafe, 36);
	meshColorList.push_back(cuadradocafe);  // índice 6
}


void CreateShaders() //añadir shaders aca
{

	Shader *shader1 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shaderRojo = new Shader(); // shader rojo
	shaderRojo->CreateFromFiles(
		"shaderColorUniform.vert",
		"shaderRojo.frag"
	);
	shaderList.push_back(*shaderRojo);

	Shader* shaderAzul = new Shader(); // shader azul
	shaderAzul->CreateFromFiles(
		"shaderColorUniform.vert",
		"shaderAzul.frag"
	);
	shaderList.push_back(*shaderAzul);

	Shader* shaderVerde = new Shader(); // shader Verde
	shaderVerde->CreateFromFiles(
		"shaderColorUniform.vert",
		"shaderVerde.frag"
	);
	shaderList.push_back(*shaderVerde);

	Shader* shaderVerdeOscuro = new Shader(); // shader Verde Oscuro
	shaderVerdeOscuro->CreateFromFiles(
		"shaderColorUniform.vert",
		"shaderVerdeOscuro.frag"
	);
	shaderList.push_back(*shaderVerdeOscuro);

	Shader* shaderCafe = new Shader(); // shader cafe
	shaderCafe->CreateFromFiles(
		"shaderColorUniform.vert",
		"shaderCafe.frag"
	);
	shaderList.push_back(*shaderCafe);

	//____________________________________________

	Shader *shader2 = new Shader();//shader para usar color como parte del VAO: letras 
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2);
}

//======================================================================================================================
int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	CreaPiramide(); //índice 0 en MeshList
	CrearCubo();//índice 1 en MeshList
	CrearPiramideBaseCuadrada();
	CrearLetrasyFiguras(); //usa MeshColor, índices en MeshColorList
	CreateShaders();
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	//Projection: Matriz de Dimensión 4x4 para indicar si vemos en 2D( orthogonal) o en 3D) perspectiva
	//glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f); // PROYECCIOOOONNNNNNNNNNNNN cambiar*************
	glm::mat4 projection = glm::perspective(glm::radians(60.0f)	,mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	
	//Model: Matriz de Dimensión 4x4 en la cual se almacena la multiplicación de las transformaciones geométricas.
	glm::mat4 model(1.0); //fuera del while se usa para inicializar la matriz con una identidad
	
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		//Recibir eventos del usuario
		glfwPollEvents();
		//Limpiar la ventana
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		

		//################################ 2D ############################
		/*
		//+++++++++++ dibujar triangulo azul									
		//Para las letras hay que usar el segundo set de shaders con índice 1 en ShaderList 
		//shaderList[1].useShader();//+++++++++++ USAR SHADER DE COLOR
		//uniformModel = shaderList[1].getModelLocation();
		//uniformProjection = shaderList[1].getProjectLocation();
		
		//Inicializar matriz de dimensión 4x4 que servirá como matriz de modelo para almacenar las transformaciones geométricas
		//model = glm::mat4(1.0);//+++++++++++matriz identidad sin transformacion
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));//+++++++++++
		//mandar matrices al shader
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA y se envían al shader como variables de tipo uniform //+++++++++++
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//meshColorList[0]->RenderMeshColor();//+++++++++++
		//+++++++++++


		//-------------------- CASAAAAAAAAAAA---------------------------
		// +++++ CUADRADO ROJO +++++
		shaderList[1].useShader();//+++++++++++ USAR SHADER DE COLOR
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();

		model = glm::mat4(1.0f);//+++++++++++matriz identidad sin transformacion
		model = glm::translate(model, glm::vec3(0.0f, -0.5, -1.0f)); // vista de la camara¿, Colocar el objeto dentro del volumen visible

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); //enviar matrices al shader
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); //rotar

		meshColorList[5]->RenderMeshColor(); // dibujar cuadrado rojo


		// +++++ NUEVO: TRIÁNGULO AZUL ENCIMA +++++
		model = glm::mat4(1.0f);  // reiniciar matriz
		// Colocar dentro del volumen visible
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -1.0f));
		// lo subimos en Y para que quede arriba del cuadrado
		model = glm::scale(model, glm::vec3(1.5f, 1.0f, 1.0f)); //hacerlo mas choncho y no tan alto

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		// Dibujar triángulo azul
		meshColorList[3]->RenderMeshColor();


		// ---- PUERTA VERDE ---------------------------------

		model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, -0.8f, -1.0f));

		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		meshColorList[2]->RenderMeshColor();
		*/


		//#########################  3D  #####################################

		//+++++++++++ dibujar cubo

		/*
		
		//Para el cubo y la pirámide se usa el primer set de shaders con índice 0 en ShaderList ++++++++++++++++++++++++++++++++++++++++++++++++
		shaderList[0].useShader(); 
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		angulo += 0.01;


		//Inicializar matriz de dimensión 4x4 que servirá como matriz de modelo para almacenar las transformaciones geométricas
		model = glm::mat4(1.0);//+++++++++++
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));//+++++++++++
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA  //+++++++++++
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//meshList[1]->RenderMesh();//+++++++++++
		
		meshList[1]->RenderMesh();
		*/

		angulo += 0.01f;

		//cubo rojo para casita
		shaderList[1].useShader();

		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();

		model = glm::mat4(1.0f);
		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));// mover el cubo hacia atrás (profundidad)
		
		//model = glm::rotate(model,glm::radians(angulo),glm::vec3(0.0f, 1.0f, 0.0f));// rotar sobre el eje Y (3D REAL)

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		meshList[1]->RenderMesh();

		//piramide azul techo
		shaderList[2].useShader();

		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -3.0f)); //moverla arriba del cubo
		//model = glm::rotate(model, angulo, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f)); //escalar
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		// dibujar pirámide nueva
		meshList[2]->RenderMesh();


//-----------------------------------------------------

		glUseProgram(0);
		mainWindow.swapBuffers();

	}
	return 0;
}
// inicializar matriz: glm::mat4 model(1.0);
// reestablecer matriz: model = glm::mat4(1.0);
//Traslación
//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
//////////////// ROTACIÓN //////////////////
//model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
////////////////  ESCALA ////////////////
//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
///////////////////// T+R////////////////
/*model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
*/
/////////////R+T//////////
/*model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
*/