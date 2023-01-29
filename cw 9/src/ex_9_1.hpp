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

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

int WIDTH = 500, HEIGHT = 500;

namespace texture {
	GLuint cubemapTexture;
	GLuint ball;
	GLuint door;
}
namespace normal {
	GLuint ball;
	GLuint door;
}

std::vector<std::string> faces =
{
	"textures/space_rt.png",
	"textures/space_lf.png",
	"textures/space_up.png",
	"textures/space_dn.png",
	"textures/space_bk.png",
	"textures/space_ft.png",
};

namespace models {
	Core::RenderContext bedContext;
	Core::RenderContext chairContext;
	Core::RenderContext chairTwoContext;
	Core::RenderContext chairThreeContext;
	Core::RenderContext deskContext;
	Core::RenderContext doorContext;
	Core::RenderContext drawerContext;
	Core::RenderContext marbleBustContext;
	Core::RenderContext materaceContext;
	Core::RenderContext pencilsContext;
	Core::RenderContext planeContext;
	Core::RenderContext roomContext;
	Core::RenderContext spaceshipContext;
	Core::RenderContext sphereContext;
	Core::RenderContext windowContext;
	Core::RenderContext window2Context;
	Core::RenderContext testContext;
	Core::RenderContext cubeContext;
	Core::RenderContext tableContext;
	Core::RenderContext wardrobeContext;
	Core::RenderContext potContext;
	Core::RenderContext ballContext;
	Core::RenderContext pillowContext;
	Core::RenderContext pillowTwoContext;
	Core::RenderContext switchContext;
}

GLuint depthMapFBO;
GLuint depthMap;

GLuint spotlightDepthMapFBO;
GLuint spotlightDepthMap;

GLuint program;
GLuint programSun;
GLuint programTest;
GLuint programTex;
GLuint programSkybox;
GLuint programDepth;

Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;

glm::vec3 sunPos = glm::vec3(-4.740971f, 2.149999f, 0.369280f);
glm::vec3 sunDir = glm::vec3(-0.93633f, 0.351106, 0.003226f);
glm::vec3 sunColor = glm::vec3(0.9f, 0.9f, 0.7f)*5;

glm::vec3 cameraPos = glm::vec3(0.479490f, 1.250000f, -2.124680f);
glm::vec3 cameraDir = glm::vec3(-0.354510f, 0.000000f, 0.935054f);


glm::vec3 spaceshipPos = glm::vec3(0.065808f, 1.250000f, -2.189549f);
glm::vec3 spaceshipDir = glm::vec3(-0.490263f, 0.000000f, 0.871578f);
GLuint VAO,VBO;

float aspectRatio = 1.f;

float exposition = 1.f;

glm::mat4 lightVP;
glm::mat4 spotlightVP;

glm::mat4 specshipCameraRotrationMatrix;

glm::vec3 pointlightPos = glm::vec3(0, 2, 0);
glm::vec3 pointlightColor = glm::vec3(0.9, 0.6, 0.6)*8;

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.4, 0.4, 0.9)*10;
float spotlightPhi = 3.14 / 4;

bool lightSwitch = true;

float ballMove = 0.1f;


float lastTime = -1.f;
float deltaTime = 0.f;

void updateDeltaTime(float time) {
	if (lastTime < 0) {
		lastTime = time;
		return;
	}

	deltaTime = time - lastTime;
	if (deltaTime > 0.1) deltaTime = 0.1;
	lastTime = time;
}

