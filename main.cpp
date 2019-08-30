#include "application.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stb_image.h>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include "math.hpp"
#include "objmf.hpp"

float vertices[] = {
     1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     1.0f,-1.0f, 0.0f,  1.0f, 1.0f,
    -1.0f,-1.0f, 0.0f,  0.0f, 1.0f,
    -1.0f, 1.0f, 0.0f,  0.0f, 0.0f
};

uint32_t indices[] = {
    0, 1, 2,
    2, 3, 0
};

std::string readFile(const std::string path) {
    std::ifstream file(path);
    std::string str;
    
    file.seekg(0, std::ios::end);   
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    
    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return str;
}

int main(int argc, char *argv[]) {

    Application::init();

    OBJMF::readObjFile("res/copeblock.obj");

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    uint32_t ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);

    int width, height, channelCount;
    uint8_t *data = stbi_load("res/kek.png", &width, &height, &channelCount, 0);

    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (!data) {
        std::cerr << "Failed to load image\n";
        exit(-1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    int succes;
    char infoLog[512];

    std::string vertexSourceString = readFile("res/shaders/shader.vert");
    std::string fragmentSourceString = readFile("res/shaders/shader.frag");
    const char *vertexSource = vertexSourceString.c_str();
    const char *fragmentSource = fragmentSourceString.c_str();

    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &succes);
    if (!succes) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Failed to compile vertex shader! Error:\n" << infoLog << '\n';
    }
    
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &succes);
    if (!succes) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Failed to compile vertex shader! Error:\n" << infoLog << '\n';
    }
    
    uint32_t shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &succes);
    if (!succes) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Failed to link shader program! Error:\n" << infoLog << '\n';
    }

    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgram, "textureSampler"), 0);

    glBindVertexArray(vao);

    while (Application::running) {
        Application::pollEvents();

        glClearColor(0.35f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(Application::window);
    }

    return 0;
}
