/*
Trabalho GA - Computação Gráfica
Amanda Grams e Paula Knob
*/

#include <iostream>
#include <sstream>
#include <string>
#define GLEW_STATIC
#include "GL/glew.h"	// Importante - este cabeçalho deve vir antes do cabeçalho glfw3
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Mesh.h"


//Variáveis globais
const char* APP_TITLE = "Amanda e Paula - Loading OBJ Models";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;

//Configurações da Câmera
FPSCamera fpsCamera(glm::vec3(0.0f, 3.0f, 10.0f));
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 3.0; // units per second
const float MOUSE_SENSITIVITY = 0.1f;

// Declaração de Funções
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(double elapsedTime);
void showFPS(GLFWwindow* window);
bool initOpenGL();

//-----------------------------------------------------------------------------
// Aplicação Principal
//-----------------------------------------------------------------------------
int main()
{
	if (!initOpenGL())
	{
		// Se ocorrer erro na inicialização
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	//carrega os shaders (vertex e fragment) 
	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("shaders/basic.vert", "shaders/basic.frag");

	// Carregar mesh e texturas
	const int numModels = 2;
	Mesh mesh[numModels];
	Texture2D texture[numModels];

	// OBJ's que estão sendo carregados na cena
	mesh[0].loadOBJ("models/cube.obj");
	mesh[1].loadOBJ("models/floor.obj");
	
	// carregando as imagens pra compôr as texturas
	texture[0].loadTexture("textures/cube.jpg", true);
	texture[1].loadTexture("textures/tile_floor.jpg", true);
	
	// Posições do model
	glm::vec3 modelPos[] = {
		glm::vec3(0.0f, 1.0f, 0.0f),	// cubo	
		glm::vec3(0.0f, 0.0f, 0.0f)		// chão
	};

	// Escala do model
	glm::vec3 modelScale[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),	// cubo	
		glm::vec3(10.0f, 1.0f, 10.0f)	// chão
	};

	double lastTime = glfwGetTime();

	// Loop de renderização
	while (!glfwWindowShouldClose(gWindow))
	{
		//exibição e cálculo do tempo decorrido
		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		// Processo de eventos
		glfwPollEvents();
		update(deltaTime);

		// Limpar a tela
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model, view, projection;

		// Crie a view matrix
		view = fpsCamera.getViewMatrix();

		// Cria a matriz de projeção
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 200.0f);

		// Deve ser chamado ANTES de configurar uniformes porque a configuração de uniformes é feita
		//no programa de shader atualmente ativo.
		shaderProgram.use();

		// Passa as matrizes pro shader
		shaderProgram.setUniform("view", view);
		shaderProgram.setUniform("projection", projection);

		// Renderiza a cena
		for (int i = 0; i < numModels; i++)
		{
			model = glm::translate(glm::mat4(), modelPos[i]) * glm::scale(glm::mat4(), modelScale[i]);
			shaderProgram.setUniform("model", model);

			texture[i].bind(0);		// Define a textura antes de desenhar.
			mesh[i].draw();			// Renderiza o OBJ no mesh
			texture[i].unbind(0);	
		}

		// Swap dos buffers
		glfwSwapBuffers(gWindow);

		lastTime = currentTime;
	}

	glfwTerminate();

	return 0;
}

//-----------------------------------------------------------------------------
// Inicialização GLFW and OpenGL
//-----------------------------------------------------------------------------
bool initOpenGL()
{
	// Inicializa GLFW 
	if (!glfwInit())
	{
		// Se ocorrer erro
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	


	// Criar uma janela de contexto compatível com o OpenGL 3.3
	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gWindow == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(gWindow);

	// Inicializa GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// Set the required callback functions
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);
	glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

	// Mouse
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	// Dimensão viewport
	glViewport(0, 0, gWindowWidth, gWindowHeight);
	glEnable(GL_DEPTH_TEST);

	return true;
}

//-----------------------------------------------------------------------------
// É chamado sempre que uma tecla é pressionada / liberada via GLFW
//-----------------------------------------------------------------------------
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// esc fecha a janela
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	
	// alterna a exibição da malha (polígonos sem preenchimento - GL_LINE) e com as texturas (GL_FILL)
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//-----------------------------------------------------------------------------
// É chamado quando a janela é redimensionada
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}

//-----------------------------------------------------------------------------
// Chamado pelo GLFW quando o scrool do mouse é girada
//-----------------------------------------------------------------------------
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

	fov = glm::clamp(fov, 1.0, 120.0);

	fpsCamera.setFOV((float)fov);
}

//-----------------------------------------------------------------------------
// Atualiza  frames
//-----------------------------------------------------------------------------
void update(double elapsedTime)
{
	// Orientaçãp Camera 
	double mouseX, mouseY;

	// Obtém a posição atual do cursor do mouse
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	// Gira a câmera a diferença na distância do mouse a partir da tela central. Multiplique esse delta por um escalonador de velocidade
	fpsCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	// Prende o cursor do mouse no centro da tela
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	// Movimento FPS da câmera
	// Frente/trás
	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());

	// Esquerda/direita
	if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());
	else if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());

	// Up/down
	if (glfwGetKey(gWindow, GLFW_KEY_Z) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getUp());
	else if (glfwGetKey(gWindow, GLFW_KEY_X) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getUp());
}

//-----------------------------------------------------------------------------
// Calcula a média de frames por segundo, e também o tempo médio que leva
// para renderizar um quadro. Essas estatísticas são anexadas à barra de legenda da janela.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // retorna o número de segundos desde que o GLFW começou, como double float

	elapsedSeconds = currentSeconds - previousSeconds;

	// Limitar atualizações de texto a 4 vezes por segundo
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		// Definição titulo da janela
		std::ostringstream outs;
		outs.precision(3);	// casas decimais
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		// Redefinir para a próxima média.
		frameCount = 0;
	}

	frameCount++;
}