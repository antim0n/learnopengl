#include<iostream>
#include<glad/glad.h> // needed for GLFW include 0_0
#include<GLFW/glfw3.h>

// OpenGL Mathematics
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// simplest vertex shader, no transformation needed
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "uniform mat4 transform;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(aPos * vec3(1.0, -1.0, 1.0), 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(ourColor, 1.0f);\n"
    "}\0";

int main()
{
    // window setup
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // make context of window the main context on the current thread
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // vertex shader setup
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader); // compile dynamically at run-time from source code
    int  success;
    char infoLog[512];
    // checking for compile-time errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader setup
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // checking for compile-time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders to a shader program object
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // checking for compile-time errors
    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAMM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // don't need shader objects after linking anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // vertex data setup
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };

    // vertex array object - draw settings
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int VBO; // vertex buffer object
    glGenBuffers(1, &VBO); // generate one buffer, can store a large number of vertices in the GPU's memory
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // current buffer for GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // copy vertex data into the buffer's memory

    // define how to interpret the vertex data
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // render loop, 1 iteration is 1 frame
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state-setting
        glClear(GL_COLOR_BUFFER_BIT); // state-using

        // draw triangle
        glUseProgram(shaderProgram); // activate -> use for every shader and rendering call

        // update uniform color
        // float timeValue = glfwGetTime();
        // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor"); <- first define ourColor in the fragment shader
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // transformation matrix
        glm::mat4 trans = glm::mat4(1.0f);
        float transform = 0.1 * glfwGetTime();
        trans = glm::translate(trans, glm::vec3(transform, transform, 0.0f));

        int vertexTransformLocation = glGetUniformLocation(shaderProgram, "transform");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(vertexTransformLocation, 1, GL_FALSE, glm::value_ptr(trans));

        // render triangles
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0);

        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional de-allocation
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // terminate and clear recources allocated by glfw
    glfwTerminate();
    return 0;
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