void initDepthMap()
{
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initDepthMapSpotlight()
{
	glGenFramebuffers(1, &spotlightDepthMapFBO);

	glGenTextures(1, &spotlightDepthMap);
	glBindTexture(GL_TEXTURE_2D, spotlightDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, spotlightDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, spotlightDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir,glm::vec3(0.f,1.f,0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide,cameraDir));
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

glm::mat4 createPerspectiveMatrix(float renderScale)
{
	
	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 20.;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0. * renderScale,0.,0.,
		0.,aspectRatio * renderScale,0.,0.,
		0.,0.,(f+n) / (n - f),2*f * n / (n - f),
		0.,0.,-1.,0.,
		});

	
	perspectiveMatrix=glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectPBR(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, glm::mat4 viewProjection, glm::mat4 spotlightVP,float roughness, float metallic) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix(1.f) * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program, "depthMap"), 0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniformMatrix4fv(glGetUniformLocation(program, "viewProjectionMatrix"), 1, GL_FALSE, (float*)&viewProjection);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(program, "spotlightDepthMap"), 1);
	glBindTexture(GL_TEXTURE_2D, spotlightDepthMap);
	glUniformMatrix4fv(glGetUniformLocation(program, "spotlightVP"), 1, GL_FALSE, (float*)&spotlightVP);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(program, "metallic"), metallic);

	glUniform3f(glGetUniformLocation(program, "color"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(program, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3f(glGetUniformLocation(program, "lightPos"), pointlightPos.x, pointlightPos.y, pointlightPos.z);

	if (lightSwitch)
	{
		glUniform3f(glGetUniformLocation(program, "lightColor"), pointlightColor.x, pointlightColor.y, pointlightColor.z);
	}
	else
	{
		glUniform3f(glGetUniformLocation(program, "lightColor"), 0.f, 0.f, 0.f);
	}

	glUniform3f(glGetUniformLocation(program, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(program, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(program, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(program, "spotlightPhi"), spotlightPhi);

	Core::DrawContext(context);

}

void drawObjectPBRTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint texture, GLuint normal, glm::mat4 viewProjection, glm::mat4 spotlightVP, float roughness, float metallic) {

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix(1.f) * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programTex, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(programTex, "depthMap"), 0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "viewProjectionMatrix"), 1, GL_FALSE, (float*)&viewProjection);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(programTex, "spotlightDepthMap"), 1);
	glBindTexture(GL_TEXTURE_2D, spotlightDepthMap);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "spotlightVP"), 1, GL_FALSE, (float*)&spotlightVP);

	glUniform1f(glGetUniformLocation(programTex, "exposition"), exposition);

	glUniform1f(glGetUniformLocation(programTex, "roughness"), roughness);
	glUniform1f(glGetUniformLocation(programTex, "metallic"), metallic);

	glUniform3f(glGetUniformLocation(programTex, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(programTex, "sunDir"), sunDir.x, sunDir.y, sunDir.z);
	glUniform3f(glGetUniformLocation(programTex, "sunColor"), sunColor.x, sunColor.y, sunColor.z);

	glUniform3f(glGetUniformLocation(programTex, "lightPos"), pointlightPos.x, pointlightPos.y, pointlightPos.z);

	if (lightSwitch)
	{
		glUniform3f(glGetUniformLocation(programTex, "lightColor"), pointlightColor.x, pointlightColor.y, pointlightColor.z);
	}
	else
	{
		glUniform3f(glGetUniformLocation(programTex, "lightColor"), 0.f, 0.f, 0.f);
	}

	glUniform3f(glGetUniformLocation(programTex, "spotlightConeDir"), spotlightConeDir.x, spotlightConeDir.y, spotlightConeDir.z);
	glUniform3f(glGetUniformLocation(programTex, "spotlightPos"), spotlightPos.x, spotlightPos.y, spotlightPos.z);
	glUniform3f(glGetUniformLocation(programTex, "spotlightColor"), spotlightColor.x, spotlightColor.y, spotlightColor.z);
	glUniform1f(glGetUniformLocation(programTex, "spotlightPhi"), spotlightPhi);

	Core::SetActiveTexture(texture, "colorTexture", programTex, 2);
	Core::SetActiveTexture(normal, "normalSampler", programTex, 3);

	Core::DrawContext(context);

}

void drawSkyBox(Core::RenderContext& context, glm::mat4 modelMatrix) {
	glDisable(GL_DEPTH_TEST);
	glUseProgram(programSkybox);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix(1.f) * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform1i(glGetUniformLocation(programSkybox, "skybox"), 0);
	Core::DrawContext(context);
	glEnable(GL_DEPTH_TEST);
}

