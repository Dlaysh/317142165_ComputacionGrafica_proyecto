#include <iostream>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <sstream>
#include <future>
#include <thread>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Clases del proyecto
#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <animatedmodel.h>
#include <material.h>
#include <light.h>
#include <cubemap.h>

// ============================================================================
// CLASES MODULARIZADAS
// ============================================================================
#include "LightManager.h"
#include "PhysicsSystem.h"
#include "RenderableObject.h"
#include "AnimatedRenderableObject.h"
#include "OrbitingMoonObject.h"
#include "AxisGizmo.h"
#include "LightIndicator.h"
#include "LoadingScreen.h"
#include "InputController.h"
#include "SceneManager.h"
#include "HierarchicalObject.h"
#include "HierarchicalOrbitingObject.h"
#include "OrbitVisualizer.h"
#include "ObjectGenerator.h"

// ============================================================================
// CONSTANTES GLOBALES
// ============================================================================
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
float movementSpeedFactor = 12.0f;
float mouseSensitivityFactor = 3.0f;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================
GLFWwindow* window;

// Cámaras
Camera camera(glm::vec3(0.0f, 5.0f, 20.0f));
Camera camera3rd(glm::vec3(0.0f, 0.0f, 0.0f));

// Control de tiempo
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Estado del jugador
glm::vec3 position(5.0f, 7.0f, -70.0f);
glm::vec3 forwardView(0.0f, 0.0f, 1.0f);
float rotateCharacter = 0.0f;
bool isPlayerMoving = false;

// Estado de la cámara
float trdpersonOffset = 5.0f;
bool activeCamera = true;
bool showLightIndicators = true;

// Sistemas principales
PhysicsSystem physicsSystem;
std::unique_ptr<SceneManager> sceneManager;
std::unique_ptr<InputController> inputController;

// ============================================================================
// CALLBACKS DE GLFW
// ============================================================================
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (inputController) {
		inputController->processMouse(window, xpos, ypos);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (inputController) {
		inputController->processScroll(window, xoffset, yoffset);
	}
}

void processInput(GLFWwindow* window) {
	if (inputController) {
		inputController->processKeyboard(window);
	}
}

// ============================================================================
// INICIALIZACIÓN
// ============================================================================

bool initializeWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cargando...", NULL, NULL);
	if (window == NULL) {
		std::cout << "ERROR: Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERROR: Failed to initialize GLAD" << std::endl;
		return false;
	}

	glEnable(GL_DEPTH_TEST);
	return true;
}

void loadShaders(LoadingScreen& loadingScreen,
	Shader*& cubemapShader, Shader*& dynamicShader,
	Shader*& mLightsShader, Shader*& mMoonShader) {

	loadingScreen.updateProgress("Compilando shaders de cubemap...");
	cubemapShader = new Shader("monster_house/shaders/10_vertex_cubemap.vs",
		"monster_house/shaders/10_fragment_cubemap.fs");

	loadingScreen.updateProgress("Compilando shaders de animación...");
	dynamicShader = new Shader("monster_house/shaders/10_vertex_skinning-physics.vs",
		"monster_house/shaders/10_fragment_skinning-physics.fs");
	dynamicShader->setBonesIDs(MAX_RIGGING_BONES);

	loadingScreen.updateProgress("Compilando shaders de iluminación...");
	mLightsShader = new Shader("monster_house/shaders/11_PhongShaderMultLights.vs",
		"monster_house/shaders/11_PhongShaderMultLights.fs");

	loadingScreen.updateProgress("Compilando shaders de órbita lunar...");
	mMoonShader = new Shader("monster_house/shaders/moon_orbit_phong.vs",
		"monster_house/shaders/moon_orbit_phong.fs");
}

