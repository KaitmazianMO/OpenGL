#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
#include <iostream>
#include <exception>
#include <ctime>

#include "GLSLProgram.h"
 
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback (GLFWwindow* window, int width, int height);
void processInput (GLFWwindow* window);
GLFWwindow *initGLFWwindowAndGlad (
    int major_ver, int minor_ver, int profile, unsigned int width, 
    unsigned int height, const char *w_name, GLFWframebuffersizefun cbfun
);

float first_triangle[] = 
{                          
    -0.5f, -0.5f, 1.0f,               
     1.0f,  0.0f, 0.0f,  // color
     0.5f, -0.5f, 0.0f,                        
     0.0f,  1.0f, 0.0f,  // color
    -0.5f,  0.5f, 1.0f,                                 
     0.0f,  0.0f, 1.0f,  // color
};

float second_triangle[] =
{                                        
    -0.5f,  0.5f, 1.0f,  // vert                        
     0.0f,  0.0f, 1.0f,  // color
     0.5f,  0.5f, 0.0f,  // vert                             
     1.0f,  0.0f, 0.0f,  // color
     0.5f, -0.5f, 1.0f,  // vert               
     0.0f,  1.0f, 0.0f,  // color
 };


int main()
{
    GLFWwindow *window = initGLFWwindowAndGlad (
        4, 6, GLFW_OPENGL_CORE_PROFILE, SCR_WIDTH, SCR_HEIGHT, 
        "Huge tiangles", framebuffer_size_callback
    );

    GLSLProgram shaderProgram;
    try {                                                 
        shaderProgram.compileShader ("../res/Shaders/triangle.vert", GL_VERTEX_SHADER);                    
        shaderProgram.compileShader ("../res/Shaders/triangle.frag", GL_FRAGMENT_SHADER);
        shaderProgram.link();
        shaderProgram.use();
    } catch (GLSLProgramException &ex) {
        std::cout << ex.what() << std::endl;
    }

    unsigned int VBO[2] = {}; 
    unsigned int VAO[2] = {};
    glGenVertexArrays (2, VAO);
    glGenBuffers      (2, VBO);
 
    glBindVertexArray (VAO[0]);
    glBindBuffer (GL_ARRAY_BUFFER, VAO[0]);
    glBufferData (GL_ARRAY_BUFFER, sizeof(first_triangle), first_triangle, GL_STATIC_DRAW);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray (0);
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof (float)));
    glEnableVertexAttribArray (1);

    glBindVertexArray (VAO[1]);
    glBindBuffer (GL_ARRAY_BUFFER, VAO[1]);
    glBufferData (GL_ARRAY_BUFFER, sizeof(second_triangle), second_triangle, GL_STATIC_DRAW);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 2 * 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray (0);                                                                     
    glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof (float)));
    glEnableVertexAttribArray (1);

    //glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
 
        glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
        glClear (GL_COLOR_BUFFER_BIT);

        //glUseProgram (firstShaderProgram);
        glBindVertexArray (VAO[0]); 
        glDrawArrays (GL_TRIANGLES, 0, 3);

        //glUseProgram (secondShaderProgram);
        glBindVertexArray (VAO[1]); 
        glDrawArrays (GL_TRIANGLES, 0, 3);
       
        glfwSwapBuffers (window);
        glfwPollEvents();
    }
 
    glDeleteVertexArrays (2, VAO);
    glDeleteBuffers (2, VBO);
 
    glfwTerminate();
    return 0;
}
 

GLFWwindow *initGLFWwindowAndGlad (int major_ver, int minor_ver, int profile, unsigned int width,
    unsigned int height, const char *w_name, GLFWframebuffersizefun cbfun)
{
    glfwInit();
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, major_ver);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, minor_ver);
    glfwWindowHint (GLFW_OPENGL_PROFILE, profile);
 
    GLFWwindow* window = glfwCreateWindow (width, height, w_name, NULL, NULL);
    if (window == NULL)
        throw std::exception ("Cant't create a window");

    glfwMakeContextCurrent (window);
    glfwSetFramebufferSizeCallback (window, cbfun);
 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::exception ("Can't init glad");

    return  window;
}

int getCompiledShader (int type, const char **source, int *success)
{
    int shader = glCreateShader (type);
    glShaderSource (shader, 1, source, NULL);
    glCompileShader (shader);

    glGetShaderiv (shader, GL_COMPILE_STATUS, success);

    return shader;
}

int getLinkedVertexAndFragmentShaers (int vert_shader, int frag_shader, int *success)
{
    int shader_program = glCreateProgram();
    glAttachShader (shader_program, vert_shader);
    glAttachShader (shader_program, frag_shader);
    glLinkProgram  (shader_program);
 
    glGetProgramiv (shader_program, GL_LINK_STATUS, success);

    return shader_program;
}

void shaderNotSuccesErrorOut (int shader, int succes, const char *msg)
{
    static char info_log[512] = {};
    if (!succes)
    { 
        glGetShaderInfoLog (shader, 512, NULL, info_log);
        std::cout << msg << info_log << std::endl;
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
 
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}