void drawObjectDepth(Core::RenderContext& context, glm::mat4 viewProjection, glm::mat4 modelMatrix)
{
	glUniformMatrix4fv(glGetUniformLocation(programDepth, "viewProjectionMatrix"), 1, GL_FALSE, (float*)&viewProjection);
	glUniformMatrix4fv(glGetUniformLocation(programDepth, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	Core::DrawContext(context);

}

void renderShadowapSun() {
	float time = glfwGetTime();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//uzupelnij o renderowanie glebokosci do teksturys
	//ustawianie przestrzeni rysowania 
	//bindowanie FBO
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//czyszczenie mapy głębokości 
	glClear(GL_DEPTH_BUFFER_BIT);
	//ustawianie programu
	glUseProgram(programDepth);
	lightVP = glm::ortho(-5.5f, 4.f, -3.f, 3.f, 1.0f, 15.0f) * glm::lookAt(sunPos, sunPos - sunDir, glm::vec3(0, 1, 0));

	drawObjectDepth(sphereContext, lightVP, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, -5.f, 0)) * glm::scale(glm::vec3(0.3f)));

	drawObjectDepth(sphereContext, lightVP,
		glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, -5.f, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)));


	drawObjectDepth(models::bedContext, lightVP, glm::mat4());
	drawObjectDepth(models::chairContext, lightVP, glm::mat4());
	drawObjectDepth(models::chairTwoContext, lightVP, glm::mat4());
	drawObjectDepth(models::chairThreeContext, lightVP, glm::mat4());
	drawObjectDepth(models::deskContext, lightVP, glm::mat4());
	drawObjectDepth(models::doorContext, lightVP, glm::mat4());
	drawObjectDepth(models::drawerContext, lightVP, glm::mat4());
	drawObjectDepth(models::marbleBustContext, lightVP, glm::mat4());
	drawObjectDepth(models::materaceContext, lightVP, glm::mat4());
	drawObjectDepth(models::pillowContext, lightVP, glm::mat4());
	drawObjectDepth(models::pillowTwoContext, lightVP, glm::mat4());
	drawObjectDepth(models::pencilsContext, lightVP, glm::mat4());
	drawObjectDepth(models::planeContext, lightVP, glm::mat4());
	drawObjectDepth(models::roomContext, lightVP, glm::mat4());
	drawObjectDepth(models::windowContext, lightVP, glm::mat4());
	drawObjectDepth(models::window2Context, lightVP, glm::mat4());
	drawObjectDepth(models::tableContext, lightVP,glm::mat4());
	drawObjectDepth(models::wardrobeContext, lightVP, glm::mat4());
	drawObjectDepth(models::potContext, lightVP, glm::mat4());


	drawObjectDepth(models::ballContext, lightVP, glm::translate(glm::vec3(-0.46428f, -0.95f, ballMove + 3.3592f)) * glm::eulerAngleX(ballMove * 1.5f));


	if (lightSwitch)
	{
		drawObjectDepth(models::switchContext, lightVP, glm::eulerAngleY(-3.14f / 2.f) * glm::translate(glm::vec3(0.f, 0.f, -3.0899f)));
	}
	else
	{
		drawObjectDepth(models::switchContext, lightVP, glm::eulerAngleZ(3.14f) * glm::eulerAngleY(3.14f / 2.f) * glm::translate(glm::vec3(0.f, 0.f, -3.0899f)));
	}

	//drawObjectDepth(shipContext, lightVP, glm::translate(spaceshipPos) * specshipCameraRotrationMatrix);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void renderShadowapSpotlight() {
	float time = glfwGetTime();
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//uzupelnij o renderowanie glebokosci do teksturys
	//ustawianie przestrzeni rysowania 
	//bindowanie FBO
	glBindFramebuffer(GL_FRAMEBUFFER, spotlightDepthMapFBO);
	//czyszczenie mapy głębokości 
	glClear(GL_DEPTH_BUFFER_BIT);
	//ustawianie programu
	glUseProgram(programDepth);
	spotlightVP = createPerspectiveMatrix(0.5f) * glm::lookAt(spotlightPos, spotlightPos + spotlightConeDir, glm::vec3(0, 1, 0));

	drawObjectDepth(sphereContext, spotlightVP, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, -5.f, 0)) * glm::scale(glm::vec3(0.3f)));

	drawObjectDepth(sphereContext, spotlightVP,
		glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, -5.f, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)));

	drawObjectDepth(models::bedContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::chairContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::chairTwoContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::chairThreeContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::deskContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::doorContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::drawerContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::marbleBustContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::materaceContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::pillowContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::pillowTwoContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::pencilsContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::planeContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::roomContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::windowContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::window2Context, spotlightVP, glm::mat4());
	drawObjectDepth(models::tableContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::wardrobeContext, spotlightVP, glm::mat4());
	drawObjectDepth(models::potContext, spotlightVP, glm::mat4());

	if (lightSwitch)
	{
		drawObjectDepth(models::switchContext, spotlightVP, glm::eulerAngleY(-3.14f / 2.f) * glm::translate(glm::vec3(0.f, 0.f, -3.0899f)));
	}
	else
	{
		drawObjectDepth(models::switchContext, spotlightVP, glm::eulerAngleZ(3.14f) * glm::eulerAngleY(3.14f / 2.f) * glm::translate(glm::vec3(0.f, 0.f, -3.0899f)));
	}


	drawObjectDepth(models::ballContext, spotlightVP, glm::translate(glm::vec3(-0.46428f, -0.95f, ballMove + 3.3592f)) * glm::eulerAngleX(ballMove * 1.5f));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}

