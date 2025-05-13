/*
PROYECTO FINAL
COMPUTACION GRAFICA
EQUIPO 8
317290967
319296738
319281998
*/
#include <iostream>
#include <cmath>

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
// Variable para el color de la luz
glm::vec3 Light1 = glm::vec3(1.0f, 1.0f, 0.0f);
float ballPosY = 0.0f;
bool movingUp = true;
bool AnimBall = true;

// Función para renderizar la bola con movimiento vertical
void renderBall(Shader& shader, Model& Ball) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, ballPosY, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	Ball.Draw(shader);
}

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();
void cargarKeyframesSillas(std::string archivoEntrada, std::string archivoSalida);
void dibujarParteAnimada(Model& parte, float desfase, GLint modelLoc, Shader& shader);

// Window dimensions
const GLuint WIDTH = 2200, HEIGHT = 2200;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(3.0f, 10.0f, 0.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;
bool recorridoActivo = false;
bool recorridoActivo2 = false;

// ================= VARIABLES DE ANIMACIÓN =================

glm::vec3 posLabNuevo = glm::vec3(-16.715f, 250.0f, 36.575f); // Más alto para caída larga
float velocidadY = 0.0f;
float gravedad = -1.0f;
float reboteFactor = 0.8f;

bool animacionActivaLabNuevo = false;
bool animacionTerminada = false;

int numRebotes = 0;
int rebotesMaximos = 6;
bool enRebote = true;
bool impactoFinal = false;
bool faseAplastamientoActiva = false;

float escalaY_LabViejo = 1.0f;
float alphaLabViejo = 1.0f; // transparencia del lab viejo

glm::vec3 posicionLabViejo = glm::vec3(0.0f, 0.0f, 0.0f); // El lab viejo está en el origen

float tiempoReaccionImpacto = 0.0f;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(2.656f, 14.766f, 14.766f),//Luces calidas
	glm::vec3(2.656f, 14.766f,  -5.8f),
	glm::vec3(2.656f, 14.766f, -10.766f),
	glm::vec3(16.73f, 14.766f, 19.53f),
	glm::vec3(16.73f, 14.766f, 14.977f),
	glm::vec3(16.73f, 14.766f,  -5.52f),
	glm::vec3(16.73f, 14.766f, -10.073f),
	glm::vec3(31.739f, 14.766f, 19.53f),
	glm::vec3(31.739f, 14.766f, 14.977f),
	glm::vec3(31.739f, 14.766f,  -5.52f),
	glm::vec3(31.739f, 14.766f, -10.073f),
	glm::vec3(28.66f, 7.83f, 20.44f),//Luces azules
	glm::vec3(18.56f, 7.83f, 10.68f),
	glm::vec3(18.56f, 7.83f, 0.97f),
	glm::vec3(7.75f, 7.83f,  -8.92f),
};

std::vector<glm::vec3> lucesRelativasLabNuevo = {
	glm::vec3(-1.21f, 77.78f,  27.009f),
	glm::vec3(-1.21f, 77.78f,  16.97f),
	glm::vec3(16.35f, 77.78f,  27.009f),
	glm::vec3(16.35f, 77.78f,  16.97f),
	glm::vec3(31.74f, 77.78f,  27.009f),
	glm::vec3(31.74f, 77.78f,  16.97f),
	glm::vec3(-1.21f, 77.78f,  2.71f),
	glm::vec3(-1.21f, 77.78f,  -7.31f),
	glm::vec3(16.35f, 77.78f,  2.71f),
	glm::vec3(16.35f, 77.78f,  -7.31f),
	glm::vec3(31.74f, 77.78f,  2.71f),
	glm::vec3(31.74f, 77.78f,  -7.31f),
	glm::vec3(30.49f, 60.16f,   21.73f),
	glm::vec3(30.49f, 60.16f,   7.15f),
	glm::vec3(30.49f, 60.16f,   -8.23f),
	glm::vec3(20.49f, 60.16f,   21.73f),
	glm::vec3(20.49f, 60.16f,   7.15f),
	glm::vec3(20.49f, 60.16f,   -8.23f),
	glm::vec3(8.65f, 60.16f,    21.73f),
	glm::vec3(8.65f, 60.16f,    7.15f),
	glm::vec3(8.65f, 60.16f,    -8.23f),
	glm::vec3(8.09f, 67.75f,  24.80f),
	glm::vec3(8.09f, 67.75f,  0.46f),
	glm::vec3(19.33f, 67.75f, 11.92f),
	glm::vec3(30.49f, 67.75f, -11.01f),
	glm::vec3(-11.19f, 71.08f, 2.88f)
};

// === Variables para animación tipo remolino de sillas viejas ===

// Sillas Viejas (Remolino)
int frameSillasViejas1 = 0, frameSillasViejas2 = 0, frameSillasViejas3 = 0;
bool animarSillasViejas1 = false, animarSillasViejas2 = false, animarSillasViejas3 = false;
int delayRemolino1 = 0, delayRemolino2 = 0, delayRemolino3 = 0;
int velocidadRemolino = 10;
int desfaseRemolino2 = 10;  // Frames de espera antes de empezar la 2da
int desfaseRemolino3 = 20;  // Frames de espera antes de empezar la 3ra

// Sillas Nuevas (Caída)
int frameSillasNuevas1 = 0, frameSillasNuevas2 = 0, frameSillasNuevas3 = 0;
bool animarSillasNuevas1 = false, animarSillasNuevas2 = false, animarSillasNuevas3 = false;
int delayCaida1 = 0, delayCaida2 = 0, delayCaida3 = 0;
int velocidadCaida = 5;
int desfaseCaida2 = 10;
int desfaseCaida3 = 20;

struct FrameRemolino {
	glm::vec3 posicion;
	float rotacionY;
	float escala;
};

std::vector<FrameRemolino> keyframesRemolino;
bool animarSillasViejas = false;
int frameSillasViejas = 0;
int delayRemolino = 0;


void prepararKeyframesRemolino() {
	keyframesRemolino.clear();

	// 🚀 Elevación inicial
	keyframesRemolino.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), glm::radians(0.0f), 1.0f });
	keyframesRemolino.push_back({ glm::vec3(0.0f, 4.5f, 0.0f), glm::radians(0.0f), 1.0f });

	// 🌀 Remolino principal (ya existente)
	keyframesRemolino.push_back({ glm::vec3(2.0f, 4.5f, 0.0f), glm::radians(0.0f), 1.0f });
	keyframesRemolino.push_back({ glm::vec3(1.6f, 4.0f, 1.2f), glm::radians(36.0f), 0.95f });
	keyframesRemolino.push_back({ glm::vec3(1.0f, 3.5f, 1.6f), glm::radians(72.0f), 0.9f });
	keyframesRemolino.push_back({ glm::vec3(0.0f, 3.0f, 2.0f), glm::radians(108.0f), 0.85f });
	keyframesRemolino.push_back({ glm::vec3(-1.0f, 2.5f, 1.6f), glm::radians(144.0f), 0.8f });
	keyframesRemolino.push_back({ glm::vec3(-1.6f, 2.0f, 1.2f), glm::radians(180.0f), 0.75f });
	keyframesRemolino.push_back({ glm::vec3(-2.0f, 1.5f, 0.0f), glm::radians(216.0f), 0.7f });
	keyframesRemolino.push_back({ glm::vec3(-1.6f, 1.0f, -1.2f), glm::radians(252.0f), 0.6f });
	keyframesRemolino.push_back({ glm::vec3(-1.0f, 0.5f, -1.6f), glm::radians(288.0f), 0.5f });
	keyframesRemolino.push_back({ glm::vec3(0.0f, 0.0f, -2.0f), glm::radians(324.0f), 0.4f });

	// 🌀 NUEVOS 10 KEYFRAMES (segunda vuelta más pequeña y más baja)
	keyframesRemolino.push_back({ glm::vec3(0.8f, -0.2f, -0.8f), glm::radians(360.0f), 0.35f });
	keyframesRemolino.push_back({ glm::vec3(0.6f, -0.5f, -0.6f), glm::radians(396.0f), 0.3f });
	keyframesRemolino.push_back({ glm::vec3(0.4f, -0.8f, -0.4f), glm::radians(432.0f), 0.25f });
	keyframesRemolino.push_back({ glm::vec3(0.2f, -1.1f, -0.2f), glm::radians(468.0f), 0.2f });
	keyframesRemolino.push_back({ glm::vec3(0.0f, -1.4f, 0.0f), glm::radians(504.0f), 0.15f });
	keyframesRemolino.push_back({ glm::vec3(-0.2f, -1.7f, 0.2f), glm::radians(540.0f), 0.1f });
	keyframesRemolino.push_back({ glm::vec3(-0.4f, -2.0f, 0.4f), glm::radians(576.0f), 0.08f });
	keyframesRemolino.push_back({ glm::vec3(-0.6f, -2.3f, 0.6f), glm::radians(612.0f), 0.05f });
	keyframesRemolino.push_back({ glm::vec3(-0.8f, -2.6f, 0.8f), glm::radians(648.0f), 0.02f });
	keyframesRemolino.push_back({ glm::vec3(0.0f, -3.0f, 0.0f), glm::radians(684.0f), 0.01f });  // Final
}