void loadModels(LoadingScreen& loadingScreen,
	AnimatedModel*& animatedAstronauta, Model*& house, Model*& sol,
	Model*& piso, Model*& naveEspacial, Model*& satelite,
	Model*& panelSolar, Model*& invernadero, Model*& escalera,
	Model*& puerta, Model*& cama, Model*& satelite2, Model*& comedor,
	Model*& sofa, Model*& leafB, Model*& prime1, Model*& domoInvernadero,
	Model*& domoEstructura, Model*& tunelMetal, Model*& plantas) {

	// ============================================================================
	// CARGA SECUENCIAL OPTIMIZADA CON FEEDBACK DETALLADO
	// SOLO MANTENEMOS LA CASA Y EL PISO LUNAR
	// ============================================================================

	loadingScreen.updateProgress("Iniciando carga...");

	// Entorno básico
	loadingScreen.updateProgress("Cargando piso...");
	piso = new Model("monster_house/models/piso.fbx");

	loadingScreen.updateProgress("Cargando casa principal...");
	house = new Model("monster_house/models/MonsterHouseFinal.fbx");

	// Los modelos restantes se establecen a nullptr ya que fueron eliminados de la carga
	animatedAstronauta = nullptr;
	sol = nullptr;
	naveEspacial = nullptr;
	satelite = nullptr;
	panelSolar = nullptr;
	invernadero = nullptr;
	escalera = nullptr;
	puerta = nullptr;
	cama = nullptr;
	satelite2 = nullptr;
	comedor = nullptr;
	sofa = nullptr;
	leafB = nullptr;
	prime1 = nullptr;
	domoInvernadero = nullptr;
	domoEstructura = nullptr;
	tunelMetal = nullptr;
	plantas = nullptr;

	loadingScreen.updateProgress("Modelos esenciales cargados exitosamente!");
}

CubeMap* loadSkybox(LoadingScreen& loadingScreen) {
	loadingScreen.updateProgress("Cargando skybox del espacio...");

	vector<std::string> faces{
		"monster_house/textures/cubemap/01/px.jpg",
		"monster_house/textures/cubemap/01/nx.jpg",
		"monster_house/textures/cubemap/01/py.jpg",
		"monster_house/textures/cubemap/01/ny.jpg",
		"monster_house/textures/cubemap/01/pz.jpg",
		"monster_house/textures/cubemap/01/nz.jpg"
	};

	CubeMap* mainCubeMap = new CubeMap();
	mainCubeMap->loadCubemap(faces);
	return mainCubeMap;
}

void setupLighting(size_t& sunLightIdx, size_t& invernaderoLightIdx,
	size_t& panelSolarLightIdx, size_t& interiorCasaLightIdx) {

	// Luz global: Sol
	Light sunLight;
	sunLight.Position = glm::vec3(35.0f, 80.0f, 700.0f);
	sunLight.Color = glm::vec4(1.0f, 0.98f, 0.92f, 1.0f);
	//sunLight.Power = 0.05f * glm::vec4(40.0f, 39.0f, 37.0f, 1.0f);
	sunLight.Power = 0.4f * glm::vec4(40.0f, 39.0f, 37.0f, 1.0f);
	sunLight.alphaIndex = 2;
	sunLightIdx = sceneManager->getLightManager().addLight(sunLight, true);

	// Luz local: Invernadero
	Light lightInvernadero;
	lightInvernadero.Position = glm::vec3(40.0f, 7.0f, -30.0f);
	lightInvernadero.Color = glm::vec4(0.95f, 1.0f, 0.97f, 1.0f);
	lightInvernadero.Power = glm::vec4(5.0f, 6.0f, 5.0f, 1.0f);
	lightInvernadero.alphaIndex = 3;
	invernaderoLightIdx = sceneManager->getLightManager().addLight(lightInvernadero, false);

	// Luz local: Panel Solar
	const glm::vec3 START_POS(-40.0f, 0.84f, 30.0f);
	Light luzPanelSolar;
	luzPanelSolar.Position = START_POS + glm::vec3(3.0f, 5.0f, 0.0f);
	luzPanelSolar.Color = glm::vec4(0.1f, 0.5f, 1.0f, 1.0f);
	luzPanelSolar.Power = glm::vec4(15.0f, 20.0f, 25.0f, 1.0f);
	luzPanelSolar.alphaIndex = 50;
	panelSolarLightIdx = sceneManager->getLightManager().addLight(luzPanelSolar, false);

	// Luz local: Interior Casa
	Light luzInteriorCasa;
	luzInteriorCasa.Position = glm::vec3(0.0f, 5.0f, 0.0f);
	luzInteriorCasa.Color = glm::vec4(1.0f, 0.9f, 0.7f, 1.0f);
	luzInteriorCasa.Power = 0.9f * glm::vec4(8.0f, 7.0f, 6.0f, 1.0f);
	luzInteriorCasa.alphaIndex = 3;
	interiorCasaLightIdx = sceneManager->getLightManager().addLight(luzInteriorCasa, false);
}

