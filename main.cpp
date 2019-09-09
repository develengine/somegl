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
#include "shaders.hpp"
#include "vaos.hpp"
#include "textures.hpp"

enum KeySigns {
    KEY_W,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_SPACE,
    KEY_SHIFT,
    KEY_COUNT
};

bool keyStates[KEY_COUNT];

void keyCallback(SDL_Event &event, bool down) {
    SDL_Scancode scancode = event.key.keysym.scancode;
    switch (scancode) {
        case SDL_SCANCODE_W:
            keyStates[KEY_W] = down;
            break;
        case SDL_SCANCODE_A:
            keyStates[KEY_A] = down;
            break;
        case SDL_SCANCODE_S:
            keyStates[KEY_S] = down;
            break;
        case SDL_SCANCODE_D:
            keyStates[KEY_D] = down;
            break;
        case SDL_SCANCODE_SPACE:
            keyStates[KEY_SPACE] = down;
            break;
        case SDL_SCANCODE_LSHIFT:
            keyStates[KEY_SHIFT] = down;
            break;
        default : return;
    }
}

int main(int argc, char *argv[]) {

    Application::init();
    Application::setKeyCallback(keyCallback);

    OBJMF::Model copeBlock = OBJMF::readObjFile("res/Dog.obj");

    const int SIZE = 10;
    const float DISTANCE = 3.0f;
    eng::Mat4f instanceInfo[SIZE * SIZE * SIZE];
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            for (int z = 0; z < SIZE; z++) {
                eng::Mat4f &mat = instanceInfo[z * SIZE * SIZE + y * SIZE + x];
                mat = eng::Mat4f::translation((float)x * DISTANCE, (float)y * DISTANCE, (float)z * DISTANCE);
            }
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
//     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    VAO vao(true);

    vao.add(makeVTNBuffer(0, copeBlock.data, copeBlock.dataSize));
    vao.add(makeMatrixBuffer(3, (float*)instanceInfo, SIZE * SIZE * SIZE, GL_STATIC_DRAW, 1));
    vao.addIndexBuffer(makeIndexBuffer(copeBlock.indices, copeBlock.indexCount));
    
    VAO::bindNone();

    copeBlock.free();

    int width, height, channelCount;
    stbi_set_flip_vertically_on_load(true);
    uint8_t *data = stbi_load("res/Dog_diffuse.jpg", &width, &height, &channelCount, STBI_rgb_alpha);

    if (!data) {
        std::cerr << "Failed to load image\n";
        exit(-1);
    }

    TexParameters parameters = {
        data,
        width, height,
        GL_REPEAT, GL_REPEAT,
        GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,
        GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE
    };

    Texture texture(parameters);

    GraphicShader shader(
        GraphicShader::load("res/shaders/shader.vert"),
        GraphicShader::load("res/shaders/shader.frag")
    );
    shader.use();

    int mvpMatrix = shader.getUniform("u_mvpMat");

    texture.bind(0, true);
    glUniform1i(shader.getUniform("textureSampler"), 0);

    vao.bind();

    eng::Vec3f cameraPos(0.f, 0.f, 3.f);
    eng::Vec2f cameraRot(0.f, 0.f);

    Application::setMouseMotionCallback([&](SDL_Event &event) {
        cameraRot[0] += (float)(event.motion.yrel) / 256.f;
        cameraRot[1] += (float)(event.motion.xrel) / 256.f;
    });

    while (Application::running) {
        Application::pollEvents();

        const float SPEED = 0.5;
        float cosVel = cos(cameraRot[1]) * SPEED;
        float sinVel = sin(cameraRot[1]) * SPEED;

        if (keyStates[KEY_W]) {
            cameraPos[2] -= cosVel;
            cameraPos[0] += sinVel;
        }
        if (keyStates[KEY_A]) {
            cameraPos[2] -= sinVel;
            cameraPos[0] -= cosVel;
        }
        if (keyStates[KEY_S]) {
            cameraPos[2] += cosVel;
            cameraPos[0] -= sinVel;
        }
        if (keyStates[KEY_D]) {
            cameraPos[2] += sinVel;
            cameraPos[0] += cosVel;
        }
        if (keyStates[KEY_SPACE]) {
            cameraPos[1] += SPEED;
        }
        if (keyStates[KEY_SHIFT]) {
            cameraPos[1] -= SPEED;
        }

        int wWidth, wHeight;
        SDL_GetWindowSize(Application::window, &wWidth, &wHeight);

        eng::Mat4f projMat = eng::Mat4f::GL_Projection(90.f, wWidth, wHeight, 0.1f, 100.f);
        eng::Mat4f viewMat = eng::Mat4f::xRotation(cameraRot[0])
            * eng::Mat4f::yRotation(cameraRot[1])
            * eng::Mat4f::translation(-cameraPos[0], -cameraPos[1], -cameraPos[2]);

        eng::Mat4f mvpMat = projMat * viewMat;

        glClearColor(0.35f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(mvpMatrix, 1, false, mvpMat[0]);

        vao.bindIndexBuffer();
        glDrawElementsInstanced(GL_TRIANGLES, copeBlock.indexCount, GL_UNSIGNED_INT, (void*)(0), SIZE * SIZE * SIZE);

        SDL_GL_SwapWindow(Application::window);
    }

    vao.free();
    shader.free();
    texture.free();

    Application::close();

    return 0;
}