// === Variables para animación de caída y rebote de sillas nuevas ===
// Estructura para keyframes de caída
struct FrameCaida {
	glm::vec3 posicion;
	float escala;
};

std::vector<FrameCaida> keyframesCaidaSillasNuevas;
bool animarSillasNuevas = false;
int frameSillasNuevas = 0;
int delayCaida = 0;
bool labYaAterrizo = false; // se activa cuando el lab ya está abajo

void prepararKeyframesCaidaSillasNuevas() {
	keyframesCaidaSillasNuevas.clear();

	glm::vec3 base = labYaAterrizo ? glm::vec3(11.417f, -14.0f, 60.445f) : glm::vec3(-16.715f, 250.0f, 36.575f);

	// 🚀 Caída inicial
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 40.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 35.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 30.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 25.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 20.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 10.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 5.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base, 1.0f }); // Impacto fuerte

	// 🏀 Primer rebote alto
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 4.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 1.5f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base, 1.0f });

	// 🏀 Segundo rebote
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 2.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 0.8f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base, 1.0f });

	// 🏀 Tercer rebote pequeño
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 1.0f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 0.4f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base, 1.0f });

	// 🏀 Cuarto rebote mínimo
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 0.5f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base + glm::vec3(0.0f, 0.2f, 0.0f), 1.0f });
	keyframesCaidaSillasNuevas.push_back({ base, 1.0f });

	// Final estable
	keyframesCaidaSillasNuevas.push_back({ base, 1.0f });  // Frame 22 (reposo total)
}

//================= ANIMACION GABNETES =================
std::vector<FrameCaida> keyframesGabinetes[12];
bool animarGabinetes[12] = { false };
int frameGabinetes[12] = { 0 };
int delayGabinetes[12] = { 0 };
int velocidadGabinete = 5;
int desfaseGabinetes[12] = { 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55 };

void prepararKeyframesGabinetes() {
	glm::vec3 baseGabinete;

	if (labYaAterrizo) {
		baseGabinete = glm::vec3(11.417f, -14.0f, 60.445f);  // Igual que las sillas nuevas
	}
	else {
		baseGabinete = glm::vec3(-16.715f, 250.0f, 36.575f); // Si el lab aún no aterriza
	}

	for (int i = 0; i < 12; ++i) {
		keyframesGabinetes[i].clear();

		// 🚀 Caída inicial
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 40.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 35.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 30.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 25.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 20.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 15.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 10.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 5.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete, 1.0f });  // Impacto fuerte

		// 🏀 Primer rebote alto
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 4.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 1.5f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete, 1.0f });

		// 🏀 Segundo rebote
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 2.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 0.8f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete, 1.0f });

		// 🏀 Tercer rebote pequeño
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 1.0f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 0.4f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete, 1.0f });

		// 🏀 Cuarto rebote mínimo
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 0.5f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete + glm::vec3(0.0f, 0.2f, 0.0f), 1.0f });
		keyframesGabinetes[i].push_back({ baseGabinete, 1.0f });  // Reposo final
	}
}

std::vector<FrameCaida> keyframesGabinetesViejos[12];
bool animarGabinetesViejos[12] = { false };
int frameGabinetesViejos[12] = { 0 };
int delayGabinetesViejos[12] = { 0 };
int desfaseGabinetesViejos[12] = { 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55 };

glm::vec3 posicionesGabinetesViejos[12] = {
};

void prepararKeyframesGabinetesViejos() {
	for (int i = 0; i < 12; ++i) {
		glm::vec3 baseGabinete = posicionesGabinetesViejos[i];
		keyframesGabinetesViejos[i].clear();

		// 🚀 Caída inicial
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 40.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 35.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 30.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 25.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 20.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 15.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 10.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 5.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete, 1.0f });  // Impacto fuerte

		// 🏀 Primer rebote alto
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 4.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 1.5f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete, 1.0f });

		// 🏀 Segundo rebote
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 2.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 0.8f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete, 1.0f });

		// 🏀 Tercer rebote pequeño
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 1.0f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 0.4f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete, 1.0f });

		// 🏀 Cuarto rebote mínimo
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 0.5f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete + glm::vec3(0.0f, 0.2f, 0.0f), 1.0f });
		keyframesGabinetesViejos[i].push_back({ baseGabinete, 1.0f });  // Reposo final
	}
}

//================ ANIMACION ESTUDIANTE ======================
int StuAnim = 0;
float FLegs = 0.0f;
float RLegs = 0.0f;
glm::vec3 StuPos(-4.528f, 4.708f, -35.127f); // X, Z, Y de Blender → X, Y, Z en OpenGL
float StuRot = 0.0f;
bool step = false;

// Variables de animación
bool moveForward = false;   // Tecla 1: Mover hacia adelante
bool turnRight = false;     // Tecla 2: Girar 90 grados a la derecha
bool turnLeft = false;      // Tecla 3: Girar 90 grados a la izquierda
bool turn180 = false;       // Tecla 4: Girar 180 grados
bool moveBackward = false;
// Variables de control para rotaciones
float targetRotation = 0.0f;  // Rotación objetivo

//================ ANIMACION ARAÑA ======================
int AranaAnim = 0;
float cabezaR = 0.0f;
float pata1R = 0.0f;
float pata2R = 0.0f;
float pata3R = 0.0f;
float pata4R = 0.0f;
float pata5R = 0.0f;
float pata6R = 0.0f;
float pata7R = 0.0f;
glm::vec3 AranaPos(-4.89813f, 1.59586f, -13.0314f);
float AranaRot = 0.0f;
bool Aranastep = false;

// Máquina de estados de la araña
bool aranaAdelante = false;
bool aranaGiraDer = false;
bool aranaGiraIzq = false;
bool aranaGira180 = false;

// ========== SALTO ARAÑA ==========
bool aranaSaltando = false;
float aranaVelocidadY = 0.0f;
float aranaAlturaBase = 1.59586f;  // Altura base original de AranaPos.y
float aranaGravedad = -0.01f;
bool aranaFlexionando = true;
float pataSaltoR = 0.0f;

float AranaTargetRot = 0.0f;

// =================== ANIMACIÓN MOSCA ======================

bool MoscaAnim = 0;
float torsoR = 0.0f;
float alaMoscaR = 0.0f;
float pataMosca1R = 0.0f;
float pataMosca2R = 0.0f;
float pataMosca3R = 0.0f;
float pataMosca4R = 0.0f;
float pataMosca5R = 0.0f;
float pataMosca6R = 0.0f;
float alturaMosca = 6.621f; // Altura inicial

enum EstadoMosca {
	EN_REPOSO,
	DESPEGANDO,
	VOLANDO,
	ATERRIZANDO,
	DETENIDO
};

EstadoMosca estadoMosca = DESPEGANDO;  // Cambiar el estado inicial a DESPEGANDO
float tiempoEstado = 0.0f;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


float obtenerDeltaTiempo() {
	float tiempoActual = glfwGetTime();  // obtiene tiempo en segundos desde que GLFW se inició
	deltaTime = tiempoActual - lastFrame;
	lastFrame = tiempoActual;
	return deltaTime;
}



bool modoNoche = false;
GLuint cubemapDia, cubemapNoche;

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



glm::vec3 promedioPosiciones(const glm::vec3* posiciones, int offset) {
	glm::vec3 suma(0.0f);
	for (int i = 0; i < 24; ++i) {
		suma += posiciones[i + offset];
	}
	return suma / 24.0f;
}


