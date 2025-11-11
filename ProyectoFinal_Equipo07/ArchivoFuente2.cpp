#include <iostream>
#include <cmath>
#include <algorithm>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"
//Audio
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();
void saveFrame(void);
void resetElements(void);
void interpolation(void);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 5.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 4.5f, 0.0f);
bool active;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,5.0f, 0.0f),
	glm::vec3(2.0f,2.0f, 0.0f),
	glm::vec3(4.0f,4.0f,  0.0f),
	glm::vec3(3.0f,3.0f, 0.0f)
};

float movelightPos = 0.0f;
float movelightPos2 = 0.0f;
float angleX = 0.0f;
float angleY = 0.0f;
float radius = 30.0f; //órbita de la luz
float rot = 0.0f;
bool activanim = false;

//Tiro parabolico variables
float	g = 9.81f, v = 12.0f, ang = 12.0f, t = 45.0f, orientacionBaseball = 0.0f, movX_Baseball = 0.0f, movY_Baseball = 0.0f;
double	n = 3.1416;
float	i = 0.0f;
bool animBaseball = false;

//Throw sincronizado (niño + pelota) ---
bool playingThrow = false;
float throwTime = 0.0f;
const float throwLaunchOffset = 0.0f;

// avion con keyframes
float rotHelice = 0.0f;
glm::vec3 avionPos = glm::vec3(2.0f, 10.0f, 0.0f);
float avionRoll = 0.0f;
float avionSpeed = 3.0f;
float avionRot = 0.0f;

#define MAX_FRAMES 20  
float total_animation_time = 1.5f;
float current_animation_time = 0.0f;

typedef struct _frame {
	float avionPosX;
	float avionPosY;
	float avionPosZ;
	float avionRoll;
	float avionRot;

	float incX;
	float incY;
	float incZ;
	float incRoll;
	float incRot;

} FRAME;

FRAME KeyFrame[MAX_FRAMES]; // arreglo de keyframes
int FrameIndex = 0;
bool play = false;
int playIndex = 0;

float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

GLfloat skyboxVertices[] = {
	// Positions
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};


GLuint indices[] =
{  // Note that we start from 0!
	0,1,2,3,
	4,5,6,7,
	8,9,10,11,
	12,13,14,15,
	16,17,18,19,
	20,21,22,23,
	24,25,26,27,
	28,29,30,31,
	32,33,34,35
};

// Positions all containers
glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

// Audio
ma_engine gAudio;          // motor global
bool gAudioReady = false;  // bandera


glm::vec3 Light1 = glm::vec3(0);
//Anim
float rotBall = 0.0f;
bool AnimBall = false;
bool AnimDog = false;
float rotDog = 0.0f;
int dogAnim = 0;
float FLegs = 0.0f;
float RLegs = 0.0f;
float head = 0.0f;
float tail = 0.0f;
glm::vec3 dogPos(0.0f, 0.0f, 0.0f);
float dogRot = 0.0f;
bool step = false;

//vuelo de los pajaros y el avion
float anguloVuelo = 0.0f; // recorrido
float radioVuelo = 8.0f; // que tan amplio se recorre
glm::vec3 centroVuelo = glm::vec3(2.0f, 8.0f, 0.0f);


//Walk1
bool walk1Run = false;
glm::vec3 walk1Origin = glm::vec3(-3.5f, 0.0f, -12.5f); // donde ya lo dibujabas

glm::vec3 walk1Pos(0.0f);
float     walk1Yaw = 90.0f; // 0°=+Z, +90°=+X (arranca mirando +X)

// Tamaño del rectángulo (ajústalos a tu gusto)
float W1_W = 16.0f;
float W1_D = 3.0f;

// Waypoints del rectángulo, relativos al origin
glm::vec3 walk1Pts[4] = {
	glm::vec3(W1_W, 0.0f,   0.0f),   // 1) +X (va al costado)
	glm::vec3(W1_W, 0.0f, +W1_D),   // 2) +Z (frente del escenario)
	glm::vec3(0.0f,  0.0f, +W1_D),  // 3) -X (cruza hacia el otro lado)
	glm::vec3(0.0f,  0.0f,  0.0f)   // 4) -Z (regresa al origen)
};
int   walk1Wp = 0;
float walk1Speed = 1.3f;     // u/s (velocidad de caminar)
float walk1TurnSpeed = 120.0f;   // °/s (qué tan rápido gira)
float walk1ArriveEps = 0.05f;    // umbral para "llegó" al punto
float walk1FaceEps = 2.0f;     // cuán alineado debe estar para avanzar



