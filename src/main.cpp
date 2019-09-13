#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <iostream>
#include <render.h>
#include "vec.h"
#include "util.h"
#include <math.h>
#include <time.h>

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

long startupTimeEpoch = time(NULL);

Vec3f colorPickerForBlaze(int index, float t){
    srand(startupTimeEpoch + index);
    return {(float)(200 + rand() % 56) / 255.0f, (float)(230 + rand() % 26) / 255.0f, (float)(0 + rand() % 150) / 255.0f};
}




struct BlazeList{
    double blazeTime = -10;
    Vec2f blazePos;
    BlazeList *next = 0;
};

BlazeList *blazes = 0;

double lastTime = -1;
double startTime = -1;
void onUpdate(Renderer *r){
    
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
    glDepthRange(1, 0);
    
    if(lastTime == -1){
         lastTime = glfwGetTime();
         startTime = lastTime;
    }
    clearAll(r);
    Vec3f black = {0, 0, 0};
    Vec3f red = {1, 0, 0};
    Vec3f green = {0, 1, 0};
    Vec3f blue = {0, 0, 1};
    Vec3f white = {1, 1, 1};
    
    Vec2f origin = {0.0f, 0.0f};
    Vec2f i = {1.0f, 0.0f};
    Vec2f j = {0.0f, 1.0f};

    float p32 = 32.0f/SCR_WIDTH;
    addLine2PosColor(r, { {-1,0}, {1, 0} }, 0.4, white, white);
    addLine2PosColor(r, { {0, -1}, {0, 1} }, 0.4, white, white);
    float ar = (float) SCR_WIDTH / SCR_HEIGHT;

    addText(r, "Y", -p32, 1 - p32 * ar, 0, p32, p32 * ar, black);
    addText(r, "X", 1 - p32, - p32 * ar, 0, p32, p32 * ar, black);
    float xMin = -4 * M_PI;
    float xMax = 1 * M_PI;
    float yMin = -4;
    float yMax = 4;
    addFunctionGraph(r, sin_, 100, -2 * M_PI, 2 * M_PI, xMin, xMax, yMin, yMax, 0, green);
    addFunctionGraph(r, parabola, 100, 1, 3, xMin, xMax, yMin, yMax,  0, red);
    addGraphGrid(r, xMin, xMax, yMin, yMax, 10, 10, p32, ar, 0.4, white);
    float gamma = 30.0f/180.0f*M_PI;
    
    //addLine2PosColor(r, { {0, 0}, {cos(gamma), sin(gamma) }}, -1, white, white);
    
    //addEllipse(r, origin, mult({cos(gamma), sin(gamma)}, p32 * 4), mult({-cos(gamma), sin(gamma)}, p32 * 2), 100, 0.0f, green);
    //addEllipse(r, {0.5f, 0}, mult({cos(gamma), sin(gamma)}, p32 * 4), mult({-cos(gamma), sin(gamma)}, p32 * 2), 100, 0.0f, black);

    float dt = glfwGetTime() - lastTime;
    float dtFull = glfwGetTime() - startTime;
    
    for(BlazeList *blaze = blazes; blaze != 0; blaze = blaze->next){
        double blazeTime = blaze->blazeTime;
        Vec2f blazePos = blaze->blazePos;
        if(glfwGetTime() - blazeTime < 3){
            addBlaze(r, blazePos, p32 * 2, glfwGetTime() - blazeTime, 3, 15, -0.5f, colorPickerForBlaze);
            blazeTime -= dt;
        }
    }

    if(glfwGetKey(r->window, GLFW_KEY_R) == GLFW_PRESS){
        sinScale += 0.5 * dt;
    }
    if(glfwGetKey(r->window, GLFW_KEY_T) == GLFW_PRESS){
        sinScale -= 0.5 * dt;
    }
    
    double xpos, ypos;
    glfwGetCursorPos(r->window, &xpos, &ypos);
    float mx = linearTransformNDC(xpos, 0, SCR_WIDTH);
    float my = linearTransform(ypos, 0, SCR_HEIGHT, 1, -1);
    
    int state = glfwGetMouseButton(r->window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        BlazeList *blaze = (BlazeList *) malloc(sizeof(BlazeList));
        blaze->blazeTime = glfwGetTime();
        blaze->blazePos = {mx, my};
        blaze->next = blazes;
        blazes = blaze;
    }

    lastTime = glfwGetTime();
}

int main()
{

    Renderer *r = allocRenderer(SCR_WIDTH, SCR_HEIGHT, "Rendering in OpenGL");
    setOnUpdate(r, onUpdate);
    glEnable(GL_DEPTH_TEST);
    loop(r);
    freeRenderer(r);
    
    for(BlazeList *blaze = blazes; blaze != 0;){
        BlazeList *next = blaze->next;
        free(blaze);
        blaze = next;
    }

    return 0;
}

