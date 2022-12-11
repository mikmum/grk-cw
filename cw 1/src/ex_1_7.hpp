#pragma once
#include "glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <vector>

#include "Shader_Loader.h"
#include "Render_Utils.h"

GLuint program; // Shader ID

GLuint quadVAO;

Core::Shader_Loader shaderLoader;

std::vector<glm::vec3> quadsPositions;

glm::vec3 quadPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 rotationPos = glm::vec3(0.0f, 0.0f, 1.0f);
float degree = 0.0f;

void renderScene(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	float time = glfwGetTime();

    glUseProgram(program);
    
    //glm::mat4 translation = glm::translate(glm::vec3(0.0f, sin(time) / 2, 0.0f)); //zad 1.7
    //glm::mat4 rotation = glm::rotate(time, glm::vec3(0.0f, 0.0f, 1.0f));


    glm::mat4 translation = glm::translate(quadPos);
    glm::mat4 rotation = glm::rotate(glm::radians( degree/10 ), rotationPos);
    glm::mat4 transformation = translation*rotation; 

    glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);

    Core::drawVAOIndexed(quadVAO, 6);

    for (unsigned i = 0; i < quadsPositions.size(); i++)
    {
        glm::mat4 translation = glm::translate( quadsPositions.at(i) );

        glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&translation);

        Core::drawVAOIndexed(quadVAO, 6);
    }

    glUseProgram(0);

    glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    //quadPos.x = ( 2 * xpos ) / 500 - 1;
    //quadPos.y = -(( 2 * ypos ) / 500 - 1);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    printf("%f,%f\n", xpos, ypos);

    if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
    {
        glm::vec3 newVector = glm::vec3((2 * xpos) / 500 - 1, -((2 * ypos) / 500 - 1), 1.0f);

        quadsPositions.insert( quadsPositions.begin() , newVector);
    }
}


void init(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    program = shaderLoader.CreateProgram("shaders/shader_1_2.vert", "shaders/shader_1_2.frag");

    //Przekopiuj kod do ladowania z poprzedniego zadania
    float points[] = { -0.1,-0.1,0.,1.,
            -0.1,0.1,0.,1.,
             0.1,0.1,0.,1.,
             0.1,-0.1,0.,1., };
    unsigned int indexes[] = { 0,1,3,
                              1,2,3 };
    quadVAO = Core::initVAOIndexed(points, indexes, 4, 4, 6);

    //Przekopiuj kod do ladowania z poprzedniego zadania
}

void shutdown(GLFWwindow* window)
{
    shaderLoader.DeleteProgram(program);
}

//obsluga wejscia
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        quadPos.y = quadPos.y + 0.1;
        //quadPos = glm::vec3(quadPos.x, quadPos.y + 0.1, quadPos.z);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        quadPos.y = quadPos.y - 0.1;
        //quadPos = glm::vec3(quadPos.x, quadPos.y - 0.1, quadPos.z);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        quadPos.x = quadPos.x + 0.1;
        //quadPos = glm::vec3(quadPos.x + 0.1, quadPos.y, quadPos.z);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        quadPos.x = quadPos.x - 0.1;
        //quadPos = glm::vec3(quadPos.x - 0.1, quadPos.y, quadPos.z);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        degree = degree - 30.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        degree = degree + 30.0f;
    }
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