void defineMaterials(Material& astronautMaterial, Material& lunarFloorMaterial,
	Material& houseMaterial, Material& spaceshipMaterial,
	Material& sateliteMaterial, Material& solarPanelMaterial,
	Material& invernaderoMaterial, Material& escaleraMaterial,
	Material& puertaMaterial, Material& camaMaterial,
	Material& sofaLuna, Material& azulTransparente,
	Material& domoEstrutura, Material& tunelMetalMaterial,
	Material& neutroSol, Material& comedorMaterial, Material& materialPlantas) {

	// Materiales esenciales para el piso y la casa

	// Piso lunar
	lunarFloorMaterial.ambient = 1.9f * glm::vec4(0.15f, 0.15f, 0.15f, 1.0f);
	lunarFloorMaterial.diffuse = 1.4f * glm::vec4(0.5f, 0.5f, 0.55f, 1.0f);
	lunarFloorMaterial.specular = 2.0f * glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	lunarFloorMaterial.transparency = 1.0f;

	// Casa
	houseMaterial.ambient = glm::vec4(0.25f, 0.25f, 0.28f, 1.0f);
	houseMaterial.diffuse = 0.7f * glm::vec4(0.7f, 0.7f, 0.75f, 1.0f);
	houseMaterial.specular = 0.1f * glm::vec4(0.4f, 0.4f, 0.45f, 1.0f);
	houseMaterial.transparency = 1.0f;

	// Establecer los demás materiales a valores neutros/vacíos (ya no se usan)
	astronautMaterial = Material();
	spaceshipMaterial = Material();
	sateliteMaterial = Material();
	solarPanelMaterial = Material();
	invernaderoMaterial = Material();
	escaleraMaterial = Material();
	puertaMaterial = Material();
	camaMaterial = Material();
	sofaLuna = Material();
	azulTransparente = Material();
	domoEstrutura = Material();
	tunelMetalMaterial = Material();
	neutroSol = Material();
	comedorMaterial = Material();
	materialPlantas = Material();
}

void createSceneObjects(AnimatedModel* animatedAstronauta, Shader* dynamicShader,
	Model* piso, Model* house, Model* invernadero,
	Model* naveEspacial, Model* satelite, Model* satelite2,
	Model* sofa, Model* leafB, Model* prime1, Model* sol,
	Model* comedor,
	Model* plantasInvernadero,
	Shader* mLightsShader, Shader* mMoonShader,
	const Material& astronautMaterial, const Material& lunarFloorMaterial,
	const Material& houseMaterial, const Material& spaceshipMaterial,
	const Material& sateliteMaterial, const Material& invernaderoMaterial,
	const Material& sofaLuna, const Material& neutroSol,
	const Material& comedorMaterial,
	const Material& materialPlantas,
	size_t interiorCasaLightIdx, size_t invernaderoLightIdx,
	OrbitingMoonObject*& satellitePtr, OrbitingMoonObject*& satellite2Ptr,
	size_t& satelliteLightIndex, size_t& satellite2LightIndex) {

	// Piso
	auto floorObj = std::make_unique<RenderableObject>(
		piso, mLightsShader, glm::vec3(0.0f),
		glm::vec3(-90.0f, -90.0f, 0.0f), glm::vec3(5.0f));
	floorObj->setMaterial(lunarFloorMaterial);
	sceneManager->addObject(std::move(floorObj));

	// Casa
	// ESCALA AJUSTADA a 1.0f y POSICIÓN Y AJUSTADA a 0.0f para estar en el suelo
	auto houseObj = std::make_unique<RenderableObject>(
		house, mLightsShader, glm::vec3(0.0f,16.5f, 0.0f),
		glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(1.0f));
	houseObj->setMaterial(houseMaterial);
	houseObj->addAffectedLight(interiorCasaLightIdx);
	sceneManager->addObject(std::move(houseObj));



	// El resto de los objetos (astronauta, invernadero, naves, satélites, props) han sido eliminados.
	// Se dejan los punteros nulos para evitar fallos.
	satellitePtr = nullptr;
	satellite2Ptr = nullptr;
	satelliteLightIndex = 0;
	satellite2LightIndex = 0;
}

void createProps(Model* panelSolar, Model* escalera, Model* puerta, Model* cama,
	Model* domoInvernadero, Model* domoEstructura, Model* tunelMetal,
	Shader* mLightsShader,
	const Material& solarPanelMaterial, const Material& escaleraMaterial,
	const Material& puertaMaterial, const Material& camaMaterial,
	const Material& azulTransparente, const Material& domoEstrutura,
	const Material& tunelMetalMaterial,
	size_t panelSolarLightIdx, size_t interiorCasaLightIdx) {
	// Todos los objetos (props) han sido eliminados.
}