int main()
{
	// Inicialización de GLFW
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final | Equipo 8", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();
	// Init GLFW
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

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
	Shader skyboxshader("Shader/Skybox.vs", "Shader/Skybox.frag");
	//=-=-=- LAB ACTUAL =-=-=-
	
	Model lab((char*)"Models/LAB.obj");
	/*	Model lab((char*)"Models/LAB.obj");
	Model medicion((char*)"Models/MEDICION.obj");
	Model medicion2((char*)"Models/MEDICION2.obj");
	Model medicion3((char*)"Models/MEDICION3.obj");
	Model medicion4((char*)"Models/MEDICION4.obj");
	Model mesas((char*)"Models/MESAS.obj");
	Model sillas((char*)"Models/SILLAS.obj");
	Model sillas2((char*)"Models/SILLAS2.obj");
	Model sillas3((char*)"Models/SILLAS3.obj");
	Model monitores((char*)"Models/MONITORES.obj");
	Model cables((char*)"Models/CABLES.obj");
	Model cables2((char*)"Models/CABLES2.obj");
	Model cables3((char*)"Models/CABLES3.obj");
	Model mouse((char*)"Models/MOUSES.obj");
	Model mouse2((char*)"Models/MOUSES2.obj");
	Model mouse3((char*)"Models/MOUSES3.obj");
	Model teclado((char*)"Models/TECLADOS.obj");
	Model teclado2((char*)"Models/TECLADOS2.obj");
	Model teclado3((char*)"Models/TECLADOS3.obj");
	Model gabinete((char*)"Models/GABINETES.obj");
	Model gabinete2((char*)"Models/GABINETES2.obj");
	Model gabinete3((char*)"Models//GABINETES3.obj");
	Model gabinete4((char*)"Models/GABINETES4.obj");
	Model gabinete5((char*)"Models/GABINETES5.obj");
	Model gabinete6((char*)"Models/GABINETES6.obj");
	Model gabinete7((char*)"Models/GABINETES7.obj");
	Model gabinete8((char*)"Models/GABINETES8.obj");
	Model gabinete9((char*)"Models/GABINETES9.obj");
	Model gabinete10((char*)"Models/GABINETES10.obj");
	Model gabinete11((char*)"Models/GABINETES11.obj");
	Model gabinete12((char*)"Models/GABINETES12.obj");
	Model StuBody((char*)"Models/student/StudentBody.obj");
	Model F_RightLeg((char*)"Models/student/RightLeg.obj");
	Model F_LeftLeg((char*)"Models/student/LeftLeg.obj");

	Model* gabinetesViejos[12] = {
	&gabinete, &gabinete2, &gabinete3, &gabinete4,
	&gabinete5, &gabinete6, &gabinete7, &gabinete8,
	&gabinete9, &gabinete10, &gabinete11, &gabinete12
	};

	Model cuerpo((char*)"Models/cuerpo.obj");
	Model cuerpo2((char*)"Models/cuerpo2.obj");
	Model cabeza((char*)"Models/cabeza.obj");
	Model pata1((char*)"Models/pata1.obj");
	Model pata2((char*)"Models/pata2.obj");
	Model pata3((char*)"Models/pata3.obj");
	Model pata4((char*)"Models/pata4.obj");
	Model pata5((char*)"Models/pata5.obj");
	Model pata6((char*)"Models/pata6.obj");
	Model pata7((char*)"Models/pata7.obj");*/

	Model torso((char*)"Models/torso.obj");
	Model ala1((char*)"Models/ala1.obj");
	Model ala2((char*)"Models/ala2.obj");
	Model pataM((char*)"Models/pataMosca1.obj");
	Model pataM2((char*)"Models/pataMosca2.obj");
	Model pataM3((char*)"Models/pataMosca3.obj");
	Model pataM4((char*)"Models/pataMosca4.obj");
	Model pataM5((char*)"Models/pataMosca5.obj");
	Model pataM6((char*)"Models/pataMosca6.obj");
	// =-=-=- LAB NUEVO =-=-=-+

	/*Model labN((char*)"Models/LABN.obj");
	Model medicionLN((char*)"Models/MEDICION_LN.obj");
	Model medicionLN2((char*)"Models/MEDICION_LN2.obj");
	Model medicionLN3((char*)"Models/MEDICION_LN3.obj");
	Model medicionLN4((char*)"Models/MEDICION_LN4.obj");
	Model mesasLN((char*)"Models/MESAS_LN.obj");
	Model monitorLN((char*)"Models/MONITORES_LN.obj");
	Model monitorLN2((char*)"Models/MONITORES_LN2.obj");
	Model monitorLN3((char*)"Models/MONITORES_LN3.obj");
	Model monitorLN4((char*)"Models/MONITORES_LN4.obj");
	Model tecladoLN((char*)"Models/TECLADOS_LN.obj");
	Model tecladoLN2((char*)"Models/TECLADOS_LN2.obj");
	Model tecladoLN3((char*)"Models/TECLADOS_LN3.obj");
	Model mouseLN((char*)"Models/MOUSES_LN.obj");
	Model mouseLN2((char*)"Models/MOUSES_LN2.obj");
	Model mouseLN3((char*)"Models/MOUSES_LN3.obj");
	Model cablesLN((char*)"Models/CABLES_LN.obj");
	Model cablesLN2((char*)"Models/CABLES_LN2.obj");
	Model cablesLN3((char*)"Models/CABLES_LN3.obj");
	Model sillasLN((char*)"Models/SILLAS_LN.obj");
	Model sillasLN2((char*)"Models/SILLAS_LN2.obj");
	Model sillasLN3((char*)"Models/SILLAS_LN3.obj");
	Model gabinetesLN1((char*)"Models/GABINETES_LN.obj");
	Model gabinetesLN2((char*)"Models/GABINETES_LN2.obj");
	Model gabinetesLN3((char*)"Models/GABINETES_LN3.obj");
	Model gabinetesLN4((char*)"Models/GABINETES_LN4.obj");
	Model gabinetesLN5((char*)"Models/GABINETES_LN5.obj");
	Model gabinetesLN6((char*)"Models/GABINETES_LN6.obj");
	Model gabinetesLN7((char*)"Models/GABINETES_LN7.obj");
	Model gabinetesLN8((char*)"Models/GABINETES_LN8.obj");
	Model gabinetesLN9((char*)"Models/GABINETES_LN9.obj");
	Model gabinetesLN10((char*)"Models/GABINETES_LN10.obj");
	Model gabinetesLN11((char*)"Models/GABINETES_LN11.obj");
	Model gabinetesLN12((char*)"Models/GABINETES_LN12.obj");

	Model* gabinetesLN[12] = {
		&gabinetesLN1, &gabinetesLN2, &gabinetesLN3, &gabinetesLN4,
		&gabinetesLN5, &gabinetesLN6, &gabinetesLN7, &gabinetesLN8,
		&gabinetesLN9, &gabinetesLN10, &gabinetesLN11, &gabinetesLN12
	};*/

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

	prepararKeyframesRemolino();
	prepararKeyframesGabinetes();


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
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	//Skybox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	//Load textures
	// Skybox de día
	std::vector<const GLchar*> carasDia = {
		"SkyBoxDia/right.jpg",
		"SkyBoxDia/left.jpg",
		"SkyBoxDia/top.jpg",
		"SkyBoxDia/bottom.jpg",
		"SkyBoxDia/back.jpg",
		"SkyBoxDia/front.jpg"
	};

	cubemapDia = TextureLoading::LoadCubemap(carasDia);

	// Skybox de noche
	std::vector<const GLchar*> carasNoche = {
		"SkyBoxNoche/right.png",
		"SkyBoxNoche/left.png",
		"SkyBoxNoche/top.png",
		"SkyBoxNoche/bottom.png",
		"SkyBoxNoche/back.png",
		"SkyBoxNoche/front.png"
	};


	cubemapNoche = TextureLoading::LoadCubemap(carasNoche);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;

		glm::mat4 modelTemp = glm::mat4(1.0f); //Temp

		float near_plane = 1.0f, far_plane = 50.0f;
		lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
		lightView = glm::lookAt(glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		Animation();


		if (animarSillasViejas1) {
			delayRemolino1++;
			if (delayRemolino1 >= velocidadRemolino) {
				frameSillasViejas1++;
				delayRemolino1 = 0;

				// Activa la segunda cuando se cumpla el desfase
				if (!animarSillasViejas2 && frameSillasViejas1 >= desfaseRemolino2) {
					animarSillasViejas2 = true;
				}
			}
		}

		if (animarSillasViejas2) {
			delayRemolino2++;
			if (delayRemolino2 >= velocidadRemolino) {  // Usa la misma velocidad
				frameSillasViejas2++;
				delayRemolino2 = 0;

				if (!animarSillasViejas3 && frameSillasViejas2 >= desfaseRemolino3) {
					animarSillasViejas3 = true;
				}
			}
		}

		if (animarSillasViejas3) {
			delayRemolino3++;
			if (delayRemolino3 >= velocidadRemolino) {  // También la misma velocidad
				frameSillasViejas3++;
				delayRemolino3 = 0;
			}
		}


		if (animarSillasNuevas1) {
			delayCaida1++;
			if (delayCaida1 >= velocidadCaida) {
				frameSillasNuevas1++;
				delayCaida1 = 0;
				if (frameSillasNuevas1 == desfaseCaida2) animarSillasNuevas2 = true;
			}
		}
		if (animarSillasNuevas2) {
			delayCaida2++;
			if (delayCaida2 >= velocidadCaida) {
				frameSillasNuevas2++;
				delayCaida2 = 0;
				if (frameSillasNuevas2 == desfaseCaida3) animarSillasNuevas3 = true;
			}
		}
		if (animarSillasNuevas3) {
			delayCaida3++;
			if (delayCaida3 >= velocidadCaida) {
				frameSillasNuevas3++;
				delayCaida3 = 0;
			}
		}

		// Clear the colorbuffer
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

		std::vector<glm::vec3> lucesFinalesLabNuevo;
		for (const auto& l : lucesRelativasLabNuevo) {
			lucesFinalesLabNuevo.push_back(l + posLabNuevo); 
		}

		std::vector<glm::vec3> todasLasLuces;
		// Agrega primero las del lab viejo
		for (int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3); ++i) {
			todasLasLuces.push_back(pointLightPositions[i]);
		}
		// Luego las del lab nuevo (con desplazamiento aplicado)
		for (const auto& l : lucesFinalesLabNuevo) {
			todasLasLuces.push_back(l);
		}

		// Directional light
		int idx = todasLasLuces.size() - 1;
		std::string base = "pointLights[" + std::to_string(idx) + "].";

		if (modoNoche) {
			// Luz nocturna más clara
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.18f, 0.18f, 0.25f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.2f, 0.2f, 0.35f);     // más luz difusa
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.4f);     // brillos más claros
		}
		else {
			// Luz brillante para el día
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.4f, 0.4f, 0.4f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
			glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.8f, 0.8f, 0.8f);
		}



		// Point light 1
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);

			// ===---===---=== LUCES DE TECHO Y MONITORES CON ANIMACIÓN EN MONITORES ---===---===---===

			bool noche = modoNoche;  // Usa tu variable real

			for (int i = 0; i < todasLasLuces.size(); i++) {
				std::string idx = std::to_string(i);
				std::string base = "pointLights[" + idx + "].";

				glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "position").c_str()),
					todasLasLuces[i].x, todasLasLuces[i].y, todasLasLuces[i].z);

				if (i <= 10) {
					// 🔆 Luces cálidas
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "ambient").c_str()),
						noche ? 0.07f : 0.04f, noche ? 0.06f : 0.03f, noche ? 0.05f : 0.02f);
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "diffuse").c_str()),
						noche ? 0.5f : 0.2f, noche ? 0.4f : 0.15f, noche ? 0.3f : 0.1f);
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "specular").c_str()),
						noche ? 0.7f : 0.3f, noche ? 0.5f : 0.2f, noche ? 0.4f : 0.15f);
				}
				else if (i >= 11 && i <= 32) {
					// 💡 Luces blancas
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "ambient").c_str()),
						noche ? 0.03f : 0.01f, noche ? 0.03f : 0.007f, noche ? 0.025f : 0.005f);
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "diffuse").c_str()),
						noche ? 0.6f : 0.05f, noche ? 0.5f : 0.04f, noche ? 0.4f : 0.025f);
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "specular").c_str()),
						noche ? 1.0f : 0.1f, noche ? 0.9f : 0.07f, noche ? 0.8f : 0.05f);
				}
				else if (i >= 33 && i <= 40) {
					// 💡 Parpadeantes
					float t = glfwGetTime();
					float flicker = abs(sin(t * 2.0f + i));
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "ambient").c_str()),
						noche ? 0.02f : 0.01f, noche ? 0.015f : 0.007f, noche ? 0.012f : 0.005f);
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "diffuse").c_str()),
						noche ? 0.6f * flicker : 0.05f, noche ? 0.5f * flicker : 0.04f, noche ? 0.9f * flicker : 0.025f);
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "specular").c_str()),
						noche ? 0.8f * flicker : 0.1f, noche ? 0.7f * flicker : 0.07f, noche ? 1.0f * flicker : 0.05f);
				}
				else {
					// 🔶 Lab nuevo
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "ambient").c_str()),
						noche ? 0.03f : 0.02f, noche ? 0.025f : 0.015f, noche ? 0.02f : 0.01f);
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "diffuse").c_str()),
						noche ? 0.2f : 0.1f, noche ? 0.15f : 0.08f, noche ? 0.1f : 0.05f);
					glUniform3f(glGetUniformLocation(lightingShader.Program, (base + "specular").c_str()),
						noche ? 0.3f : 0.2f, noche ? 0.25f : 0.15f, noche ? 0.2f : 0.1f);
				}

				// 📏 Atenuación (más fuerte de día)
				glUniform1f(glGetUniformLocation(lightingShader.Program, (base + "constant").c_str()), 1.0f);
				glUniform1f(glGetUniformLocation(lightingShader.Program, (base + "linear").c_str()),
					noche ? 0.09f : 0.14f);
				glUniform1f(glGetUniformLocation(lightingShader.Program, (base + "quadratic").c_str()),
					noche ? 0.032f : 0.07f);
			}



		// Spotlight - Luz azul del proyector al pizarrón
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), 15.176f, 4.2527f, 12.95f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), 0.0f, -0.391f, -0.9204f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(9.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(13.0f)));
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.01f, 0.01f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.3f, 0.3f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);


		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

		// ---===---===--- RECORRIDO LAB VIEJO ===---===---===

		if (recorridoActivo) {
			float t = glfwGetTime();
			float radius = 18.0f;
			float centerX = 20.0f;
			float centerZ = 6.35f;

			float camX = sin(t * 0.3f) * radius + centerX;
			float camZ = cos(t * 0.3f) * radius + centerZ;

			camera.SetPosition(glm::vec3(camX, 12.0f, camZ));
			camera.SetFront(glm::normalize(glm::vec3(centerX - camX, -0.5f, centerZ - camZ)));
		}

		if (recorridoActivo2) {
			float t = glfwGetTime();

			float centerX = posLabNuevo.x + 16.0f;
			float centerZ = posLabNuevo.z + 3.0f;

			float radius = 24.5f;
			float camX = sin(t * 0.3f) * radius + centerX;
			float camZ = cos(t * 0.3f) * radius + centerZ;
			float camY = posLabNuevo.y + 70.0f;  // Altura de cámara

			// Dirección: mirar hacia el centro pero con inclinación hacia abajo en Y
			glm::vec3 direccion = glm::normalize(glm::vec3(centerX - camX, -0.5f, centerZ - camZ));

			camera.SetPosition(glm::vec3(camX, camY, camZ));
			camera.SetFront(direccion);
		}

		// ================= LÓGICA DE ANIMACIÓN =================

		if (animacionActivaLabNuevo && !animacionTerminada) {
			if (enRebote) {
				velocidadY += gravedad;
				posLabNuevo.y += velocidadY;

				if (posLabNuevo.y <= -40.0f) { // baja más antes de rebotar
					posLabNuevo.y = -40.0f;
					velocidadY = -velocidadY * reboteFactor;
					numRebotes++;

					if (numRebotes >= rebotesMaximos || fabs(velocidadY) < 0.2f) {
						enRebote = false;
					}
				}
			}
			else {
				if (!impactoFinal) {
					impactoFinal = true;
					tiempoReaccionImpacto = glfwGetTime();
				}

				float tiempoDesdeImpacto = glfwGetTime() - tiempoReaccionImpacto;

				if (tiempoDesdeImpacto >= -0.5f) {
					faseAplastamientoActiva = true;
				}

				if (faseAplastamientoActiva) {
					if (escalaY_LabViejo > 0.0f) {
						escalaY_LabViejo -= 0.05f; // aplastamiento más rápido
						posLabNuevo.y -= 0.8f;      // lab nuevo sigue bajando lentamente
						alphaLabViejo -= 1.5f;     // se desvanece visualmente
						if (alphaLabViejo < 0.0f) alphaLabViejo = 0.0f;
					}
					else {
						escalaY_LabViejo = 0.0f;
						alphaLabViejo = 0.0f;
						animacionTerminada = true;
					}
				}
			}
		}

		for (int i = 0; i < 12; ++i) {
			if (!animarGabinetes[i]) {
				if (frameGabinetes[0] >= desfaseGabinetes[i]) {
					animarGabinetes[i] = true;
				}
			}
			else {
				delayGabinetes[i]++;
				if (delayGabinetes[i] >= velocidadGabinete) {
					frameGabinetes[i]++;
					delayGabinetes[i] = 0;
				}
			}
		}

		for (int i = 0; i < 12; ++i) {
			if (!animarGabinetesViejos[i]) {
				if (frameGabinetesViejos[0] >= desfaseGabinetesViejos[i]) {
					animarGabinetesViejos[i] = true;
				}
			}
			else {
				delayGabinetesViejos[i]++;
				if (delayGabinetesViejos[i] >= velocidadGabinete) {
					frameGabinetesViejos[i]++;
					delayGabinetesViejos[i] = 0;
				}
			}
		}

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
		// =-=-=- LAB ACTUAL =-=-=-

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		lab.Draw(lightingShader);
		
		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//mesas.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//cables.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//cables2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//cables3.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//monitores.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//mouse.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//mouse2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//mouse3.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//teclado.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//teclado2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//teclado3.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//gabinete.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//gabinete2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//gabinete3.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//medicion.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//medicion2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//medicion3.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::scale(model, glm::vec3(1.0f, escalaY_LabViejo, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//medicion4.Draw(lightingShader);

		//// === SILLAS VIEJAS 1 ===
		//if (animarSillasViejas1 && frameSillasViejas1 < keyframesRemolino.size()) {
		//	FrameRemolino kf = keyframesRemolino[frameSillasViejas1];
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, kf.posicion);
		//	model = glm::rotate(model, kf.rotacionY, glm::vec3(0, 1, 0));
		//	model = glm::scale(model, glm::vec3(kf.escala));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillas.Draw(lightingShader);
		//}
		//else if (!animarSillasViejas1) {
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición base
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillas.Draw(lightingShader);
		//}

		//// === SILLAS VIEJAS 2 ===
		//if (animarSillasViejas2 && frameSillasViejas2 < keyframesRemolino.size()) {
		//	FrameRemolino kf = keyframesRemolino[frameSillasViejas2];
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, kf.posicion);
		//	model = glm::rotate(model, kf.rotacionY, glm::vec3(0, 1, 0));
		//	model = glm::scale(model, glm::vec3(kf.escala));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillas2.Draw(lightingShader);
		//}
		//else if (!animarSillasViejas2) {
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición base
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillas2.Draw(lightingShader);
		//}

		//// === SILLAS VIEJAS 3 ===
		//if (animarSillasViejas3 && frameSillasViejas3 < keyframesRemolino.size()) {
		//	FrameRemolino kf = keyframesRemolino[frameSillasViejas3];
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, kf.posicion);
		//	model = glm::rotate(model, kf.rotacionY, glm::vec3(0, 1, 0));
		//	model = glm::scale(model, glm::vec3(kf.escala));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillas3.Draw(lightingShader);
		//}
		//else if (!animarSillasViejas3) {
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Posición base
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillas3.Draw(lightingShader);
		//}

		//for (int i = 0; i < 12; ++i) {
		//	if (animarGabinetesViejos[i] && frameGabinetesViejos[i] < keyframesGabinetesViejos[i].size()) {
		//		FrameCaida kf = keyframesGabinetesViejos[i][frameGabinetesViejos[i]];
		//		model = glm::mat4(1.0f);
		//		model = glm::translate(model, kf.posicion);
		//		model = glm::scale(model, glm::vec3(kf.escala));
		//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//		gabinetesViejos[i]->Draw(lightingShader);
		//	}
		//	else if ((animarGabinetesViejos[i] || frameGabinetesViejos[0] >= desfaseGabinetesViejos[i]) && !keyframesGabinetesViejos[i].empty()) {
		//		FrameCaida kfFinal = keyframesGabinetesViejos[i].back();
		//		model = glm::mat4(1.0f);
		//		model = glm::translate(model, kfFinal.posicion);
		//		model = glm::scale(model, glm::vec3(kfFinal.escala));
		//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//		gabinetesViejos[i]->Draw(lightingShader);
		//	}
		//}

		 //=-=-=- LAB NUEVO =-=-=-

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//labN.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//medicionLN.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//medicionLN2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//medicionLN3.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//medicionLN4.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//mesasLN.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//monitorLN.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//monitorLN2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//monitorLN3.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//monitorLN4.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//tecladoLN.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//tecladoLN2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//tecladoLN3.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//mouseLN.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//mouseLN2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//mouseLN3.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//cablesLN.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//cablesLN2.Draw(lightingShader);

		//model = glm::mat4(1);
		//model = glm::translate(model, posLabNuevo);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		//cablesLN3.Draw(lightingShader);

		////=== SILLAS NUEVAS 1 ===
		//if (animarSillasNuevas1 && frameSillasNuevas1 < keyframesCaidaSillasNuevas.size()) {
		//	FrameCaida kf = keyframesCaidaSillasNuevas[frameSillasNuevas1];
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, kf.posicion);
		//	model = glm::scale(model, glm::vec3(kf.escala));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillasLN.Draw(lightingShader);
		//}
		//else if (!keyframesCaidaSillasNuevas.empty()) {
		//	FrameCaida kfFinal = keyframesCaidaSillasNuevas.back();
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, kfFinal.posicion);
		//	model = glm::scale(model, glm::vec3(kfFinal.escala));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillasLN.Draw(lightingShader);
		//}

		//// === SILLAS NUEVAS 2 ===
		//if (animarSillasNuevas2 && frameSillasNuevas2 < keyframesCaidaSillasNuevas.size()) {
		//	FrameCaida kf = keyframesCaidaSillasNuevas[frameSillasNuevas2];
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, kf.posicion);
		//	model = glm::scale(model, glm::vec3(kf.escala));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillasLN2.Draw(lightingShader);
		//}
		//else if (!keyframesCaidaSillasNuevas.empty()) {
		//	FrameCaida kfFinal = keyframesCaidaSillasNuevas.back();
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, kfFinal.posicion);
		//	model = glm::scale(model, glm::vec3(kfFinal.escala));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillasLN2.Draw(lightingShader);
		//}

		//// === SILLAS NUEVAS 3 ===
		//if (animarSillasNuevas3 && frameSillasNuevas3 < keyframesCaidaSillasNuevas.size()) {
		//	FrameCaida kf = keyframesCaidaSillasNuevas[frameSillasNuevas3];
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, kf.posicion);
		//	model = glm::scale(model, glm::vec3(kf.escala));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillasLN3.Draw(lightingShader);
		//}
		//else if (!keyframesCaidaSillasNuevas.empty()) {
		//	FrameCaida kfFinal = keyframesCaidaSillasNuevas.back();
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, kfFinal.posicion);
		//	model = glm::scale(model, glm::vec3(kfFinal.escala));
		//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//	sillasLN3.Draw(lightingShader);
		//}

		//// === GABINETES ===

		//for (int i = 0; i < 12; ++i) {
		//	if (animarGabinetes[i] && frameGabinetes[i] < keyframesGabinetes[i].size()) {
		//		FrameCaida kf = keyframesGabinetes[i][frameGabinetes[i]];
		//		model = glm::mat4(1.0f);
		//		model = glm::translate(model, kf.posicion);
		//		model = glm::scale(model, glm::vec3(kf.escala));
		//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//		gabinetesLN[i]->Draw(lightingShader);
		//	}
		//	else if ((animarGabinetes[i] || frameGabinetes[0] >= desfaseGabinetes[i]) && !keyframesGabinetes[i].empty()) {
		//		FrameCaida kfFinal = keyframesGabinetes[i].back();
		//		model = glm::mat4(1.0f);
		//		model = glm::translate(model, kfFinal.posicion);
		//		model = glm::scale(model, glm::vec3(kfFinal.escala));
		//		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//		gabinetesLN[i]->Draw(lightingShader);
		//	}
		//}

		////Body
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, StuPos);
		//model = glm::scale(model, glm::vec3(8.0f));  
		//model = glm::rotate(model, glm::radians(StuRot), glm::vec3(0.0f, 1.0f, 0.0f));
		//modelTemp = model;
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//StuBody.Draw(lightingShader);

		//// Front Left Leg
		//model = modelTemp;
		//model = glm::rotate(model, glm::radians(FLegs), glm::vec3(0.0f, 0.0f, -1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//F_LeftLeg.Draw(lightingShader);

		//// Front Right Leg
		//model = modelTemp;
		//model = glm::rotate(model, glm::radians(FLegs), glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//F_RightLeg.Draw(lightingShader);


		//// Cuerpo
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, AranaPos);
		//model = glm::scale(model, glm::vec3(1.0f));  // Más pequeña
		//model = glm::rotate(model, glm::radians(AranaRot), glm::vec3(0.0f, 1.0f, 0.0f));
		//modelTemp = model;
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//cuerpo.Draw(lightingShader);

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, AranaPos);
		//model = glm::scale(model, glm::vec3(1.0f));  // Más pequeña
		//model = glm::rotate(model, glm::radians(AranaRot), glm::vec3(0.0f, 1.0f, 0.0f));
		//modelTemp = model;
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//cuerpo2.Draw(lightingShader);

		//// Cabeza
		//model = modelTemp;
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(cabezaR), glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//cabeza.Draw(lightingShader);

		//// Pata 1
		//model = modelTemp;
		//model = glm::translate(model, glm::vec3(-0.317343f, -0.019981f, -0.334217f));
		//model = glm::rotate(model, glm::radians(pata1R), glm::vec3(0.0f, 0.0f, -1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//pata1.Draw(lightingShader);

		//// Pata 2
		//model = modelTemp;
		//model = glm::translate(model, glm::vec3(-0.354153f, -0.029496f, -0.506979f));
		//model = glm::rotate(model, glm::radians(pata2R), glm::vec3(0.0f, 0.0f, -1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//pata2.Draw(lightingShader);

		//// Pata 3
		//model = modelTemp;
		//model = glm::translate(model, glm::vec3(-0.267455f, 0.026129f, -0.645586f));
		//model = glm::rotate(model, glm::radians(pata3R), glm::vec3(0.0f, 0.0f, -1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//pata3.Draw(lightingShader);

		//// Pata 4
		//model = modelTemp;
		//model = glm::translate(model, glm::vec3(0.32149f, -0.029496f, -0.320834f));
		//model = glm::rotate(model, glm::radians(pata4R), glm::vec3(0.0f, 0.0f, -1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//pata4.Draw(lightingShader);

		//// Pata 5
		/*model = modelTemp;
		model = glm::translate(model, glm::vec3(0.27101f, -0.029496f, -0.506979f));
		model = glm::rotate(model, glm::radians(pata5R), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pata5.Draw(lightingShader);*/

		//// Pata 6
		/*model = modelTemp;
		model = glm::translate(model, glm::vec3(0.245932f, 0.017995f, -0.622514f));
		model = glm::rotate(model, glm::radians(pata6R), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pata6.Draw(lightingShader);*/

		// Establecer altura mínima para evitar que la mosca atraviese la mesa
		if (alturaMosca < 6.621f) {
			alturaMosca = 6.621f; // Evitar que la mosca baje por debajo de la mesa
		}

		// Establecer altura máxima si quieres limitar cuánto puede subir
		if (alturaMosca > 14.8594f) {
			alturaMosca = 14.8594f; // Altura máxima que puede alcanzar la mosca
		}

		// Ajustar las patas para que sigan el movimiento de la mosca
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.023786f, alturaMosca - 0.03f, 18.1816f)); // Ajuste de altura
		model = glm::rotate(model, glm::radians(pataMosca1R), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación de la pata 1
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pataM.Draw(lightingShader);

		// Torso de la mosca
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.05608f, alturaMosca, 18.2091f));
		model = glm::rotate(model, glm::radians(torsoR), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		torso.Draw(lightingShader);

		// Ala 1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.02417f, alturaMosca, 18.2145f)); // posición global
		model = glm::rotate(model, glm::radians(alaMoscaR), glm::vec3(1.0f, 0.0f, 0.0f)); // rotación alrededor del eje X (ajustar si es Z u otro)
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ala1.Draw(lightingShader);

		// Ala derecha (ala2)
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.059036f, alturaMosca, 18.2458f)); // posición global de la segunda ala
		model = glm::rotate(model, glm::radians(alaMoscaR), glm::vec3(1.0f, 0.0f, 0.0f)); // rota un poco diferente si lo deseas
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ala2.Draw(lightingShader);

		// Pata 1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.023786f, alturaMosca - 0.03f, 18.1816f)); // Ajuste de altura
		model = glm::rotate(model, glm::radians(pataMosca1R), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación alrededor del eje X
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pataM.Draw(lightingShader);

		// Pata 2
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.002416f, alturaMosca - 0.03f, 18.198f)); // Ajuste de altura de la pata 2
		model = glm::rotate(model, glm::radians(pataMosca2R), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación alrededor del eje X
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pataM2.Draw(lightingShader);

		// Pata 3
		model = modelTemp;
		model = glm::translate(model, glm::vec3(-0.010993f, alturaMosca - 0.03f, 18.2282f)); // Ajuste de altura de la pata 3
		model = glm::rotate(model, glm::radians(pataMosca3R), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación alrededor del eje Y
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pataM3.Draw(lightingShader);

		// Pata 4
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.08501f, alturaMosca - 0.03f, 18.2328f)); // Ajuste de altura de la pata 4
		model = glm::rotate(model, glm::radians(pataMosca4R), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación alrededor del eje Y
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pataM4.Draw(lightingShader);

		// Pata 5
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.069206f, alturaMosca - 0.03f, 18.2684f)); // Ajuste de altura de la pata 5
		model = glm::rotate(model, glm::radians(pataMosca5R), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación alrededor del eje Y
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pataM5.Draw(lightingShader);

		// Pata 6
		model = modelTemp;
		model = glm::translate(model, glm::vec3(0.049586f, alturaMosca - 0.03f, 18.2773f)); // Ajuste de altura de la pata 6
		model = glm::rotate(model, glm::radians(pataMosca6R), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación alrededor del eje Y
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		pataM6.Draw(lightingShader);


		glBindVertexArray(0);

		//Draw skybox
		glDepthFunc(GL_LEQUAL);
		skyboxshader.Use();

		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Asegúrate que este uniforme coincida con la unidad que activas
		glUniform1i(glGetUniformLocation(skyboxshader.Program, "skybox"), 0);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);  // Asegúrate de que es 0, no 1
		glBindTexture(GL_TEXTURE_CUBE_MAP, modoNoche ? cubemapNoche : cubemapDia);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);


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
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)

		model = glm::mat4(1);
		model = glm::translate(model, pointLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO);

		glBindVertexArray(0);



		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();



	return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{

	if (recorridoActivo) return;

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
		pointLightPositions[0].x += 0.01f;
	}
	if (keys[GLFW_KEY_G])
	{
		pointLightPositions[0].x -= 0.01f;
	}

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
			Light1 = glm::vec3(1.0f, 1.0f, 0.0f);

		}
		else
		{
			Light1 = glm::vec3(0);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
		}
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		recorridoActivo = !recorridoActivo;
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		recorridoActivo2 = !recorridoActivo2;
		recorridoActivo = false;  // Desactiva el otro recorrido si está activo
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		if (!animacionActivaLabNuevo) {
			// Iniciar animación
			animacionActivaLabNuevo = true;
			animacionTerminada = false;
			enRebote = true;
			impactoFinal = false;
			faseAplastamientoActiva = false;
			numRebotes = 0;
			escalaY_LabViejo = 1.0f;
			alphaLabViejo = 1.0f;
			posLabNuevo = glm::vec3(0.0f, 100.0f, 0.0f);
			velocidadY = 0.0f;
		}
		else {
			// Cancelar animación
			animacionActivaLabNuevo = false;
			animacionTerminada = true;
		}
	}


	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		// Activa la primera animación
		animarSillasViejas1 = true;
		frameSillasViejas1 = 0;
		delayRemolino1 = 0;

		// Prepara el desfase de las otras dos
		animarSillasViejas2 = false;
		frameSillasViejas2 = 0;
		delayRemolino2 = 0;

		animarSillasViejas3 = false;
		frameSillasViejas3 = 0;
		delayRemolino3 = 0;
	}

	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		labYaAterrizo = false;
		prepararKeyframesCaidaSillasNuevas();

		// Activa la primera animación de sillas nuevas
		animarSillasNuevas1 = true;
		frameSillasNuevas1 = 0;
		delayCaida1 = 0;

		// Prepara el desfase de las otras dos
		animarSillasNuevas2 = false;
		frameSillasNuevas2 = 0;
		delayCaida2 = 0;

		animarSillasNuevas3 = false;
		frameSillasNuevas3 = 0;
		delayCaida3 = 0;
	}

	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		prepararKeyframesGabinetes();
		for (int i = 0; i < 12; ++i) {
			animarGabinetes[i] = false;  // Empiezan desactivados
			frameGabinetes[i] = 0;
			delayGabinetes[i] = 0;
		}
		// Activa el primero de inmediato
		animarGabinetes[0] = true;
	}
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_1:
			moveForward = !moveForward;  // Alternar avance
			turnRight = false;
			turnLeft = false;
			turn180 = false;
			moveBackward = false;
			break;

		case GLFW_KEY_2:
			if (turnRight) {
				turnRight = false;  // Detener si ya estaba girando
			}
			else {
				moveForward = false;
				moveBackward = false;
				turnRight = true;
				turnLeft = false;
				turn180 = false;
				targetRotation = fmod(StuRot + 90.0f, 360.0f);
				if (targetRotation < 0.0f) targetRotation += 360.0f;
			}
			break;

		case GLFW_KEY_3:
			if (turnLeft) {
				turnLeft = false;
			}
			else {
				moveForward = false;
				moveBackward = false;
				turnRight = false;
				turnLeft = true;
				turn180 = false;
				targetRotation = fmod(StuRot - 90.0f, 360.0f);
				if (targetRotation < 0.0f) targetRotation += 360.0f;
			}
			break;

		case GLFW_KEY_4:
			if (turn180) {
				turn180 = false;
			}
			else {
				moveForward = false;
				moveBackward = false;
				turnRight = false;
				turnLeft = false;
				turn180 = true;
				targetRotation = fmod(StuRot + 180.0f, 360.0f);
				if (targetRotation < 0.0f) targetRotation += 360.0f;
			}
			break;

		case GLFW_KEY_5:
			moveBackward = !moveBackward;  // Alternar retroceso
			moveForward = false;
			turnRight = false;
			turnLeft = false;
			turn180 = false;
			break;
		}
	}

	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		prepararKeyframesGabinetesViejos();
		for (int i = 0; i < 12; ++i) {
			animarGabinetesViejos[i] = false;     // Inicia todos en falso
			frameGabinetesViejos[i] = 0;
			delayGabinetesViejos[i] = 0;
		}
		animarGabinetesViejos[0] = true;          // Activa el primero inmediatamente
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		modoNoche = !modoNoche;
	}

	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_Z:  // Avanza
			aranaAdelante = !aranaAdelante;
			aranaGiraDer = aranaGiraIzq = aranaGira180 = false;
			break;

		case GLFW_KEY_X:  // Gira derecha
			aranaGiraDer = true;
			aranaAdelante = aranaGiraIzq = aranaGira180 = false;
			AranaTargetRot = fmod(AranaRot + 90.0f, 360.0f);
			break;

		case GLFW_KEY_C:  // Gira izquierda
			aranaGiraIzq = true;
			aranaAdelante = aranaGiraDer = aranaGira180 = false;
			AranaTargetRot = fmod(AranaRot - 90.0f + 360.0f, 360.0f);
			break;

		case GLFW_KEY_V:  // Gira 180
			aranaGira180 = true;
			aranaAdelante = aranaGiraDer = aranaGiraIzq = false;
			AranaTargetRot = fmod(AranaRot + 180.0f, 360.0f);
			break;

		case GLFW_KEY_H:
			if (!aranaSaltando) {
				aranaSaltando = true;
				aranaVelocidadY = 0.15f;  // Velocidad inicial del salto
			}
			break;

		}
	}


}