void renderScene(GLFWwindow* window)
{
	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float time = glfwGetTime();
	updateDeltaTime(time);
	renderShadowapSun();
	renderShadowapSpotlight();

	drawSkyBox(models::cubeContext, glm::translate(cameraPos));

	//space lamp
	glUseProgram(programSun);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix(1.f) * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::translate(glm::vec3(0.f, -5.f, 0));
	glUniformMatrix4fv(glGetUniformLocation(programSun, "transformation"), 1, GL_FALSE, (float*)&transformation);
	if (lightSwitch)
	{
		glUniform3f(glGetUniformLocation(programSun, "color"), sunColor.x / 2, sunColor.y / 2, sunColor.z / 2);
	}
	else
	{
		glUniform3f(glGetUniformLocation(programSun, "color"), sunColor.x / 20, sunColor.y / 20, sunColor.z / 20);
	}
	glUniform1f(glGetUniformLocation(programSun, "exposition"), exposition);
	Core::DrawContext(sphereContext);

	glUseProgram(program);

	drawObjectPBR(sphereContext, glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, -5.f, 0)) * glm::scale(glm::vec3(0.3f)), glm::vec3(0.2, 0.7, 0.3), lightVP, spotlightVP, 0.3, 0.0);

	drawObjectPBR(sphereContext,
		glm::translate(pointlightPos) * glm::scale(glm::vec3(0.1)) * glm::eulerAngleY(time / 3) * glm::translate(glm::vec3(4.f, -5.f, 0)) * glm::eulerAngleY(time) * glm::translate(glm::vec3(1.f, 0, 0)) * glm::scale(glm::vec3(0.1f)),
		glm::vec3(0.5, 0.5, 0.5), lightVP, spotlightVP, 0.7, 0.0);

	drawObjectPBR(models::bedContext, glm::mat4(), glm::vec3(0.03f, 0.03f, 0.03f), lightVP, spotlightVP,0.2f, 0.0f);
	drawObjectPBR(models::chairContext, glm::mat4(), glm::vec3(0.195239f, 0.37728f, 0.8f), lightVP, spotlightVP, 0.4f, 0.0f);
	drawObjectPBR(models::chairTwoContext, glm::mat4(), glm::vec3(0.195239f, 0.37728f, 0.8f), lightVP, spotlightVP, 0.4f, 0.0f);
	drawObjectPBR(models::chairThreeContext, glm::mat4(), glm::vec3(0.195239f, 0.37728f, 0.8f), lightVP, spotlightVP, 0.4f, 0.0f);
	drawObjectPBR(models::deskContext, glm::mat4(), glm::vec3(0.428691f, 0.08022f, 0.036889f), lightVP, spotlightVP, 0.2f, 0.0f);
	drawObjectPBR(models::doorContext, glm::mat4(), glm::vec3(0.402978f, 0.120509f, 0.057729f), lightVP, spotlightVP, 0.2f, 0.0f);
	drawObjectPBR(models::drawerContext, glm::mat4(), glm::vec3(0.428691f, 0.08022f, 0.036889f), lightVP, spotlightVP, 0.2f, 0.0f);
	drawObjectPBR(models::marbleBustContext, glm::mat4(), glm::vec3(1.f, 1.f, 1.f), lightVP, spotlightVP, 0.5f, 1.0f);
	drawObjectPBR(models::materaceContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), lightVP, spotlightVP, 0.8f, 0.0f);
	drawObjectPBR(models::pillowContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), lightVP, spotlightVP, 0.8f, 0.0f);
	drawObjectPBR(models::pillowTwoContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), lightVP, spotlightVP, 0.8f, 0.0f);
	drawObjectPBR(models::pencilsContext, glm::mat4(), glm::vec3(0.10039f, 0.018356f, 0.001935f), lightVP, spotlightVP, 0.1f, 0.0f);
	drawObjectPBR(models::planeContext, glm::mat4(), glm::vec3(0.402978f, 0.120509f, 0.057729f), lightVP, spotlightVP, 0.2f, 0.0f);
	drawObjectPBR(models::roomContext, glm::mat4(), glm::vec3(0.9f, 0.9f, 0.9f), lightVP, spotlightVP, 0.8f, 0.0f);
	drawObjectPBR(models::windowContext, glm::mat4() , glm::vec3(0.402978f, 0.120509f, 0.057729f), lightVP, spotlightVP, 0.2f, 0.0f);
	drawObjectPBR(models::window2Context, glm::mat4(), glm::vec3(0.402978f, 0.120509f, 0.057729f), lightVP, spotlightVP, 0.2f, 0.0f);
	drawObjectPBR(models::tableContext, glm::mat4(), glm::vec3(0.428691f, 0.08022f, 0.036889f), lightVP, spotlightVP, 0.2f, 0.0f);
	drawObjectPBR(models::wardrobeContext, glm::mat4(), glm::vec3(0.428691f, 0.08022f, 0.036889f), lightVP, spotlightVP, 0.2f, 0.0f);
	drawObjectPBR(models::potContext, glm::mat4(), glm::vec3(0.10039f, 0.018356f, 0.001935f), lightVP, spotlightVP, 0.1f, 0.0f);

	if (lightSwitch)
	{
		drawObjectPBR(models::switchContext, glm::eulerAngleY(-3.14f / 2.f) * glm::translate(glm::vec3(0.f, 0.f, -3.0899f)), glm::vec3(1.f, 1.f, 1.f), lightVP, spotlightVP, 0.5f, 1.0f);
	}
	else
	{
		drawObjectPBR(models::switchContext, glm::eulerAngleZ(3.14f) * glm::eulerAngleY(3.14f/2.f) * glm::translate(glm::vec3(0.f, 0.f, -3.0899f)), glm::vec3(1.f, 1.f, 1.f), lightVP, spotlightVP, 0.5f, 1.0f);
	}

	glUseProgram(programTex);
	drawObjectPBRTexture(models::ballContext, glm::translate(glm::vec3(-0.46428f, -0.95f, ballMove + 3.3592f)) * glm::eulerAngleX(ballMove * 1.5f), texture::ball, normal::ball, lightVP, spotlightVP, 0.2f, 0.0f);
	
	glUseProgram(program);
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, spaceshipDir));
	specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-spaceshipDir.x,-spaceshipDir.y,-spaceshipDir.z,0,
		0.,0.,0.,1.,
		});


	//drawObjectColor(shipContext,
	//	glm::translate(cameraPos + 1.5 * cameraDir + cameraUp * -0.5f) * inveseCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()),
	//	glm::vec3(0.3, 0.3, 0.5)
	//	);
	drawObjectPBR(shipContext,
		glm::translate(spaceshipPos) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::vec3(0.03f)),
		glm::vec3(0.3, 0.3, 0.5),
		lightVP, spotlightVP, 0.2,1.0
	);

	spotlightPos = spaceshipPos + 0.2 * spaceshipDir;
	spotlightConeDir = spaceshipDir;



	//test depth buffer
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glUseProgram(programTest);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, spotlightDepthMap);
		//Core::DrawContext(models::testContext);

	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
	WIDTH = width;
	HEIGHT = height;
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