void setupHierarchy() {
	auto worldOrbitNode = std::make_unique<HierarchicalOrbitingObject>(
		static_cast<RenderableObject*>(nullptr));
	worldOrbitNode->setOrbitParameters(0.05f, 1000.0f, 0.95f);
	worldOrbitNode->setOrbitAngles(glm::vec3(0.0f));

	HierarchicalObject* rootPtr = worldOrbitNode.get();
	sceneManager->setWorldRoot(rootPtr);
	sceneManager->addHierarchicalObject(std::move(worldOrbitNode));
}
void generateSpaceships(Model* naveEspacial, Shader* mLightsShader,
	const Material& spaceshipMaterial) {
	// Se elimina la generación de naves espaciales.
	std::cout << "\n[DEMO] Generacion de flota de naves espaciales desactivada." << std::endl;
}
void generateLunarHousesExample(Model* house, Shader* mLightsShader,
	const Material& houseMaterial) {
	// Se elimina la generación de casas adicionales.
	std::cout << "\n[DEMO] Generacion de colonias lunares desactivada." << std::endl;
}

void setupDebugTools(OrbitingMoonObject* satellitePtr, OrbitingMoonObject* satellite2Ptr,
	size_t satelliteLightIndex, size_t satellite2LightIndex) {
	// Se eliminan todas las herramientas de debug relacionadas con satélites.
	// Se mantienen las variables para evitar errores de compilación, aunque sean nulas.
}

// ============================================================================
// AJUSTE DE ALTURA DE CÁMARA
// ============================================================================
void setupCameras() {
	camera.Position = position;
	camera.Position.y += 2.5f; // Altura de vista más alta
	camera.Front = forwardView;

	camera3rd.Position = position;
	camera3rd.Position.y += 2.7f; // Altura de vista más alta
	camera3rd.Position -= trdpersonOffset * forwardView;
	camera3rd.Front = forwardView;
}

void printControls() {
	std::cout << "\n=== CONTROLES ===" << std::endl;
	std::cout << "W/Flecha Arriba: Mover adelante" << std::endl;
	std::cout << "S/Flecha Abajo: Mover atras" << std::endl;
	std::cout << "A/D: Mover izquierda/derecha" << std::endl;
	std::cout << "Flechas: Rotar personaje" << std::endl;
	std::cout << "SPACE: Saltar" << std::endl;
	std::cout << "C: Cambiar camara" << std::endl;
	std::cout << "L: Toggle orbitas" << std::endl;
	std::cout << "Shift: Correr" << std::endl;
	std::cout << "Mouse: Rotacion" << std::endl;
	std::cout << "Scroll: Zoom\n" << std::endl;
}

// ============================================================================
// FUNCIÓN PRINCIPAL DE INICIALIZACIÓN
// ============================================================================

