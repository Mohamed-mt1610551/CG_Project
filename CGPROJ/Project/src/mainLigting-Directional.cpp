#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h"	
#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp" 
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "Mesh.h"
#include <GLM/gtc/type_ptr.hpp>

// Global Variables
const char* APP_TITLE = "Solar System";
int gWindowWidth = 1600;
int gWindowHeight = 1200;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;
const int numPlanets = 8; // Excluding sun and space
float planetAngles[numPlanets] = { 0.0f }; // Initial angles
float selfRotationAngles[numPlanets] = { 0.0f }; // Self-rotation angles for each model
GLfloat scale = 0.3f;


// experiment with translation
bool Pmove = true; // movement of planet
bool isPaused = false; // toggle pause on/off
float offset = 0.0f;
float maxOffest = 0.7f;
float increment = 0.01f;


FPSCamera fpsCamera(glm::vec3(20.0f, 20.0f, 10.0f));

const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.0; // units per second
const float MOUSE_SENSITIVITY = 0.1f;

// Function prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(double elapsedTime);
void showFPS(GLFWwindow* window);
bool initOpenGL();
void updatePlanet(double deltaTime);
void updateSelfRotation(double deltaTime);


int main()
{
	if (!initOpenGL())
	{
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	//setting shaders
	ShaderProgram shaderProgram, shaderProgramOneTex;
	shaderProgram.loadShaders("res/shaders/lighting-phong-mat-dir.vert", "res/shaders/lighting-phong-mat-dir.frag");
	ShaderProgram lightShader;
	lightShader.loadShaders("res/shaders/lamp.vert", "res/shaders/lamp.frag");


	// Load meshes and textures
	const int numModels = 10;
	Mesh mesh[numModels];
	Texture texture[numModels];
	mesh[0].loadOBJ("res/models/earth/Earth.obj"); // Space
	mesh[1].loadOBJ("res/models/mercury/mercury.obj"); //Mercury
	mesh[2].loadOBJ("res/models/Venus/venus.obj"); // Venus 
	mesh[3].loadOBJ("res/models/earth/Earth.obj"); // Earth        
	mesh[4].loadOBJ("res/models/mars/mars.obj"); // Mars
	mesh[5].loadOBJ("res/models/jupiter/jupiter.obj"); // Jupiter
	mesh[6].loadOBJ("res/models/saturn/13906_Saturn_v1_l3.obj"); // Saturn
	mesh[7].loadOBJ("res/models/uranus/13907_Uranus_v2_l3.obj"); // Uranus
	mesh[8].loadOBJ("res/models/neptune/13908_Neptune_V2_l3.obj"); // Neptune
	mesh[9].loadOBJ("res/models/sun/sun.obj"); //sun





	texture[0].loadTexture("res/models/Stars.jpg", true);
	texture[1].loadTexture("res/models/mercury/mercury-texture.jpeg", true);
	texture[2].loadTexture("res/models/Venus/venus-texture.jpg", true);
	texture[3].loadTexture("res/models/earth/Earth_TEXTURE_CM.tga", true); 
	texture[4].loadTexture("res/models/mars/mars-texture.jpg", true); 
	texture[5].loadTexture("res/models/jupiter/jupiter-texture.jpg", true);
	texture[6].loadTexture("res/models/saturn/Saturn_diff.jpg", true);
	texture[7].loadTexture("res/models/uranus/13907_Uranus_planet_diff.jpg", true);
	texture[8].loadTexture("res/models/neptune/13908_Neptune_planet_diff.jpg", true);
	texture[9].loadTexture("res/models/sun/13913_Sun_diff.jpg", true);

	// distance from sun horizental
	glm::vec3 modelPos[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),  // Sun (stationary)
		glm::vec3(20.0f, 0.0f, 0.0f),  // Mercury
		glm::vec3(30.5f, 0.0f, 0.0f),  // Venus
		glm::vec3(40.0f, 0.0f, 0.0f),  // Earth
		glm::vec3(50.0f, 0.0f, 0.0f),  // Mars
		glm::vec3(70.0f, 0.0f, 0.0f),  // Jupiter
		glm::vec3(90.0f, 0.0f, 0.0f),  // Saturn
		glm::vec3(120.0f, 0.0f, 0.0f),  // Uranus
		glm::vec3(150.0f, 0.0f, 0.0f),  // Neptune
		glm::vec3(0.0f, 0.0f, 0.0f),  // Space
	};


	// Model scale
	glm::vec3 modelScale[] = {
		glm::vec3(250.0f * scale),	// Space
		glm::vec3(0.3f * scale),	// Mercury
		glm::vec3(0.5f * scale),	// Venus
		glm::vec3(0.5f * scale),	// Earth
		glm::vec3(0.3f * scale),	// Mars
		glm::vec3(4.0f * scale),	// Jupiter
		glm::vec3(0.032f * scale),	// Saturn
		glm::vec3(0.03f * scale) ,	// Uranus
		glm::vec3(0.03f * scale),	// Neptune
		glm::vec3(20.0f *scale),	// Sun
	};



	//code to make the planets orbit abut the center of the sun (fix the offset)
float sunRadius = modelScale[9].x; // Assuming x is the radius for the scaling

// Adjust modelPos for planets to start orbiting at the edge of the sun's radius
for (int i = 1; i < numModels; i++) {
	// This assumes the orbit is in the x-z plane and the sun is at the origin
	modelPos[i].x += sunRadius;
}

	double lastTime = glfwGetTime();
	float angle = 0.0f;



	// Rendering loop 
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		// Poll for and process events
		glfwPollEvents();
		update(deltaTime);
		updatePlanet(deltaTime);      // Updates planet orbit angles
		updateSelfRotation(deltaTime); // Updates self-rotation angles

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0), view(1.0), projection(1.0);

		// Create the View matrix
		view = fpsCamera.getViewMatrix();

		// Create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 400.0f);

		//view position to be passed to fragment shader
		glm::vec3 viewPos;
		viewPos.x = fpsCamera.getPosition().x;
		viewPos.y = fpsCamera.getPosition().y;
		viewPos.z = fpsCamera.getPosition().z;

		//set the light
		glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f); //white color light

		////move the light on x dir
		//angle += (float)deltaTime * 50.0f;
		//lightPos.x = 10.0f * sinf(glm::radians(angle));

		shaderProgram.use();

		// Set lights properties
		glUniform3f(glGetUniformLocation(shaderProgram.getProgram(), "light.position"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shaderProgram.getProgram(), "light.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(shaderProgram.getProgram(), "light.specular"), 0.0f, 0.0f, 0.0f);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));

		for (int i = 0; i < numModels; i++) {
			glm::mat4 model = glm::mat4(1.0);
			glUniform3f(glGetUniformLocation(shaderProgram.getProgram(), "light.ambient"), 0.05f, 0.05f, 0.05f);
			if (i == 0 || i==9) { // Sun or sapce
				model = glm::translate(model, modelPos[i]) * glm::scale(model, modelScale[i]);

				if (i == 0) {
					glUniform3f(glGetUniformLocation(shaderProgram.getProgram(), "light.ambient"), 0.4f, 0.4f, 0.4f);

				}
			
				if (i == 9) {

					// Draw the light
			;
					lightShader.use();
					lightShader.setUniform("lightColor", lightColor);
					lightShader.setUniform("model", model);
					lightShader.setUniform("view", view);
					lightShader.setUniform("projection", projection);
	
				}			
			}
			else 
			{ 
				if (Pmove ==true)
				{
					// Orbit rotation
					float orbitX = cos(planetAngles[i - 1]) * modelPos[i].x;
					float orbitZ = sin(planetAngles[i - 1]) * modelPos[i].x;
					glm::vec3 orbitPos = glm::vec3(orbitX, modelPos[i].y + sunRadius / 2.0f, orbitZ);
					model = glm::translate(model, orbitPos) * glm::scale(model, modelScale[i]);
				}
				else 
				{
					model = glm::translate(model, modelPos[i]) * glm::scale(model, modelScale[i]);
				}
				// Self-rotation
				model = glm::rotate(model, glm::radians(selfRotationAngles[i-1]), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			shaderProgram.setUniform("model", model);

			//set material properties
			shaderProgram.setUniform("material.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
			shaderProgram.setUniformSampler("material.diffuseMap", 0);
			shaderProgram.setUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
			shaderProgram.setUniform("material.shininess", 45.0f);

		

			texture[i].bind(0);		// set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[i].draw();			// Render the OBJ mesh
			texture[i].unbind(0);
		}

		glfwSwapBuffers(gWindow);

		lastTime = currentTime;
	}

	glfwTerminate();

	return 0;
}


//-----------------------------------------------------------------------------
// Initialize GLFW and OpenGL
//-----------------------------------------------------------------------------
bool initOpenGL()
{
	// Intialize GLFW 
	// GLFW is configured.  Must be called before calling any GLFW functions
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3

	// Create an OpenGL 3.3 core, forward compatible context window
	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gWindow == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(gWindow);

	// Initialize GLEW
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

	// Hides and grabs cursor, unlimited movement
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CROSSHAIR_CURSOR);
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	glClearColor(0.3f, 0.4f, 0.6f, 1.0f);

	// Define the viewport dimensions
	glViewport(0, 0, gWindowWidth, gWindowHeight);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	return true;
}

//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------
// Called by GLFW when the mouse wheel is rotated
//-----------------------------------------------------------------------------
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

	fov = glm::clamp(fov, 1.0, 120.0);

	fpsCamera.setFOV((float)fov);
}


enum class PlanetType {
	Sun = 0,
	Mercury = 1,
	Venus = 2,
	Earth = 3,
	Moon = 4,
	Mars = 5,
	Jupiter = 6,
	Saturn = 7,
	Uranus = 8,
	Neptune = 9,
	FreeCamera = 10
};
PlanetType currentPlanet = PlanetType::FreeCamera;
//-----------------------------------------------------------------------------
// Update stuff every frame
//-----------------------------------------------------------------------------
void update(double elapsedTime)
{
	// Camera orientation
	double mouseX, mouseY;

	// Get the current mouse cursor position delta
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	// Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
	fpsCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	// Clamp mouse cursor to center of screen
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	// Camera FPS movement

//Go to planet
// Lock the camera to a planet based on key press
	
	if (glfwGetKey(gWindow, GLFW_KEY_0) == GLFW_PRESS)
		currentPlanet = PlanetType::Sun;
	else if (glfwGetKey(gWindow, GLFW_KEY_1) == GLFW_PRESS)
		currentPlanet = PlanetType::Mercury;
	else if (glfwGetKey(gWindow, GLFW_KEY_2) == GLFW_PRESS)
		currentPlanet = PlanetType::Venus;
	else if (glfwGetKey(gWindow, GLFW_KEY_3) == GLFW_PRESS)
		currentPlanet = PlanetType::Earth;
	else if (glfwGetKey(gWindow, GLFW_KEY_4) == GLFW_PRESS)
		currentPlanet = PlanetType::Moon;
	else if (glfwGetKey(gWindow, GLFW_KEY_5) == GLFW_PRESS)
		currentPlanet = PlanetType::Mars;
	else if (glfwGetKey(gWindow, GLFW_KEY_6) == GLFW_PRESS)
		currentPlanet = PlanetType::Jupiter;
	else if (glfwGetKey(gWindow, GLFW_KEY_7) == GLFW_PRESS)
		currentPlanet = PlanetType::Saturn;
	else if (glfwGetKey(gWindow, GLFW_KEY_8) == GLFW_PRESS)
		currentPlanet = PlanetType::Uranus;
	else if (glfwGetKey(gWindow, GLFW_KEY_9) == GLFW_PRESS)
		currentPlanet = PlanetType::Neptune;
	// Return to free camera control with the 'V' key
	 if (glfwGetKey(gWindow, GLFW_KEY_V) == GLFW_PRESS)
		currentPlanet = PlanetType::FreeCamera;



	 // Toggle movement of planets ON/OFF when 'P' is pressed
	 if (glfwGetKey(gWindow, GLFW_KEY_P) == GLFW_PRESS) {
		 Pmove = !Pmove;
	 }

	 // Check if the camera is not in free camera mode and pause accordingly
	 if (currentPlanet != PlanetType::FreeCamera) {
		 Pmove = false;	 
	 }

		 
	 

	// Update camera position based on the selected planet
	 /* Preset position  * scale 
	glm::vec3 modelPos[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),  // Sun (stationary)
		glm::vec3(20.0f, 0.0f, 0.0f),  // Mercury
		glm::vec3(30.5f, 0.0f, 0.0f),  // Venus
		glm::vec3(40.0f, 0.0f, 0.0f),  // Earth
		glm::vec3(50.0f, 0.0f, 0.0f),  // Mars
		glm::vec3(70.0f, 0.0f, 0.0f),  // Jupiter
		glm::vec3(90.0f, 0.0f, 0.0f),  // Saturn
		glm::vec3(120.0f, 0.0f, 0.0f),  // Uranus
		glm::vec3(150.0f, 0.0f, 0.0f)   // Neptune
	};
	 	glm::vec3 modelScale[] = {
		glm::vec3(3.0f, 3.0f, 3.0f),	// sun
		glm::vec3(0.2f, 0.2f, 0.2f),	// Venus
		glm::vec3(0.1f, 0.1f, 0.1f),	// Moon
		glm::vec3(0.2f, 0.2f, 0.2f),	// Earth
		glm::vec3(0.4f, 0.4f, 0.4f),	// Jupiter
	};
	 
	 */
	 // Need to add correct positions here 
	switch (currentPlanet) {
	case PlanetType::Sun:
		fpsCamera.setPosition(glm::vec3(-51.0f, 4.3f, 17.3f));
		fpsCamera.setLook(glm::vec3(0.989f, -0.061f, -0.128f));
		break;
	case PlanetType::Mercury:
		fpsCamera.setPosition(glm::vec3(25.55f, 0.015f, 2.09f));
		fpsCamera.setLook(glm::vec3(0.32f, -0.01f, -0.95f));
		break;
	case PlanetType::Venus:
		fpsCamera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		break;
	case PlanetType::Earth:
		fpsCamera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		break;
	case PlanetType::Moon:
		fpsCamera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		break;
	case PlanetType::Mars:
		fpsCamera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		break;
	case PlanetType::Jupiter:
		fpsCamera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		break;
	case PlanetType::Saturn:
		fpsCamera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		break;
	case PlanetType::Uranus:
		fpsCamera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		break;
	case PlanetType::Neptune:
		fpsCamera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		break;
	case PlanetType::FreeCamera:
		// Do nothing, allow free camera control
		break;
	}
	// Forward/backward
	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());

	// Strafe left/right
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
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

	elapsedSeconds = currentSeconds - previousSeconds;

	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;
		std::string str = "(" + std::to_string(fpsCamera.getPosition().x) + ", " + std::to_string(fpsCamera.getPosition().y) + ", " + std::to_string(fpsCamera.getPosition().z) + ")";
		std::string str1 = "(" + std::to_string(fpsCamera.getLook().x) + ", " + std::to_string(fpsCamera.getLook().y) + ", " + std::to_string(fpsCamera.getLook().z) + ")";
		// The C++ way of setting the window title
		std::ostringstream outs;
		outs.precision(3);	// decimal places
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)" << "    " 
			<< "CamPos:"<< str  << "    " << "Look pos:" << str1 ;
		
		glfwSetWindowTitle(window, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}


void updatePlanet(double deltaTime) {
	// Update planet angles
	float rotationSpeeds[numPlanets] = { 0.5f, 0.3f, 0.2f, 0.19f , 0.18f, 0.16f, 0.14f, 0.10f }; // Example speeds
	for (int i = 0; i < numPlanets; i++) {
		planetAngles[i] += rotationSpeeds[i] * deltaTime;
	}
}

void updateSelfRotation(double deltaTime) {
	float selfRotationSpeeds[numPlanets] = { 10.0f, 15.0f, 20.0f, 25.0f, 20.0f, 19.0f, 18.0f, 17.0f }; // Make sure Neptune has a speed

	for (int i = 0; i < numPlanets; i++) {
		selfRotationAngles[i] += selfRotationSpeeds[i] * deltaTime;

		// Correct if the angle exceeds 360 degrees
		if (selfRotationAngles[i] > 360.0f) {
			selfRotationAngles[i] -= 360.0f;
		}

		// Debug output
		std::cout << "Planet " << i << " rotation angle: " << selfRotationAngles[i] << std::endl;
	}
}