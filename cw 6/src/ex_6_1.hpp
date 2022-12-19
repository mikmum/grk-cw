#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "SOIL/SOIL.h"
#include "SOIL/stb_image_aug.h"

namespace texture {
	GLuint earth;
	GLuint clouds;
	GLuint moon;
	GLuint ship;
	GLuint planetTwo;
	GLuint planet;
	GLuint sun;
	GLuint scratches;
	GLuint rust;

	GLuint grid;

	GLuint earthNormal;
	GLuint asteroidNormal;
	GLuint shipNormal;
	GLuint moonNormal;
	GLuint testNormal;
	GLuint rustNormal;

	GLuint cubemapTexture;
}


GLuint program;
GLuint programSun;
GLuint programTex;
GLuint programEarth;
GLuint programShip;
GLuint programProcTex;
GLuint programSkybox;
Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext asteroidContext;
Core::RenderContext cubeContext;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 spaceshipPos = glm::vec3(-4.f, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);
GLuint VAO, VBO;
glm::vec3 lightColor = glm::vec3(0.9, 0.7, 0.8);
glm::vec3 lightDir = glm::normalize(glm::vec3(0.8f, -0.5f, 0.5f));
glm::vec3 lightPos = glm::vec3(0.0, 0.0, 0.0);

float aspectRatio = 1.f;
float E = 0.1;

float lastTime = -1.f;
float deltaTime = 0.f;

std::vector<std::string> faces =
{
	"textures/skybox/space_rt.png",
	"textures/skybox/space_lf.png",
	"textures/skybox/space_up.png",
	"textures/skybox/space_dn.png",
	"textures/skybox/space_bk.png",
	"textures/skybox/space_ft.png",
};

void loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &texture::cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::cubemapTexture);

	int w, h;
	unsigned char* data;
	for (unsigned int i = 0; i < 6; i++)
	{
		data = SOIL_load_image(faces[i].c_str(), &w, &h, 0, SOIL_LOAD_RGBA);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
		);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}


void updateDeltaTime(float time) {
	if (lastTime < 0) {
		lastTime = time;
		return;
	}

	deltaTime = time - lastTime;
	if (deltaTime > 0.1) deltaTime = 0.1;
	lastTime = time;
}

glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide, cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{

	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 20.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f + n) / (n - f),2 * f * n / (n - f),
		0.,0.,-1.,0.,
		});


	perspectiveMatrix = glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

glm::mat4 createSpaceshipMatrix()
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 spaceshipRotationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});

	spaceshipRotationMatrix = glm::transpose(spaceshipRotationMatrix);

	glm::mat4 spaceshipMatrix = spaceshipRotationMatrix * glm::translate(-spaceshipPos);

	return spaceshipMatrix;
}

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, GLuint program) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(glGetUniformLocation(program, "E"), E);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(program, "spotPos"), spaceshipPos.x, spaceshipPos.y, spaceshipPos.z);
	glUniform3f(glGetUniformLocation(program, "spotDir"), spaceshipDir.x, spaceshipDir.y, spaceshipDir.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	Core::DrawContext(context);

}

void drawSkyBox(Core::RenderContext& context, glm::mat4 modelMatrix) {
	glDisable(GL_DEPTH_TEST);
	glUseProgram(programSkybox);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform1i(glGetUniformLocation(programSkybox, "skybox"), 0);
	Core::DrawContext(context);
	glEnable(GL_DEPTH_TEST);
}

void drawObject(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, glm::vec3 colorTwo, GLuint program) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "color"), colorTwo.x, colorTwo.y, colorTwo.z);

	Core::DrawContext(context);

}

void drawObjectTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint texture, GLuint program, GLuint normal) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(glGetUniformLocation(program, "E"), E);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(program, "spotPos"), spaceshipPos.x, spaceshipPos.y, spaceshipPos.z);
	glUniform3f(glGetUniformLocation(program, "spotDir"), cameraDir.x, cameraDir.y, cameraDir.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "camerPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(texture, "colorTexture", program, 0);
	Core::SetActiveTexture(normal, "normalSampler", program, 1);
	Core::DrawContext(context);

}

void drawWithTwoTextures(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint texture, GLuint textureTwo, GLuint program) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(glGetUniformLocation(program, "E"), E);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(program, "spotPos"), spaceshipPos.x, spaceshipPos.y, spaceshipPos.z);
	glUniform3f(glGetUniformLocation(program, "spotDir"), cameraDir.x, cameraDir.y, cameraDir.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(texture, "colorTexture", program, 0);
	Core::SetActiveTexture(textureTwo, "clouds", program, 2);
	Core::DrawContext(context);

}

void drawShip(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint texture, GLuint textureTwo, GLuint textureThree, GLuint normalOne, GLuint normalTwo, GLuint program) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(glGetUniformLocation(program, "E"), E);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform3f(glGetUniformLocation(program, "spotPos"), spaceshipPos.x, spaceshipPos.y, spaceshipPos.z);
	glUniform3f(glGetUniformLocation(program, "spotDir"), spaceshipDir.x, spaceshipDir.y, spaceshipDir.z);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(texture, "colorTexture", program, 0);
	Core::SetActiveTexture(textureTwo, "rust", program, 2);
	Core::SetActiveTexture(textureThree, "scratches", program, 3);
	Core::SetActiveTexture(normalOne, "shipNormal", program, 4);
	Core::SetActiveTexture(normalTwo, "rustNormal", program, 5);
	Core::DrawContext(context);

}


void createAsteroidBelt(float time)
{
	for (int i = 0; i < 64; i++)
	{
		drawObjectColor(asteroidContext, glm::translate(glm::vec3(7 * cos(time / 5 + i * 6.28 / 64), 0, 7 * sin(time / 5 + i * 6.28 / 64))) * glm::scale(glm::vec3(0.0001f, 0.0001f, 0.0001f)), glm::vec3(0.3f, 0.2f, 0.1f), program);
	}
}

