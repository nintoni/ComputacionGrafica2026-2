#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <glew.h>
#include <glfw3.h>

const int WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader;

// Variables para cambio de color en rgb
float lastTime = 0.0f;
float r = 0.0f, g = 0.0f, b = 0.0f;

// Vertex Shader
static const char* vShader = "                        \n\
#version 330                                        \n\
layout (location =0) in vec3 pos;                   \n\
void main()                                         \n\
{                                                   \n\
gl_Position=vec4(pos.x,pos.y,pos.z,1.0f);           \n\
}";

// Fragment Shader
static const char* fShader = "                      \n\
#version 330                                        \n\
out vec4 color;                                     \n\
void main()                                         \n\
{                                                   \n\
    color = vec4(1.0f,1.0f,1.0f,1.0f);              \n\
}";



void CrearTriangulo() {
    GLfloat vertices[] = {
        //V
        -0.6f, 0.0f, 0.0f,
        -0.45f, 0.0f, 0.0f,
        -0.4f, -0.3f, 0.0f,

        -0.35f, 0.0f, 0.0f,
        -0.2f, 0.0f, 0.0f,
        -0.4f, -0.3f, 0.0f,
        
        -0.6f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.4f, -0.3f, 0.0f,

        -0.2f, 0.0f, 0.0f,
        -0.3f, -0.5f, 0.0f,
        -0.4f, -0.3f, 0.0f,
        
        -0.5f, -0.5f, 0.0f,
        -0.3f, -0.5f, 0.0f,
        -0.4f, -0.3f, 0.0f,

        //C
        -0.1f, 0.3f, 0.0f,
        0.0f, 0.3f, 0.0f,
        -0.1f, -0.2f, 0.0f,

        0.0f, -0.2f, 0.0f,
        0.0f, 0.3f, 0.0f,
        -0.1f, -0.2f, 0.0f,
        //
        0.0f, 0.3f, 0.0f,
        0.2f, 0.3f, 0.0f,
        0.0f, 0.2f, 0.0f,

        0.0f, 0.2f, 0.0f,
        0.2f, 0.3f, 0.0f,
        0.2f, 0.2f, 0.0f,
        //
        0.0f, -0.1f, 0.0f,
        0.2f, -0.1f, 0.0f,
        0.0f, -0.2f, 0.0f,

        0.0f, -0.2f, 0.0f,
        0.2f, -0.1f, 0.0f,
        0.2f, -0.2f, 0.0f,

        //I
        0.3f, 0.5f, 0.0f,
        0.6f, 0.5f, 0.0f,
        0.3f, 0.4f, 0.0f,

        0.3f, 0.4f, 0.0f,
        0.6f, 0.5f, 0.0f,
        0.6f, 0.4f, 0.0f,
        //
        0.4f, 0.4f, 0.0f,
        0.5f, 0.4f, 0.0f,
        0.4f, 0.1f, 0.0f,

        0.4f, 0.1f, 0.0f,
        0.5f, 0.4f, 0.0f,
        0.5f, 0.1f, 0.0f,
        //
        0.3f, 0.1f, 0.0f,
        0.6f, 0.1f, 0.0f,
        0.3f, 0.0f, 0.0f,

        0.3f, 0.0f, 0.0f,
        0.6f, 0.1f, 0.0f,
        0.6f, 0.0f, 0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    GLuint theShader = glCreateShader(shaderType);
    glShaderSource(theShader, 1, &shaderCode, NULL);
    glCompileShader(theShader);
    glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
    shader = glCreateProgram();
    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);
    glLinkProgram(shader);
    glValidateProgram(shader);
}

int main()
{
    if (!glfwInit())
        return 1;

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "fondo random", NULL, NULL);
    if (!mainWindow)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(mainWindow);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    // Semilla diferente cada ejecución para evitar ciclos d reloj
    srand((unsigned int)time(NULL));

    CrearTriangulo();
    CompileShaders();

    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        float currentTime = glfwGetTime();

        if (currentTime - lastTime >= 2.0f) //diferencia de dos segundos
        {
            r = (float)rand() / RAND_MAX;
            g = (float)rand() / RAND_MAX;
            b = (float)rand() / RAND_MAX;

            lastTime = currentTime;
        }

        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 90);
        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    glfwDestroyWindow(mainWindow);
    glfwTerminate();
    return 0;
}