bool Start() {
	if (!initializeWindow()) return false;

	LoadingScreen loadingScreen(window, 24);
	loadingScreen.render();

	// Variables locales (se mantienen solo las necesarias o se inicializan a nullptr)
	Shader* cubemapShader, * dynamicShader, * mLightsShader, * mMoonShader;
	AnimatedModel* animatedAstronauta = nullptr; // Eliminado
	Model* house, * sol = nullptr, * piso, * naveEspacial = nullptr, * satelite = nullptr, * panelSolar = nullptr; // Eliminados
	Model* invernadero = nullptr, * escalera = nullptr, * puerta = nullptr, * cama = nullptr, * satelite2 = nullptr, * comedor = nullptr; // Eliminados
	Model* sofa = nullptr, * leafB = nullptr, * prime1 = nullptr, * domoInvernadero = nullptr, * domoEstructura = nullptr, * tunelMetal = nullptr; // Eliminados
	Model* plantas = nullptr; // Eliminado

	// Cargar recursos
	loadShaders(loadingScreen, cubemapShader, dynamicShader, mLightsShader, mMoonShader);
	// Solo cargará la casa y el piso
	loadModels(loadingScreen, animatedAstronauta, house, sol, piso, naveEspacial,
		satelite, panelSolar, invernadero, escalera, puerta, cama, satelite2,
		comedor, sofa, leafB, prime1, domoInvernadero, domoEstructura, tunelMetal, plantas);
	CubeMap* mainCubeMap = loadSkybox(loadingScreen);

	// Inicializar sistemas
	loadingScreen.updateProgress("Inicializando gestores de escena...");
	sceneManager = std::make_unique<SceneManager>(camera, camera3rd, activeCamera);
	sceneManager->setCubemap(mainCubeMap, cubemapShader);

	inputController = std::make_unique<InputController>(
		position, forwardView, rotateCharacter, activeCamera,
		trdpersonOffset, camera, camera3rd);

	physicsSystem.setGroundLevel(0.0f);

	// Configurar iluminación (SE MANTIENE)
	loadingScreen.updateProgress("Configurando sistema de iluminacion...");
	size_t sunLightIdx, invernaderoLightIdx, panelSolarLightIdx, interiorCasaLightIdx;
	setupLighting(sunLightIdx, invernaderoLightIdx, panelSolarLightIdx, interiorCasaLightIdx);

	// Definir materiales (solo se definen los necesarios)
	Material astronautMaterial, lunarFloorMaterial, houseMaterial, spaceshipMaterial;
	Material sateliteMaterial, solarPanelMaterial, invernaderoMaterial, escaleraMaterial;
	Material puertaMaterial, camaMaterial, sofaLuna, azulTransparente, domoEstrutura;
	Material tunelMetalMaterial, neutroSol, comedorMaterial, materialPlantas;

	defineMaterials(astronautMaterial, lunarFloorMaterial, houseMaterial, spaceshipMaterial,
		sateliteMaterial, solarPanelMaterial, invernaderoMaterial, escaleraMaterial,
		puertaMaterial, camaMaterial, sofaLuna, azulTransparente, domoEstrutura,
		tunelMetalMaterial, neutroSol, comedorMaterial, materialPlantas);

	// Crear objetos de la escena (SOLO CASA Y PISO)
	OrbitingMoonObject* satellitePtr = nullptr;
	OrbitingMoonObject* satellite2Ptr = nullptr;
	size_t satelliteLightIndex = 0, satellite2LightIndex = 0;

	createSceneObjects(animatedAstronauta, dynamicShader, piso, house, invernadero,
		naveEspacial, satelite, satelite2, sofa, leafB, prime1, sol, comedor,
		plantas,
		mLightsShader, mMoonShader, astronautMaterial, lunarFloorMaterial,
		houseMaterial, spaceshipMaterial, sateliteMaterial, invernaderoMaterial,
		sofaLuna, neutroSol, comedorMaterial, materialPlantas, interiorCasaLightIdx, invernaderoLightIdx,
		satellitePtr, satellite2Ptr, satelliteLightIndex, satellite2LightIndex);

	// Creación de props eliminada
	createProps(panelSolar, escalera, puerta, cama, domoInvernadero, domoEstructura, tunelMetal,
		mLightsShader, solarPanelMaterial, escaleraMaterial, puertaMaterial, camaMaterial,
		azulTransparente, domoEstrutura, tunelMetalMaterial, panelSolarLightIdx, interiorCasaLightIdx);

	// Configurar jerarquía y herramientas de debug
	setupHierarchy();
	setupDebugTools(satellitePtr, satellite2Ptr, satelliteLightIndex, satellite2LightIndex);

	// Generación de objetos extra eliminada
	loadingScreen.updateProgress("Generando colonias lunares...");
	generateLunarHousesExample(house, mLightsShader, houseMaterial);
	loadingScreen.updateProgress("Generando flota de naves espaciales...");
	generateSpaceships(naveEspacial, mLightsShader, spaceshipMaterial);

	// Configurar cámaras y finalizar (SE MANTIENE)
	setupCameras();
	loadingScreen.updateProgress("Finalizando inicializacion...");

	glfwSetWindowTitle(window, "Monster House");
	printControls();

	return true;
}

// ============================================================================
// LOOP PRINCIPAL
// ============================================================================

bool Update() {
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	processInput(window);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		float jumpDisplacement = physicsSystem.getCurrentVerticalDisplacement();

		if (activeCamera) {
			camera.Position = position;
			// Altura de 1st person ajustada
			camera.Position.y += 2.5f + jumpDisplacement;
			camera.Position += 0.37f * forwardView;
		}
		else {
			camera3rd.Position = position;
			// Altura de 3rd person ajustada
			camera3rd.Position.y += 2.7f + jumpDisplacement;
			camera3rd.Position -= trdpersonOffset * forwardView;
		}

		if (sceneManager) {
			sceneManager->update(deltaTime);
			sceneManager->render();
		}
	}

	float jumpDisplacement = physicsSystem.getCurrentVerticalDisplacement();

	if (activeCamera) {
		camera.Position = position;
		// MODIFICACIÓN: Altura aumentada a 7.0f
		camera.Position.y += 7.0f + jumpDisplacement;
		camera.Position += 0.37f * forwardView;
	}
	else {
		camera3rd.Position = position;
		// MODIFICACIÓN: Altura aumentada a 7.2f
		camera3rd.Position.y += 7.2f + jumpDisplacement;
		camera3rd.Position -= trdpersonOffset * forwardView;
	}

	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

// ============================================================================
// PUNTO DE ENTRADA
// ============================================================================

int main() {
	if (!Start())
		return -1;

	while (!glfwWindowShouldClose(window)) {
		if (!Update())
			break;
	}

	glfwTerminate();
	return 0;
}