void renderScene(GLFWwindow* window)
{


	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float time = glfwGetTime();
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();

	deltaTime = time - lastTime;
	lastTime = time;

	drawSkyBox(cubeContext, glm::translate(cameraPos));

	glUseProgram(program);

	glm::vec3 custom_color = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 custom_color2 = glm::vec3(1.f, 1.f, 0.f);
	glm::vec3 custom_color3 = glm::vec3(0.f, 1.f, 1.f);
	glm::vec3 custom_color4 = glm::vec3(1.f, 1.f, 1.f);

	drawObjectColor(sphereContext, glm::translate(glm::vec3(5, 0, 5)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), custom_color2, program);
	drawObjectColor(sphereContext, glm::translate(glm::vec3(-5, 0, -5)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), custom_color2, program);
	drawObjectColor(sphereContext, glm::translate(glm::vec3(5, 0, -5)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), custom_color2, program);
	drawObjectColor(sphereContext, glm::translate(glm::vec3(-5, 0, 5)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), custom_color2, program);
	drawObjectColor(sphereContext, glm::translate(glm::vec3(10, 0, -10)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), custom_color2, program);

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(programSun);

	drawObjectTexture(sphereContext, glm::mat4(), texture::sun, programSun, texture::moonNormal);

	glUseProgram(programEarth);

	drawWithTwoTextures(sphereContext, glm::translate(glm::vec3(3 * cos(time), 0, 3 * sin(time))) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f)), texture::earth, texture::clouds, programEarth); //ziemia
	glUseProgram(programTex);

	drawObjectTexture(sphereContext, glm::translate(glm::vec3(1.5 * cos(2 * time), 0, 1.5 * sin(2 * time))) * glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)), texture::moon, programTex, texture::testNormal);

	drawObjectTexture(sphereContext, glm::translate(glm::vec3(3 * cos(time), 0, 3 * sin(time))) * glm::translate(glm::vec3(cos(-time), 0, sin(-time))) * glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)), texture::moon, programTex, texture::moonNormal); //księżyc
	drawObjectTexture(sphereContext, glm::translate(glm::vec3(6 * cos(time / 2), 0, 6 * sin(time / 2))) * glm::scale(glm::vec3(0.7f, 0.7f, 0.7f)), texture::planet, programTex, texture::testNormal);

	drawObjectTexture(sphereContext, glm::translate(glm::vec3(9 * cos(time / 3), 0, 9 * sin(time / 3))), texture::planetTwo, programTex, texture::moonNormal);

	glUseProgram(programProcTex);

	drawObject(sphereContext, glm::translate(glm::vec3(12 * cos(time / 4), 0, 12 * sin(time / 4))) * glm::scale(glm::vec3(1.2f, 1.2f, 1.2f)), custom_color, custom_color2, programProcTex);

	glUseProgram(program);

	createAsteroidBelt(time);

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});
	glUseProgram(programShip);

	drawShip(shipContext,
		glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.2f)),
		texture::ship, texture::rust, texture::scratches, texture::shipNormal, texture::rustNormal, programShip
	);



	glUseProgram(0);
	glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
}
void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_5_1.vert", "shaders/shader_5_1.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_5_sun.vert", "shaders/shader_5_sun.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_5_1_tex.vert", "shaders/shader_5_1_tex.frag");
	programEarth = shaderLoader.CreateProgram("shaders/shader_5_1_earth.vert", "shaders/shader_5_1_earth.frag");
	programShip = shaderLoader.CreateProgram("shaders/shader_5_1_ship.vert", "shaders/shader_5_1_ship.frag");
	programProcTex = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);
	loadModelToContext("./models/asteroid.obj", asteroidContext);
	loadModelToContext("./models/cube.obj", cubeContext);

	texture::earth = Core::LoadTexture("./textures/earth.png");
	texture::earthNormal = Core::LoadTexture("./textures/earth_normalmap.png");
	texture::shipNormal = Core::LoadTexture("./textures/spaceship_normal.jpg");
	texture::moonNormal = Core::LoadTexture("./textures/moon_normals.png");
	texture::testNormal = Core::LoadTexture("./textures/test_normals.png");
	texture::rustNormal = Core::LoadTexture("./textures/rust_normal.jpg");
	texture::grid = Core::LoadTexture("./textures/grid.png");
	texture::moon = Core::LoadTexture("./textures/test.png");
	texture::planet = Core::LoadTexture("./textures/rust.jpg");
	texture::planetTwo = Core::LoadTexture("./textures/jupiter.jpg");
	texture::sun = Core::LoadTexture("./textures/sun.png");
	texture::clouds = Core::LoadTexture("./textures/clouds.jpg");
	texture::rust = Core::LoadTexture("./textures/rust.jpg");
	texture::scratches = Core::LoadTexture("./textures/scratches.jpg");
	loadCubemap(faces);
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
	shaderLoader.DeleteProgram(programSun);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 0.05f * deltaTime * 60;
	float moveSpeed = 0.05f * deltaTime * 60;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		angleSpeed *= 3;
		moveSpeed *= 3;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		spaceshipPos += spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		spaceshipPos -= spaceshipDir * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		spaceshipPos += spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		spaceshipPos -= spaceshipSide * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		spaceshipPos += spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		spaceshipPos -= spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		E = E - 0.1;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		E = E + 0.1;

	cameraPos = spaceshipPos - 0.3f * spaceshipDir + glm::vec3(0, 1, 0) * 0.1f;
	cameraDir = spaceshipDir;

	//cameraDir = glm::normalize(-cameraPos);

}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}
//}