/*---------------------------------------------------------*/
/* ------------------- Proyecto Final ---------------------*/
/* -------------------     Equipo:    ---------------------*/
/* ---------------- Juarez Aguilar Osmar ------------------*/
/* ---------------- Meza Ortega Fernando ------------------*/
/*---------------------    2021-1   ------------------------*/
#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h> 	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y modelos
#include <glm/gtc/matrix_transform.hpp>	//camera y modelos
#include <glm/gtc/type_ptr.hpp>
#include<time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texturas

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader.h> 
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

void myData(void);
void LoadTextures(void);
unsigned int generateTextures(char*, bool);

// Opciones
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//
GLuint VBO, VAO, EBO;

// Camara
Camera camera(glm::vec3(0.0f, -115.0f, 450.0f));
float MovementSpeed = 0.2f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisegundos // 1000 milisegundos == 1 sec
double	deltaTime = 1.0f,
lastFrame = 0.0f;

//Iluminaci�n
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

// Posiciones
float x = 0.0f;
float y = 0.0f;
float movAuto_z = 0.0f;
bool animacion = false;


//Keyframes (Manipulaci�n y dibujo)
float	posX = 0.0f,
posY = 0.0f,
posZ = 0.0f,
rotRodIzq = 0.0f,
giroMonito = 0.0f;
float	incX = 0.0f,
incY = 0.0f,
incZ = 0.0f,
rotInc = 0.0f,
giroMonitoInc = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para guardar cuadros clave
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

//Variables para cargar las texturas
unsigned int	t_wall,
				t_inside_wall,
				t_floor,
				t_ceiling,
				t_pnbll_table,
				t_pnbll_board;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}


void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}
}

unsigned int generateTextures(const char* filename, bool alfa)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{
	t_wall = generateTextures("Texturas/bricks.jpg", 0);
	t_inside_wall = generateTextures("Texturas/inside_wall.jpg", 0);
	t_floor = generateTextures("Texturas/floor.jpg", 0);
	t_ceiling = generateTextures("Texturas/ceiling.jpg", 0);
	t_pnbll_table = generateTextures("Texturas/pinball_table.jpg", 0);
	t_pnbll_board = generateTextures("Texturas/pinball_board.jpg", 0);
}