void Animation() {

	float speed = 0.08f;
	float rotSpeed = 2.0f;

	// Función lambda para diferencia angular (-180 a 180)
	auto angleDiff = [](float a, float b) {
		float diff = fmod((a - b + 540.0f), 360.0f) - 180.0f;
		return diff;
		};

	// Animación de las piernas al caminar
	if (moveForward) {
		if (!step) {
			RLegs += 0.3f;
			FLegs += 0.3f;
			if (RLegs > 13.0f)
				step = true;
		}
		else {
			RLegs -= 0.3f;
			FLegs -= 0.3f;
			if (RLegs < -13.0f)
				step = false;
		}

		// Dirección según rotación
		float radians = glm::radians(StuRot);
		float dirX = cos(radians);   // antes era sin
		float dirZ = -sin(radians);  // antes era cos

		StuPos.x += dirX * speed;
		StuPos.z += dirZ * speed;

	}

	// Girar 90° derecha
	if (turnRight) {
		float diff = angleDiff(targetRotation, StuRot);
		if (abs(diff) > 1.0f) {
			StuRot += (diff > 0 ? rotSpeed : -rotSpeed);
			RLegs = sin(glfwGetTime() * 5.0f) * 3.0f;
			FLegs = sin(glfwGetTime() * 5.0f) * 3.0f;
		}
		else {
			turnRight = false;
			StuRot = targetRotation;
			RLegs = 0.0f;
			FLegs = 0.0f;
		}
	}

	// Girar 90° izquierda
	if (turnLeft) {
		float diff = angleDiff(targetRotation, StuRot);
		if (abs(diff) > 1.0f) {
			StuRot += (diff > 0 ? rotSpeed : -rotSpeed);
			RLegs = sin(glfwGetTime() * 5.0f) * 3.0f;
			FLegs = sin(glfwGetTime() * 5.0f) * 3.0f;
		}
		else {
			turnLeft = false;
			StuRot = targetRotation;
			RLegs = 0.0f;
			FLegs = 0.0f;
		}
	}

	// Girar 180°
	if (turn180) {
		float diff = angleDiff(targetRotation, StuRot);
		if (abs(diff) > 1.0f) {
			StuRot += (diff > 0 ? rotSpeed : -rotSpeed);
			RLegs = sin(glfwGetTime() * 5.0f) * 3.0f;
			FLegs = sin(glfwGetTime() * 5.0f) * 3.0f;
		}
		else {
			turn180 = false;
			StuRot = targetRotation;
			RLegs = 0.0f;
			FLegs = 0.0f;
		}
	}

	// Animación al caminar hacia atrás
	if (moveBackward) {
		if (!step) {
			RLegs += 0.3f;
			FLegs += 0.3f;
			if (RLegs > 13.0f)
				step = true;
		}
		else {
			RLegs -= 0.3f;
			FLegs -= 0.3f;
			if (RLegs < -13.0f)
				step = false;
		}

		// Dirección opuesta al frente
		float radians = glm::radians(StuRot);
		float dirX = cos(radians);
		float dirZ = -sin(radians);

		StuPos.x -= dirX * speed;
		StuPos.z -= dirZ * speed;
	}

	// Normalizar StuRot
	if (StuRot >= 360.0f) StuRot -= 360.0f;
	if (StuRot < 0.0f) StuRot += 360.0f;



	// ========== ARAÑA ==========


	// Movimiento de la cabeza
	cabezaR = sin(glfwGetTime() * 2.5f) * 10.0f;

	if (aranaAdelante) {
		// Movimiento alternado: patas 1,3,5 contra 2,4,6
		if (!Aranastep) {
			pata1R += 0.4f;  // grupo A
			pata3R += 0.4f;
			pata5R += 0.4f;

			pata2R -= 0.4f;  // grupo B
			pata4R -= 0.4f;
			pata6R -= 0.4f;

			if (pata1R >= 15.0f)
				Aranastep = true;
		}
		else {
			pata1R -= 0.4f;
			pata3R -= 0.4f;
			pata5R -= 0.4f;

			pata2R += 0.4f;
			pata4R += 0.4f;
			pata6R += 0.4f;

			if (pata1R <= -15.0f)
				Aranastep = false;
		}

		// Movimiento hacia adelante (modelo mirando eje +X)
		float radians = glm::radians(AranaRot);
		AranaPos.x += sin(radians) * 0.05f;
		AranaPos.z += cos(radians) * 0.05f;
	}

	if (aranaGiraDer || aranaGiraIzq || aranaGira180) {
		float diff = fmod((AranaTargetRot - AranaRot + 540.0f), 360.0f) - 180.0f;
		if (abs(diff) > 1.0f) {
			AranaRot += (diff > 0 ? 1.5f : -1.5f);

			// Movimiento continuo durante giro
			pata1R = sin(glfwGetTime() * 5.0f) * 10.0f;
			pata2R = -pata1R;
			pata3R = pata1R * 0.8f;
			pata4R = -pata3R;
			pata5R = pata1R * 0.6f;
			pata6R = -pata5R;
		}
		else {
			AranaRot = AranaTargetRot;
			aranaGiraDer = aranaGiraIzq = aranaGira180 = false;

			// Resetear patas
			pata1R = pata2R = pata3R = pata4R = pata5R = pata6R = 0.0f;
		}
	}

	// ========== SALTO DE LA ARAÑA CON ROTACIÓN REALISTA DE PATAS ==========

	if (aranaSaltando) {
		AranaPos.y += aranaVelocidadY;
		aranaVelocidadY += aranaGravedad;

		// Fase de impulso (subida): las patas se recogen hacia arriba
		if (aranaVelocidadY > 0) {
			float angulo = sin(glfwGetTime() * 5.0f) * 25.0f;
			pata1R = pata3R = pata5R = -angulo;
			pata2R = pata4R = pata6R = -angulo;
		}
		// Fase de caída: las patas se abren un poco hacia afuera como equilibrio
		else {
			float angulo = sin(glfwGetTime() * 4.0f) * 15.0f;
			pata1R = pata3R = pata5R = angulo;
			pata2R = pata4R = pata6R = angulo;
		}

		// Termina el salto
		if (AranaPos.y <= aranaAlturaBase) {
			AranaPos.y = aranaAlturaBase;
			aranaSaltando = false;
			aranaVelocidadY = 0.0f;
			pata1R = pata2R = pata3R = pata4R = pata5R = pata6R = 0.0f;
		}
	}

	// ===================== ANIMACION MOSCA ======================

	float deltaTime = obtenerDeltaTiempo();  // Función para obtener el tiempo entre frames
	tiempoEstado += deltaTime;

	switch (estadoMosca) {
	case DESPEGANDO:
		// Movimiento suave del torso hacia adelante
		torsoR += deltaTime * 15.0f;  // Reduce la velocidad para que no sea tan rápido

		// Batido de las alas, con una frecuencia más baja para que sea menos frenético
		alaMoscaR = sin(glfwGetTime() * 20.0f) * 25.0f;  // Batido más realista

		// Animación de las patas en el despegue, usando desfases para evitar sincronización perfecta
		pataMosca1R = sin(glfwGetTime() * 4.0f + 0.0f) * 15.0f;
		pataMosca2R = sin(glfwGetTime() * 4.5f + 0.5f) * 15.0f;
		pataMosca3R = sin(glfwGetTime() * 5.0f + 1.0f) * 15.0f;
		pataMosca4R = sin(glfwGetTime() * 3.0f + 1.5f) * 15.0f;
		pataMosca5R = sin(glfwGetTime() * 3.5f + 2.0f) * 15.0f;
		pataMosca6R = sin(glfwGetTime() * 2.5f + 2.5f) * 15.0f;

		// Elevar la mosca mientras despegas, con una pequeña oscilación para hacer el movimiento más natural
		alturaMosca += deltaTime * 1.0f + sin(glfwGetTime() * 2.0f) * 0.05f; // Oscilación suave en la altura

		// Asegurarse de que la altura no baje de 6.621f ni suba demasiado rápido
		if (alturaMosca < 6.621f) {
			alturaMosca = 6.621f;
		}
		if (alturaMosca > 14.8594f) {
			alturaMosca = 14.8594f;
		}

		// Cambiar al estado de volando si la mosca ha alcanzado la inclinación deseada
		if (torsoR > 12.0f) {  // Ajustado para ser un poco más suave
			estadoMosca = VOLANDO;
			tiempoEstado = 0.0f;  // Resetear el tiempo cuando cambia el estado
		}
		break;
	
	case VOLANDO:
		// Movimiento del torso (oscilación controlada)
		torsoR = sin(glfwGetTime() * 1.5f) * 5.0f;  // Limitar el rango de movimiento del torso

		// Batido de las alas (frecuencia reducida para un batido más realista)
		alaMoscaR = sin(glfwGetTime() * 20.0f) * 30.0f;  // Batido de alas

		// Animación de las patas al volar (variar ligeramente el desfase entre las patas)
		pataMosca1R = sin(glfwGetTime() * 4.0f + 0.0f) * 15.0f;
		pataMosca2R = sin(glfwGetTime() * 4.5f + 0.5f) * 15.0f;
		pataMosca3R = sin(glfwGetTime() * 5.0f + 1.0f) * 15.0f;
		pataMosca4R = sin(glfwGetTime() * 3.0f + 1.5f) * 15.0f;
		pataMosca5R = sin(glfwGetTime() * 3.5f + 2.0f) * 15.0f;
		pataMosca6R = sin(glfwGetTime() * 2.5f + 2.5f) * 15.0f;

		// Oscilación suave en la altura
		alturaMosca += sin(glfwGetTime() * 0.5f) * 0.05f;  // Movimiento suave de la altura

		// Asegurarse de que la altura no baje de 6.621f ni suba demasiado
		if (alturaMosca < 6.621f) {
			alturaMosca = 6.621f;
		}
		if (alturaMosca > 14.8594f) {
			alturaMosca = 14.8594f;
		}

		// Cambiar al estado de aterrizaje después de cierto tiempo
		if (tiempoEstado > 6.0f) {
			estadoMosca = ATERRIZANDO;
			tiempoEstado = 0.0f;
		}
		break;

	case ATERRIZANDO:
		// Reducir la inclinación del torso de forma más suave
		torsoR -= deltaTime * 10.0f;  // Reducir la velocidad para que el torso no se incline demasiado rápido

		// Batido suave de las alas al aterrizar (frecuencia más baja para un movimiento más suave)
		alaMoscaR = sin(glfwGetTime() * 15.0f) * 10.0f;  // Reducir la frecuencia para batido suave de alas

		// Animación de las patas al aterrizar, con menor amplitud
		pataMosca1R = sin(glfwGetTime() * 4.0f + 0.0f) * 5.0f;  // Menor amplitud
		pataMosca2R = sin(glfwGetTime() * 4.5f + 0.5f) * 5.0f;
		pataMosca3R = sin(glfwGetTime() * 5.0f + 1.0f) * 5.0f;
		pataMosca4R = sin(glfwGetTime() * 3.0f + 1.5f) * 5.0f;
		pataMosca5R = sin(glfwGetTime() * 3.5f + 2.0f) * 5.0f;
		pataMosca6R = sin(glfwGetTime() * 2.5f + 2.5f) * 5.0f;

		// Reducir la altura mientras aterriza (hacerlo de forma más suave)
		alturaMosca -= deltaTime * 0.25f;  // Reducir la velocidad del descenso para suavizar el aterrizaje

		// Asegurarse de que la altura no baje de 6.621f
		if (alturaMosca < 6.621f) {
			alturaMosca = 6.621f;  // Limitar la altura mínima
			estadoMosca = DETENIDO;  // Detener la animación
			// No resetear tiempoEstado aquí, permite que la animación continúe suavemente
		}
		break;

	case DETENIDO:
		// Mantener el batido suave de las alas (con pequeña oscilación)
		alaMoscaR = sin(glfwGetTime() * 5.0f) * 5.0f;  // Batido suave y lento

		// Detener el movimiento del torso
		torsoR = 0.0f;  // Torso completamente detenido

		// Las patas están detenidas, pero podemos agregar un pequeño movimiento residual
		pataMosca1R = sin(glfwGetTime() * 1.0f) * 2.0f;  // Movimiento sutil
		pataMosca2R = sin(glfwGetTime() * 1.5f) * 2.0f;
		pataMosca3R = sin(glfwGetTime() * 2.0f) * 2.0f;
		pataMosca4R = sin(glfwGetTime() * 1.5f) * 2.0f;
		pataMosca5R = sin(glfwGetTime() * 1.0f) * 2.0f;
		pataMosca6R = sin(glfwGetTime() * 1.0f) * 2.0f;

		// Esperar 2 segundos en el estado DETENIDO y luego reiniciar la animación
		if (tiempoEstado > 2.0f) {
			estadoMosca = DESPEGANDO;  // Reiniciar la animación desde DESPEGANDO
			tiempoEstado = 0.0f;  // Resetear el tiempo
		}
		break;

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