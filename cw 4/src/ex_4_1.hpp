#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>



GLuint program;
Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext asteroidContext;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);

glm::vec3 spaceshipPos = glm::vec3(-4.f, 0, 0);
glm::vec3 spaceshipDir = glm::vec3(1.f, 0.f, 0.f);
GLuint VAO,VBO;

float aspectRatio = 1.f;

float lastFrameTime = 0.f;
float deltaTime = 0.f;

glm::vec3 lightColor = glm::vec3(0.9, 0.7, 0.8);

glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir,glm::vec3(0.f,1.f,0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide,cameraDir));
	//glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});

	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);

	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(-cameraPos);

	//cameraMatrix = glm::mat4({
	//	1.,0.,0.,cameraPos.x,
	//	0.,1.,0.,cameraPos.y,
	//	0.,0.,1.,cameraPos.z,
	//	0.,0.,0.,1.,
	//	});

	//cameraMatrix = glm::transpose(cameraMatrix);
	//return Core::createViewMatrix(cameraPos, cameraDir, up);

	return cameraMatrix;
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

glm::mat4 createPerspectiveMatrix()
{
	
	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 20.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,1,0.,0.,
		0.,0.,(f+n) / (n - f),2*f * n / (n - f),
		0.,0.,-1.,0.,
		});

	
	perspectiveMatrix=glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}




void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color) {
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);

	glUniform3f(glGetUniformLocation(program, "in_color"), color.r, color.g, color.b);

	Core::DrawContext(context);
}

void createAsteroidBelt(float time)
{
	for (int i = 0; i < 64; i++)
	{
		drawObjectColor(asteroidContext, glm::translate(glm::vec3(7 * cos(time / 5 + i*6.28/64), 0, 7 * sin(time / 5 + i*6.28/64))) * glm::scale(glm::vec3(0.0001f, 0.0001f, 0.0001f)), glm::vec3(0.3f, 0.2f, 0.1f));
	}
}

void renderScene(GLFWwindow* window)
{


	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glm::mat4 transformation;
	float time = glfwGetTime();
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();

	deltaTime = time - lastFrameTime;
	lastFrameTime = time;


	glUseProgram(program);

	glm::vec3 custom_color = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 custom_color2 = glm::vec3(1.f, 1.f, 0.f);
	glm::vec3 custom_color3 = glm::vec3(0.f, 1.f, 1.f);
	glm::vec3 custom_color4 = glm::vec3(1.f, 1.f, 1.f);

	drawObjectColor(sphereContext, glm::translate(glm::vec3(5, 0, 5)) * glm::scale(glm::vec3(0.1f,0.1f,0.1f)), custom_color2);
	drawObjectColor(sphereContext, glm::translate(glm::vec3(-5, 0, -5)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), custom_color2);
	drawObjectColor(sphereContext, glm::translate(glm::vec3(5, 0, -5)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), custom_color2);
	drawObjectColor(sphereContext, glm::translate(glm::vec3(-5, 0, 5)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), custom_color2);
	drawObjectColor(sphereContext, glm::translate(glm::vec3(10, 0, -10)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)), custom_color2);
	
	glClear( GL_DEPTH_BUFFER_BIT );

	drawObjectColor(sphereContext, glm::translate(glm::vec3(0, 0, 0)), custom_color2);
	Core::DrawContext(sphereContext);

	drawObjectColor(sphereContext, glm::translate(glm::vec3( 1.5*cos(2*time), 0, 1.5*sin(2*time))) * glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)), glm::vec3(0.3f, 0.2f, 0.1f));
	drawObjectColor(sphereContext, glm::translate(glm::vec3(3 * cos(time), 0, 3 * sin(time))) * glm::scale(glm::vec3(0.5f, 0.5f, 0.5f) ), custom_color);

	drawObjectColor(sphereContext, glm::translate(glm::vec3(6 * cos(time/2), 0, 6 * sin(time/2))) * glm::scale(glm::vec3(0.7f, 0.7f, 0.7f)), glm::vec3(0.5f, 0.5f, 0.5f));

	drawObjectColor(sphereContext, glm::translate(glm::vec3(9 * cos(time / 3), 0, 9 * sin(time / 3))), glm::vec3(0.4f, 0.5f, 0.8f));

	drawObjectColor(sphereContext, glm::translate(glm::vec3(12 * cos(time / 4), 0, 12 * sin(time / 4))) * glm::scale(glm::vec3(1.2f, 1.2f, 1.2f)), glm::vec3(0.7f, 0.2f, 0.3f));

	drawObjectColor(sphereContext, glm::translate(glm::vec3(3 * cos(time), 0, 3 * sin(time))) * glm::translate(glm::vec3( cos(-time), 0, sin(-time))) * glm::scale(glm::vec3(0.2f, 0.2f, 0.2f)), custom_color4);


	createAsteroidBelt(time);

	drawObjectColor(shipContext,glm::scale(glm::rotate(glm::rotate(glm::inverse(createSpaceshipMatrix()), glm::radians(90.f), glm::vec3(1.f,0.0f,0.f)), glm::radians(180.f), glm::vec3(0.0f,0.0f,1.0f)), glm::vec3(0.003f,0.003f,0.003f)), custom_color3);
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

	glEnable(GL_DEPTH_TEST); //jak wyłączymy to jest problem, że rysuje wierzchołki będące za obiektami
	program = shaderLoader.CreateProgram("shaders/shader_4_1.vert", "shaders/shader_4_1.frag");

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/ferry.obj", shipContext);
	loadModelToContext("./models/asteroid.obj", asteroidContext);

}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	//glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir, glm::vec3(0.f,1.f,0.f)));
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	float angleSpeed = 1.f * deltaTime;
	float moveSpeed = 1.f * deltaTime;
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
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(spaceshipDir, 0));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		spaceshipDir = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(spaceshipDir, 0));

	cameraPos = spaceshipPos - 1.5 * spaceshipDir - glm::vec3(0, -0.5f, 0);
	cameraDir = spaceshipDir;

	//spaceshipPos = cameraPos + 1.5 * cameraDir + glm::vec3(0, -0.5f, 0);
	//spaceshipDir = cameraDir;

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