void myData()
{
	float vertices[] = {
		//CUBO
		// positions          // texture coords
		 0.5f,  0.5f, 0.5f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.5f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.5f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.5f,   0.0f, 1.0f,  // top left 

		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7 // top right
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3 // bottom right
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 // bottom left- Trasera
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6 // top left 

		-0.5f, 0.5f, 0.5f,		1.0f, 1.0f, //V4 - Izq
		-0.5f, -0.5f, 0.5f,		1.0f, 0.0f, //V0
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, //V3
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, //V7

		0.5f, 0.5f, 0.5f,		1.0f, 1.0f, //V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,//V1
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f, //V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f, //V6

		-0.5f, 0.5f, 0.5f,		1.0f, 1.0f, //V4 - Sup
		-0.5f, 0.5f, -0.5f,		1.0f, 0.0f, //V7
		0.5f, 0.5f, -0.5f,		0.0f, 0.0f, //V6
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f, //V5

		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f, //V3
		-0.5f, -0.5f, 0.5f,		1.0f, 0.0f, //V0 - Inf
		0.5f, -0.5f, 0.5f,		0.0f, 0.0f, //V1
		0.5f, -0.5f, -0.5f,		0.0f, 1.0f, //V2

		//TRIANGULO
		-0.5f, -0.5f, 0.0f,     0.0f, 0.0f,	//TRI
		0.5f, -0.5f, 0.0f, 		1.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 		0.5f, 1.0f,

		//TRAPECIO
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//FRONTAL
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,
		0.25f, 0.5f, 0.5f, 		0.0f, 0.0f,
		-0.25, 0.5f, 0.5f, 		0.0f, 1.0f,

		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//TRASERA
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,
		-0.25f, 0.5f, -0.5f, 	0.0f, 0.0f,
		0.25, 0.5f, -0.5f, 		0.0f, 1.0f,

		-0.25f, 0.5f, -0.5f, 	1.0f, 1.0f,
		-0.25, 0.5f, 0.5f, 		1.0f, 0.0f,
		-0.5f,-0.5f, 0.5f,		0.0f, 0.0f,	//IZQ
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,

		0.25f, 0.5f, 0.5f,		1.0f, 1.0f,	//DER
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 	0.0f, 0.0f,
		0.25f, 0.5f, -0.5f,		0.0f, 1.0f,

		-0.25f, 0.5f, 0.5f,		1.0f, 1.0f,	//SUP
		0.25f, 0.5f, 0.5f,		1.0f, 0.0f,
		0.25f, 0.5f, -0.5f, 	0.0f, 0.0f,
		-0.25, 0.5f, -0.5f,		0.0f, 1.0f,

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//INF
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 	0.0f, 0.0f,
		0.5, -0.5f, 0.5f,		0.0f, 1.0f

	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gaming Room - Final Project", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Datos a utilizar
	LoadTextures();
	myData();
	//my_sphere.init();
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	Shader projectionShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs");
	// -------------------------
	//Shader staticShader("Shaders/lightVertex.vs", "Shaders/lightFragment.fs");
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	Model tv("resources/objects/television/television_2.obj");
	Model billar("resources/objects/mesa_billar/mesa_billar.obj");
	Model sofa_set("resources/objects/sofa_set/sofa.obj");

	//Inicializaci�n de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.getPosition());
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.8f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(0.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

		// -------------------------------------------------------------------------------------------------------------------------
		// ESCENARIO - MODELOS
		// -------------------------------------------------------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		// -------------------------------------------------------------------------------------------------------------------------
		// Piso
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -150.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Television
		model = glm::translate(glm::mat4(1.0f), glm::vec3(70.0f, -149.0f, -215.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		tv.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Sofa
		model = glm::translate(glm::mat4(1.0f), glm::vec3(70.0f, -173.0f, -115.0f));
		model = glm::scale(model, glm::vec3(4.5f, 4.5f, 4.5f));
		staticShader.setMat4("model", model);
		sofa_set.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Mesa de billar 
		model = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f, -155.5f,170.0f));
		model = glm::scale(model, glm::vec3(12.0f, 12.0f, 12.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		staticShader.setMat4("model", model);
		billar.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------
		
		// -------------------------------------------------------------------------------------------------------------------------
		// Flippers
		// -------------------------------------------------------------------------------------------------------------------------


		// -------------------------------------------------------------------------------------------------------------------------
		

		// -------------------------------------------------------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------------------------------------
		// ESCENARIO - FIGURAS
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// ESTRUCTURA DEL CUARTO 
		projectionShader.use();
		// pass them to the shaders
		projectionShader.setMat4("model", model);
		projectionShader.setMat4("view", view);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		projectionShader.setMat4("projection", projection);

		glBindVertexArray(VAO);
		//Colocar c�digo aqu�
		//este algortmo solicita que al final se dibujen los objetos con blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//Fuente valor de alpha, disminuir en uno

		//Estructura externa
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -50.0f, 0.0f));
		model = glm::scale(model, glm::vec3(501.0f, 201.0f, 551.0f));
		projectionShader.setMat4("model", model);
		//la textura se puede combinar con algun color, si se pone en blanco no afectara la imagen
		projectionShader.setVec3("aColor", 1.0f, 0.8f, 0.0f);
		glBindTexture(GL_TEXTURE_2D, t_wall);//
		glDrawArrays(GL_QUADS, 0, 24);

		//estructura interna - Paredes
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -50.0f, 0.0f));
		model = glm::scale(model, glm::vec3(500.0f, 200.0f, 550.0f));
		projectionShader.setMat4("model", model);
		projectionShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_inside_wall);//
		glDrawArrays(GL_QUADS, 0, 24);

		// Piso
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -149.0f, 0.0f));
		model = glm::scale(model, glm::vec3(500.0f, 0.25f, 550.0f));
		projectionShader.setMat4("model", model);
		projectionShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_floor);//
		glDrawArrays(GL_QUADS, 0, 24);

		// Techo
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 50.0f, 0.0f));
		model = glm::scale(model, glm::vec3(500.0f, 0.25f, 550.0f));
		projectionShader.setMat4("model", model);
		projectionShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_ceiling);//
		glDrawArrays(GL_QUADS, 0, 24);

		// -------------------------------------------------------------------------------------------------------------------------
		// Mesa del Pinball
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, -120.0f, 50.0f));
		model = glm::rotate(model, glm::radians(5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(85.0f, 20.0f, 150.f));
		projectionShader.setMat4("model", model);
		projectionShader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glBindTexture(GL_TEXTURE_2D, t_pnbll_table);
		glDrawArrays(GL_QUADS, 0, 24);

		//patas
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, -140.0f, 115.0f));
		model = glm::scale(model, glm::vec3(10.0f, 20.0f, 8.f));
		projectionShader.setMat4("model", model);
		projectionShader.setVec3("aColor", glm::vec3(0.0f, 0.0f, 0.0f));
		glDrawArrays(GL_QUADS, 0, 24); //A

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-120.0f, -140.0f, 115.0f));
		model = glm::scale(model, glm::vec3(10.0f, 20.0f, 8.f));
		projectionShader.setMat4("model", model);
		projectionShader.setVec3("aColor", glm::vec3(0.0f, 0.0f, 0.0f));
		glDrawArrays(GL_QUADS, 0, 24); //B

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-180.0f, -137.0f, -10.0f));
		model = glm::scale(model, glm::vec3(10.0f, 25.0f, 8.f));
		projectionShader.setMat4("model", model);
		projectionShader.setVec3("aColor", glm::vec3(0.0f, 0.0f, 0.0f));
		glDrawArrays(GL_QUADS, 0, 24); //C

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-120.0f, -137.0f, -10.0f));
		model = glm::scale(model, glm::vec3(10.0f, 25.0f, 8.f));
		projectionShader.setMat4("model", model);
		projectionShader.setVec3("aColor", glm::vec3(0.0f, 0.0f, 0.0f));
		glDrawArrays(GL_QUADS, 0, 24); //D
		projectionShader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));

		//Tablero pinball - Figura
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, -110.0f, 50.7f));
		model = glm::rotate(model, glm::radians(5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(85.0f, 0.5f, 150.f));
		projectionShader.setMat4("model", model);
		projectionShader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glBindTexture(GL_TEXTURE_2D, t_pnbll_board);
		glDrawArrays(GL_QUADS, 0, 24);
		

		glDisable(GL_BLEND);
		glBindVertexArray(0);
		//--------------------------------------------------------------------------------------------------------------------------


		
		// -------------------------------------------------------------------------------------------------------------------------
		// TERMINA ESCENARIO
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, (float)deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

/*void my_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	//To play KeyFrame animation
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "play" << std::endl;
			resetElements();
			//First Interpolation
			interpolation();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}
	}
	//To Save a KeyFrame
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}*/

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}