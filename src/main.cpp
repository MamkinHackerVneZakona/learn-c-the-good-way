#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <iostream>
#include <render.h>
#include "vec.h"
#include "util.h"
#include <math.h>


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float sinScale = 1;

float parabola(float x){
    return 2 * (x - 2)*(x - 2) - 1;
}

float sin_(float x){
    return sin(x) * sinScale;
}

double lastTime = -1;
void onUpdate(Renderer *r){
    if(lastTime == -1) lastTime = glfwGetTime();
    clearAll(r);
    Vec3f black = {0, 0, 0};
    Vec3f red = {1, 0, 0};
    Vec3f green = {0, 1, 0};
    Vec3f blue = {0, 0, 1};
    Vec3f white = {1, 1, 1};

    float p32 = 32.0f/SCR_WIDTH;
    addLine2PosColor(r, { {-1,0}, {1, 0} }, -1, white, white);
    addLine2PosColor(r, { {0, -1}, {0, 1} }, -1, white, white);
    float ar = (float) SCR_WIDTH / SCR_HEIGHT;

    addText(r, "Y", -p32, 1 - p32 * ar, 0, p32, p32 * ar, black);
    addText(r, "X", 1 - p32, - p32 * ar, 0, p32, p32 * ar, black);
    float xMin = -4 * M_PI;
    float xMax = 1 * M_PI;
    float yMin = -4;
    float yMax = 4;
    addFunctionGraph(r, sin_, 100, -2 * M_PI, 2 * M_PI, xMin, xMax, yMin, yMax, 0, green);
    addFunctionGraph(r, parabola, 100, 1, 3, xMin, xMax, yMin, yMax,  0, red);
    addGraphGrid(r, xMin, xMax, yMin, yMax, 10, 10, p32, ar, -1, white);

    float dt = glfwGetTime() - lastTime;

    if(glfwGetKey(r->window, GLFW_KEY_R) == GLFW_PRESS){
        sinScale += 0.5 * dt;
    }
    if(glfwGetKey(r->window, GLFW_KEY_T) == GLFW_PRESS){
        sinScale -= 0.5 * dt;
    }

    lastTime = glfwGetTime();
}

int main()
{

    Renderer *r = allocRenderer(SCR_WIDTH, SCR_HEIGHT, "Rendering in OpenGL");
    setOnUpdate(r, onUpdate);
    loop(r);
    freeRenderer(r);

    return 0;
}