//Walk2
bool walk2Run = false;
glm::vec3 walk2Origin = glm::vec3(7.0f, 0.0f, 6.0f);

glm::vec3 walk2Pos(0.0f); // posición relativa al origin
float     walk2Yaw = 0.0f; // 0° = +Z

const float W2_LIM = 2.25f;
const float W2_M = 0.03f;

glm::vec3 walk2Pts[6] = {
	glm::vec3(0.0f,   0.0f,   3.0f),
	glm::vec3(-15.5f,   0.0f,   3.0f),
	glm::vec3(-15.5f,   0.0f, -15.0f),
	glm::vec3(2.5f,   0.0f, -15.0f),
	glm::vec3(2.5f,   0.0f, 1.5f),
	glm::vec3(0.0f,   0.0f, 0.0f),
};

int   walk2Wp = 0;
float walk2Speed = 0.9f;    // u/s
float walk2TurnSpeed = 120.0f;  // °/s
float walk2ArriveEps = 0.04f;   // llegó
float walk2FaceEps = 2.0f;    // “casi” alineado


//Animación de apertura/cierre de la puerta
float speed = 50.0f;
float speed2 = 1.0f;
float tiempo = glfwGetTime() * speed2;
bool puerta1Abriendo = false;
bool puerta1Cerrando = false;
bool puerta2Abriendo = false;
bool puerta2Cerrando = false;
float rotPuertaVidrio = 0.0f;
float rotPuertaVidrio2 = 0.0f;
float velocidadAnimacion = 0.3f;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, u8"Equipo 07 - Proyecto Final", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);



	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
	Shader SkyBoxshader("Shader/SkyBox.vs", "Shader/SkyBox.frag");
	Shader animShader("Shader/anim.vs", "Shader/anim.frag");



	//models
	Model Muros((char*)"Models/EstructuraBase.obj");
	Model Bench((char*)"Models/Bench/Bench.obj");
	Model PisoMadera((char*)"Models/Piso/PisoMadera.obj");
	Model PisoPatio((char*)"Models/Patio/Patio.obj");
	Model PisoPasillo((char*)"Models/PisoPasillos/PisoPasillos.obj");
	Model Skylight((char*)"Models/Tragaluz/Tragaluz.obj");
	Model PuertasPrincipalesIzq((char*)"Models/PuertasPrincipales/PuertasPrincipalesIzq.obj");
	Model PuertasPrincipalesDer((char*)"Models/PuertasPrincipales/PuertasPrincipalesDer.obj");
	Model Pasto((char*)"Models/Pasto/Pasto.obj");
	Model Baseball((char*)"Models/Baseball/Baseball.obj");
	Model Moon((char*)"Models/Moon/Moon.obj");
	Model Globos((char*)"Models/Globos/Globo.obj");

	//Modelos de Miximo
	ModelAnim Niño((char*)"Models/Throw/Throw.dae");
	Niño.initShaders(animShader.Program);
	ModelAnim Walk1((char*)"Models/Walking/Walking.dae");
	Walk1.initShaders(animShader.Program);
	ModelAnim Walk2((char*)"Models/Walking2/Walking.dae");
	Walk2.initShaders(animShader.Program);
	ModelAnim Don((char*)"Models/Globos/Box Idle.dae");
	Don.initShaders(animShader.Program);

	ModelAnim Birds((char*)"Models/Birds/bird.fbx");
	Birds.initShaders(animShader.Program);



	/*ModelAnim Bat((char*)"Models/bat/source/Sketchfab_2023_10_26_02_42_48.fbx");
	Bat.initShaders(animShader.Program);*/

	//Pinturas Sala1
	Model Pintura1((char*)"Models/Pinturas/Pintura1.obj");
	Model Pintura6((char*)"Models/Pinturas/Pintura6.obj");
	Model Pintura7((char*)"Models/Pinturas/Pintura7.obj");
	Model Pintura8((char*)"Models/Pinturas/Pintura8.obj");
	Model Pintura10((char*)"Models/Pinturas/Pintura10.obj");
	Model Pintura11((char*)"Models/Pinturas/Pintura11.obj");
	Model Boton((char*)"Models/Pinturas/boton.obj");

	//Pinturas Sala2
	Model Pintura2((char*)"Models/Pinturas/Pintura2.obj");
	Model Pintura9((char*)"Models/Pinturas/Pintura9.obj");
	Model Pintura4((char*)"Models/Pinturas/Pintura3.obj");
	Model Pintura3((char*)"Models/Pinturas/Pintura4.obj");
	Model Pintura5((char*)"Models/Pinturas/Pintura5.obj");

	//Patio
	Model PinturaPatio1((char*)"Models/Pinturas/Pintura1P.obj");
	Model PinturaPatio2((char*)"Models/Pinturas/Pintura2P.obj");
	Model PinturaPatio3((char*)"Models/Pinturas/Pintura3P.obj");
	Model LogoPatio((char*)"Models/Pinturas/LogoH.obj");
	Model LogoPuerta((char*)"Models/Pinturas/Logo.obj");

	// Avion y frames
	Model avion_cuerpo((char*)"Models/avion/avion_cuerpo.obj");
	Model avion_helice((char*)"Models/avion/avion_helice1.obj");
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].avionPosX = 0;
		KeyFrame[i].avionPosY = 0;
		KeyFrame[i].avionPosZ = 0;
		KeyFrame[i].avionRoll = 0;
		KeyFrame[i].avionRot = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].incRoll = 0;
		KeyFrame[i].incRot = 0;
	}


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "useSolidColor"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");


	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Inicializar audio
	if (ma_engine_init(NULL, &gAudio) == MA_SUCCESS) {
		gAudioReady = true;
	}
	else {
		std::cerr << "No se pudo inicializar miniaudio.\n";
	}

	// Walk1
	//walk1Pos = glm::vec3(0.0f);
	//{
	//	glm::vec3 toT = walk1Pts[0] - walk1Pos;
	//	float desiredYaw = glm::degrees(std::atan2(toT.x, toT.z));
	//	walk1Yaw = desiredYaw;
	//	walk1Wp = 0;
	//	walk1Run = true;
	//}

	//// Walk2
	//walk2Pos = glm::vec3(0.0f);
	//{
	//	glm::vec3 toT = walk2Pts[0] - walk2Pos;
	//	float desiredYaw = glm::degrees(std::atan2(toT.x, toT.z));
	//	walk2Yaw = desiredYaw;
	//	walk2Wp = 0;
	//	walk2Run = true;
	//}

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		Animation();

		// Clear the colorbuffer
		glClearColor(0.06f, 0.06f, 0.10f, 1.0f);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);


		glm::mat4 modelTemp = glm::mat4(1.0f); //Temp



		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		lightPos.y = radius * cos(angleY);
		lightPos.x = radius * sin(angleY);

		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.15f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.06f, 0.07f, 0.10f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.18f, 0.22f, 0.30f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.20f, 0.22f, 0.30f);


		// Point light 1
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);

		//Moon point light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.02f, 0.03f, 0.06f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 0.10f, 0.14f, 0.22f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 0.16f, 0.18f, 0.26f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.018f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.0012f);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.075f);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.075f);

		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.075f);


		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));


		// Set material properties
		glUniform3f(glGetUniformLocation(lightingShader.Program, "material.ambient"), 0.07f, 0.07f, 0.07f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0.35f, 0.35f, 0.35f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "material.specular"), 0.40f, 0.40f, 0.40f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 24.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);



		//Carga de modelo 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Muros.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Bench.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PisoMadera.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PisoPatio.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pasto.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PisoPasillo.Draw(lightingShader);

		model = glm::mat4(1);
		glEnable(GL_BLEND);//Activa la funcionalidad para trabajar el canal alfa
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1050);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Desactiva el canal alfa 
		Skylight.Draw(lightingShader);
		glDisable(GL_BLEND);

		model = glm::mat4(1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(-45.771f, 6.833f, -34.167f));
		model = glm::rotate(model, glm::radians(rotPuertaVidrio), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1050);
		PuertasPrincipalesDer.Draw(lightingShader);
		glDisable(GL_BLEND);

		model = glm::mat4(1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(-45.876f, 7.659f, -50.525f));
		model = glm::rotate(model, glm::radians(rotPuertaVidrio2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Desactiva el canal alfa 
		PuertasPrincipalesIzq.Draw(lightingShader);
		glDisable(GL_BLEND);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 1.2f + movY_Baseball, 0.0f + movX_Baseball));
		model = glm::rotate(model, glm::radians(orientacionBaseball), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Baseball.Draw(lightingShader);

		////Pinturas
		//Sala1
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(-2.8f, 0.5f, 1.8f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura1.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(-0.8f, 0.3f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura6.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(2.8f, 0.5f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura7.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.8f, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura8.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(-0.8f, 0.5f, 1.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura10.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.8f, 0.5f, 1.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura11.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(1.6f, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Boton.Draw(lightingShader);

		//Sala2
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura2.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura9.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura4.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura3.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pintura5.Draw(lightingShader);

		//Patio
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PinturaPatio1.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PinturaPatio2.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PinturaPatio3.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LogoPatio.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(0.0, 0.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LogoPuerta.Draw(lightingShader);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::translate(model, glm::vec3(-1.9, -5.6f, 4.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Globos.Draw(lightingShader);

		//avion
		lightingShader.Use();
		modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		model = glm::mat4(1.0f);

		model = glm::translate(model, avionPos);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f + avionRot), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(avionRoll), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));

		modelTemp = model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		avion_cuerpo.Draw(lightingShader);
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.5f));

		//model = glm::rotate(model, glm::radians(rotHelice), glm::vec3(0.0f, 0.0f, 1.0f)); no rota sobre su propio eje
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		avion_helice.Draw(lightingShader);

		glBindVertexArray(0);


		/*_______________________________Personaje Animado__________________1_________*/
		animShader.Use();
		//glDisable(GL_CULL_FACE);

		modelLoc = glGetUniformLocation(animShader.Program, "model");
		viewLoc = glGetUniformLocation(animShader.Program, "view");
		projLoc = glGetUniformLocation(animShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glUniform3f(glGetUniformLocation(animShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(animShader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.ambient"), 0.15f, 0.15f, 0.18f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.diffuse"), 0.35f, 0.35f, 0.45f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.specular"), 0.30f, 0.30f, 0.35f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.direction"), 0.0f, -1.0f, -1.0f);
		view = camera.GetViewMatrix();

		model = glm::mat4(1);
		//model = glm::translate(model, glm::vec3(PosIni.x + 5.0f, PosIni.y - 1.0f, PosIni.z));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.008f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Niño.Draw(animShader);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(0.008f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Don.Draw(animShader);

		model = glm::mat4(1);
		model = glm::translate(model, walk1Origin + walk1Pos);
		model = glm::rotate(model, glm::radians(walk1Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.008f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Walk1.Draw(animShader);


		model = glm::mat4(1);
		model = glm::translate(model, walk2Origin + walk2Pos);
		model = glm::rotate(model, glm::radians(walk2Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.008f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Walk2.Draw(animShader);
		glBindVertexArray(0);


		model = glm::mat4(1);
		glm::vec3 nuevaPos = glm::vec3((centroVuelo.x + radioVuelo * cos(anguloVuelo)) / 2, centroVuelo.y, (centroVuelo.z + radioVuelo * sin(anguloVuelo)));
		model = glm::translate(model, nuevaPos);
		model = glm::scale(model, glm::vec3(0.018f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // volteamos
		model = glm::rotate(model, -anguloVuelo + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotacion
		model = glm::rotate(model, glm::radians(15.0f * sin(anguloVuelo * 4.0f)), glm::vec3(1.0f, 0.0f, 0.1f)); // rotacion de inclinacion
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Birds.Draw(animShader);
		glBindVertexArray(0);



		//glEnable(GL_CULL_FACE);

		//model = glm::mat4(1);
		//glEnable(GL_BLEND);//Avtiva la funcionalidad para trabajar el canal alfa
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		//model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	 //   Ball.Draw(lightingShader); 
		//glDisable(GL_BLEND);  //Desactiva el canal alfa 
		//glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos + movelightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		Moon.Draw(lightingShader);
		// Draw the light object (using light's vertex attributes)

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[1]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//Moon.Draw(lampShader);

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[2]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[3]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	if (gAudioReady) {
		ma_engine_uninit(&gAudio);
	}

	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);


	}

	if (keys[GLFW_KEY_T])
	{
		angleY += 0.05f;
	}
	if (keys[GLFW_KEY_G])
	{
		angleY -= 0.05f;
	}
	/*
	if (keys[GLFW_KEY_Y])
	{
		pointLightPositions[0].y += 0.01f;
	}

	if (keys[GLFW_KEY_H])
	{
		pointLightPositions[0].y -= 0.01f;
	}
	if (keys[GLFW_KEY_U])
	{
		pointLightPositions[0].z -= 0.1f;
	}
	if (keys[GLFW_KEY_J])
	{
		pointLightPositions[0].z += 0.01f;
	}
	*/
	if (keys[GLFW_KEY_K]) walk2Run = !walk2Run;
	if (keys[GLFW_KEY_L]) walk1Run = !walk1Run;

	if (!play)
	{
		float targetRoll = 0.0f;
		float rollSpeed = 7.0f;
		float maxRoll = 20.0f;

		if (keys[GLFW_KEY_KP_4])
		{
			avionPos.x -= avionSpeed * deltaTime;
			targetRoll = maxRoll;
		}
		if (keys[GLFW_KEY_KP_6])
		{
			avionPos.x += avionSpeed * deltaTime;
			targetRoll = -maxRoll;
		}
		if (keys[GLFW_KEY_KP_8])
		{
			avionPos.z -= avionSpeed * deltaTime;
		}
		if (keys[GLFW_KEY_KP_5])
		{
			avionPos.z += avionSpeed * deltaTime;
		}
		if (keys[GLFW_KEY_KP_9])
		{
			avionPos.y += avionSpeed * deltaTime;
		}
		if (keys[GLFW_KEY_KP_7])
		{
			avionPos.y -= avionSpeed * deltaTime;
		}
		avionRoll += (targetRoll - avionRoll) * rollSpeed * deltaTime;

		float yawSpeed = 80.0f; // Grados por segundo (ajústalo)

		if (keys[GLFW_KEY_1])
		{
			avionRot += yawSpeed * deltaTime;
		}
		// Girar Derecha (Tecla '2')
		if (keys[GLFW_KEY_2])
		{
			avionRot -= yawSpeed * deltaTime;
		}
	}


}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(0.2f, 0.8f, 1.0f);

		}
		else
		{
			Light1 = glm::vec3(0);
		}
	}
	if (keys[GLFW_KEY_P]) {

		animBaseball ^= true;

	}
	if (keys[GLFW_KEY_R]) {
		walk2Pos = glm::vec3(0.0f);
		walk2Yaw = 0.0f; 
		walk2Wp = 0;
		walk2Run = true;
	}
	if (keys[GLFW_KEY_O]) {
		walk1Pos = glm::vec3(0.0f);
		walk1Yaw = 90.0f; // mirando +X
		walk1Wp = 0;
		walk1Run = true;
	}
	if (keys[GLFW_KEY_Z]) {
		if (gAudioReady) {
			ma_engine_play_sound(&gAudio, "Audio/Z.mp3", NULL);
		}
	}
	if (keys[GLFW_KEY_X]) {
		if (gAudioReady) {
			ma_engine_play_sound(&gAudio, "Audio/X.mp3", NULL);
		}
	}
	if (keys[GLFW_KEY_C]) {
		if (gAudioReady) {
			ma_engine_play_sound(&gAudio, "Audio/Prueba.mp3", NULL);
		}
	}
	if (keys[GLFW_KEY_V]) {
		if (gAudioReady) {
			ma_engine_play_sound(&gAudio, "Audio/Prueba.mp3", NULL);
		}
	}
	if (keys[GLFW_KEY_B]) {
		if (gAudioReady) {
			ma_engine_play_sound(&gAudio, "Audio/Prueba.mp3", NULL);
		}
	}
	if (keys[GLFW_KEY_N]) {
		if (gAudioReady) {
			ma_engine_play_sound(&gAudio, "Audio/Prueba.mp3", NULL);
		}
	}

	//Puerta
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		puerta1Abriendo = true;  puerta1Cerrando = false;
		puerta2Abriendo = true;  puerta2Cerrando = false;
	}
	if (key == GLFW_KEY_COMMA && action == GLFW_PRESS) {
		puerta1Abriendo = false; puerta1Cerrando = true;
		puerta2Abriendo = false; puerta2Cerrando = true;
	}

	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		saveFrame();
	}

	// Reproducir Animación 
	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			printf("Reproduciendo animacion...\n");
			resetElements();
			interpolation();
			play = true;
			playIndex = 0;
			current_animation_time = 0.0f;
		}
		else
		{
			play = false;
		}
	}
}
void Animation() {
	rotHelice += 150.0f * deltaTime; //helice del avion
	//Animación de las puertas
	if (puerta1Abriendo) {
		rotPuertaVidrio += velocidadAnimacion;
		if (rotPuertaVidrio > 70.0f) rotPuertaVidrio = 70.0f;
	}
	if (puerta1Cerrando) {
		rotPuertaVidrio -= velocidadAnimacion;
		if (rotPuertaVidrio < 0.0f) rotPuertaVidrio = 0.0f;
	}

	// Puerta 2: 0° a −70° (abre al lado opuesto)
	if (puerta2Abriendo) {
		rotPuertaVidrio2 -= velocidadAnimacion;
		if (rotPuertaVidrio2 < -70.0f) rotPuertaVidrio2 = -70.0f;
	}
	if (puerta2Cerrando) {
		rotPuertaVidrio2 += velocidadAnimacion;
		if (rotPuertaVidrio2 > 0.0f) rotPuertaVidrio2 = 0.0f;
	}

	anguloVuelo += 0.4f * deltaTime;
	if (AnimBall)
	{
		rotBall += 0.4f;
	}

	// -------- Walk1 path (rectángulo, giro progresivo) --------
	if (walk1Run) {
		glm::vec3 target = walk1Pts[walk1Wp];
		glm::vec3 toT = target - walk1Pos;
		float distXZ = glm::length(glm::vec2(toT.x, toT.z));

		//Yaw deseado (0°=+Z, +90°=+X)
		float desiredYaw = glm::degrees(std::atan2(toT.x, toT.z));

		//Diferencia angular mínima en [-180,180]
		float diff = desiredYaw - walk1Yaw;
		while (diff > 180.0f)  diff -= 360.0f;
		while (diff < -180.0f) diff += 360.0f;

		//Giro progresivo
		float maxStep = walk1TurnSpeed * deltaTime;
		if (std::abs(diff) > maxStep) walk1Yaw += (diff > 0 ? maxStep : -maxStep);
		else                          walk1Yaw = desiredYaw;

		if (std::abs(diff) < walk1FaceEps) {
			float yaw = glm::radians(walk1Yaw);
			glm::vec3 fwd(std::sin(yaw), 0.0f, std::cos(yaw));
			walk1Pos += fwd * (walk1Speed * deltaTime);
		}

		if (distXZ < walk1ArriveEps) {
			int prev = walk1Wp;
			walk1Wp = (walk1Wp + 1) % 4;

			if (prev == 3 && walk1Wp == 0) {
				walk1Pos = glm::vec3(0.0f);
				while (walk1Yaw > 180.0f)  walk1Yaw -= 360.0f;
				while (walk1Yaw < -180.0f) walk1Yaw += 360.0f;
			}
		}
	}


	// -------- Walk2 path (con giro progresivo) --------
	if (walk2Run) {
		glm::vec3 target = walk2Pts[walk2Wp];
		glm::vec3 toT = target - walk2Pos;
		float distXZ = glm::length(glm::vec2(toT.x, toT.z));

		//Yaw deseado hacia el objetivo (0°=+Z, +90°=+X)
		float desiredYaw = glm::degrees(std::atan2(toT.x, toT.z));

		//Diferencia angular mínima en [-180,180]
		float diff = desiredYaw - walk2Yaw;
		while (diff > 180.0f) diff -= 360.0f;
		while (diff < -180.0f) diff += 360.0f;

		//Giro progresivo limitado por turnSpeed
		float maxStep = walk2TurnSpeed * deltaTime;
		if (std::abs(diff) > maxStep) walk2Yaw += (diff > 0 ? maxStep : -maxStep);
		else                          walk2Yaw = desiredYaw;

		if (std::abs(diff) < walk2FaceEps) {
			float yaw = glm::radians(walk2Yaw);
			glm::vec3 fwd(std::sin(yaw), 0.0f, std::cos(yaw));
			walk2Pos += fwd * (walk2Speed * deltaTime);
		}

		if (distXZ < walk2ArriveEps) {
			int prev = walk2Wp;
			walk2Wp = (walk2Wp + 1) % 6;

			if (prev == 5 && walk2Wp == 0) {
				walk2Pos = glm::vec3(0.0f);
				while (walk2Yaw > 180.0f)  walk2Yaw -= 360.0f;
				while (walk2Yaw < -180.0f) walk2Yaw += 360.0f;
			}

		}
	}


	if (animBaseball)
	{
		orientacionBaseball -= 6.1f;
		//i += 0.12f;
		i += deltaTime / 2;
		movX_Baseball = 0 + i * v * cos(ang * n / 180);
		movY_Baseball = 0 + (i * v * sin(ang * n / 180) - (g * i * i) / 2);
		if (movY_Baseball <= 0.0f)
		{
			i = 0.0f;
			movX_Baseball = 0.0f;
			movY_Baseball = 0.0f;
			animBaseball = false;
		}
	}

	if (play)
	{
		if (current_animation_time >= total_animation_time)
		{

			avionPos.x = KeyFrame[playIndex + 1].avionPosX;
			avionPos.y = KeyFrame[playIndex + 1].avionPosY;
			avionPos.z = KeyFrame[playIndex + 1].avionPosZ;
			avionRoll = KeyFrame[playIndex + 1].avionRoll;
			avionRot = KeyFrame[playIndex + 1].avionRot;

			playIndex++;

			if (playIndex > FrameIndex - 2)
			{
				printf("Termina animacion\n");
				playIndex = 0;
				play = false;
			}
			else
			{
				current_animation_time = 0.0f;
				interpolation();
			}
		}
		else
		{

			avionPos.x += KeyFrame[playIndex].incX * deltaTime;
			avionPos.y += KeyFrame[playIndex].incY * deltaTime;
			avionPos.z += KeyFrame[playIndex].incZ * deltaTime;
			avionRoll += KeyFrame[playIndex].incRoll * deltaTime;
			avionRot += KeyFrame[playIndex].incRot * deltaTime;

			current_animation_time += deltaTime;
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}


// Funciones de keyframes
void saveFrame(void)
{
	if (FrameIndex < MAX_FRAMES)
	{
		printf("Guardando KeyFrame %d\n", FrameIndex);
		KeyFrame[FrameIndex].avionPosX = avionPos.x;
		KeyFrame[FrameIndex].avionPosY = avionPos.y;
		KeyFrame[FrameIndex].avionPosZ = avionPos.z;
		KeyFrame[FrameIndex].avionRoll = avionRoll;
		KeyFrame[FrameIndex].avionRot = avionRot;
		FrameIndex++;
	}
	else
	{
		printf("Arreglo de KeyFrames lleno\n");
	}
}

void resetElements(void)
{
	avionPos.x = KeyFrame[0].avionPosX;
	avionPos.y = KeyFrame[0].avionPosY;
	avionPos.z = KeyFrame[0].avionPosZ;
	avionRoll = KeyFrame[0].avionRoll;
	avionRot = KeyFrame[0].avionRot;
}

void interpolation(void)
{
	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].avionPosX - KeyFrame[playIndex].avionPosX) / total_animation_time;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].avionPosY - KeyFrame[playIndex].avionPosY) / total_animation_time;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].avionPosZ - KeyFrame[playIndex].avionPosZ) / total_animation_time;
	KeyFrame[playIndex].incRoll = (KeyFrame[playIndex + 1].avionRoll - KeyFrame[playIndex].avionRoll) / total_animation_time;
	KeyFrame[playIndex].incRot = (KeyFrame[playIndex + 1].avionRot - KeyFrame[playIndex].avionRot) / total_animation_time;
}