void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_9_1.vert", "shaders/shader_9_1.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_9_1_texture.vert", "shaders/shader_9_1_texture.frag");
	programTest = shaderLoader.CreateProgram("shaders/test.vert", "shaders/test.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_8_sun.vert", "shaders/shader_8_sun.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
	programDepth = shaderLoader.CreateProgram("shaders/shader_shadow.vert", "shaders/shader_shadow.frag");

	loadModelToContext("./models/obiekty/bed/bed.obj", models::bedContext);
	loadModelToContext("./models/obiekty/chair/chair.obj", models::chairContext);
	loadModelToContext("./models/obiekty/chair/chairTwo.obj", models::chairTwoContext);
	loadModelToContext("./models/obiekty/chair/chairThree.obj", models::chairThreeContext);
	loadModelToContext("./models/obiekty/desk/desk.obj", models::deskContext);
	loadModelToContext("./models/obiekty/door/door.obj", models::doorContext);
	loadModelToContext("./models/obiekty/window/window.obj", models::windowContext);
	loadModelToContext("./models/obiekty/window2/window.obj", models::window2Context);
	loadModelToContext("./models/obiekty/room/room.obj", models::roomContext);
	loadModelToContext("./models/obiekty/floor/floor.obj", models::planeContext);
	loadModelToContext("./models/obiekty/table/table.obj", models::tableContext);
	loadModelToContext("./models/obiekty/wardrobe/wardrobe.obj", models::wardrobeContext);
	loadModelToContext("./models/obiekty/pot/pot.obj", models::potContext);
	loadModelToContext("./models/obiekty/ball/ball.obj", models::ballContext);
	loadModelToContext("./models/obiekty/pillow/pillow.obj", models::pillowContext);
	loadModelToContext("./models/obiekty/pillow/pillowTwo.obj", models::pillowTwoContext);
	loadModelToContext("./models/obiekty/switch/switch.obj", models::switchContext);

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/spaceship.obj", shipContext);
	//loadModelToContext("./models/drawer.obj", models::drawerContext);
	loadModelToContext("./models/marbleBust.obj", models::marbleBustContext);
	loadModelToContext("./models/materace.obj", models::materaceContext);
	//loadModelToContext("./models/pencils.obj", models::pencilsContext);
	loadModelToContext("./models/spaceship.obj", models::spaceshipContext);
	loadModelToContext("./models/sphere.obj", models::sphereContext);
	loadModelToContext("./models/test.obj", models::testContext);
	loadModelToContext("./models/cube.obj", models::cubeContext);
	loadCubemap(faces);

	texture::ball = Core::LoadTexture("./models/obiekty/ball/ball_diffues.jpg");
	texture::door = Core::LoadTexture("./models/obiekty/door/door.jpg");

	normal::ball = Core::LoadTexture("./models/obiekty/ball/ball_diffues.jpg");
	normal::door = Core::LoadTexture("./models/obiekty/door/door.jpg");

	initDepthMap();
	initDepthMapSpotlight();
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(spaceshipDir, glm::vec3(0.f,1.f,0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = 0.05f * deltaTime * 60;
	float moveSpeed = 0.05f * deltaTime * 60;
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

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && ballMove < 0.01f)
		ballMove = ballMove + 0.05;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS && ballMove > -7.f)
		ballMove = ballMove - 0.05;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		lightSwitch = !lightSwitch;

	cameraPos = spaceshipPos - 0.5 * spaceshipDir + glm::vec3(0, 1, 0) * 0.2f;
	cameraDir = spaceshipDir;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.025;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.025;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		printf("spaceshipPos = glm::vec3(%ff, %ff, %ff);\n", spaceshipPos.x, spaceshipPos.y, spaceshipPos.z);
		printf("spaceshipDir = glm::vec3(%ff, %ff, %ff);\n", spaceshipDir.x, spaceshipDir.y, spaceshipDir.z);
	}

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