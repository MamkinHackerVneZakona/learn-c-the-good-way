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

Vec4f colorPickerForBlaze(int index, float t){
    srand(startupTimeEpoch + index);
    return {(float)(200 + rand() % 56) / 255.0f, (float)(230 + rand() % 26) / 255.0f, (float)(0 + rand() % 150) / 255.0f, 1};
}




struct BlazeList{
    double blazeTime = -10;
    Vec2f blazePos;
    BlazeList *next = 0;
};

struct BubbleList{
    double bubbleTime = -10;
    Vec2f bubblePos;
    BubbleList *next = 0;
};

struct LineList{
    Line2 line;
    LineList *next = 0;
};


BlazeList *blazes = 0;
BubbleList *bubbles = 0;
LineList *lines = 0;

bool creatingLine = false;
Line2 lineInProcess;

bool creatingBall = false;
bool ballCreated = false;
Line2 ballInProcess;
Vec2f ballPos;

double lastBubbleTime = -1;
double lastTime = -1;
double startTime = -1;


void integrateBall(float dt){
    float p32 = 32.0f/SCR_WIDTH;

    if(ballCreated){

        Line2 nearestLine;
        float minT = 100000; //TODO use max float val
        Vec2f normal;

        for(LineList *line = lines; line != 0;){

            Vec2f d = sub(line->line.end, line->line.start);
            float m = mag(d);
            Vec2f sn = mult({-d.y, d.x}, 1.0f/m);
            float sdist = dot(sub(ballPos, line->line.start), sn);
            float dist;
            Vec2f n;
            if(sdist < 0){
                dist = -sdist;
                n = mult(sn, -1);
            }else{
                dist = sdist;
                n = sn;
            }

            if(dot(n, sub(ballInProcess.end, ballInProcess.start)) < 0){
                dist = dot(sub(sub(ballPos, mult(n, p32)), line->line.start), n);

                float t = dist / (float) abs(dot(n, sub(ballInProcess.end, ballInProcess.start)));

                if(t > dt/1000 && t < minT){
                    minT = t;
                    nearestLine = line->line;
                    normal = n;
                }
            }

            line = line->next;

        }

        //printf("minT %f", minT);

        if(dt < minT){
            ballPos = add(ballPos, mult(sub(ballInProcess.end, ballInProcess.start), dt));
        }else{
            ballPos = add(ballPos, mult(sub(ballInProcess.end, ballInProcess.start), minT));
            Vec2f refl = sub(sub(ballInProcess.end, ballInProcess.start), mult(normal, 2 * dot(sub(ballInProcess.end, ballInProcess.start), normal)));
            ballInProcess.end = add(ballInProcess.start, refl);
            integrateBall(dt - minT);
        }
    }
}

void onUpdate(Renderer *r){
    
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 
    glEnable(GL_BLEND);
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
    Vec4f blueAlpha = {blue.x, blue.y, blue.z, 1.0};
    
    Vec2f origin = {0.0f, 0.0f};
    Vec2f i = {1.0f, 0.0f};
    Vec2f j = {0.0f, 1.0f};

    float p32 = 32.0f/SCR_WIDTH;
    float p16 = p32/2;
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
    
    for(BlazeList *blaze = blazes, *prev = 0; blaze != 0;){
        double blazeTime = blaze->blazeTime;
        Vec2f blazePos = blaze->blazePos;
        if(glfwGetTime() - blazeTime < 3){
            addBlaze(r, blazePos, p32 * 2, glfwGetTime() - blazeTime, 3, 15, -0.5f, colorPickerForBlaze);
            prev = blaze;
            blaze = blaze->next;
        }else{
            BlazeList *blazeNext = blaze->next; //free outlived blazes
            free(blaze);
            if(prev) prev->next = blazeNext;
            else if (blazeNext == 0){//removed the last one
                blazes = 0;
            }

            blaze = blazeNext;

        }
        
    }

    for(LineList *line = lines; line != 0;){
        addLine2PosColor(r, line->line, 0.1f, black, black);
        line = line->next;
    }

    if(ballCreated){
        addEllipse(r, ballPos, mult(i, p32), mult(j, p32), 15, 0.6, blueAlpha);
    }


    for(BubbleList *bubble = bubbles, *prev = 0; bubble != 0;){
        double bubbleTime = bubble->bubbleTime;
        Vec2f bubblePos = bubble->bubblePos;
        if(glfwGetTime() - bubbleTime < 0.5){
            addEllipse(r, bubblePos, mult(i, p16), mult(j, p16), 15, -0.5f, blueAlpha);
            prev = bubble;
            bubble = bubble->next;
        }else if(glfwGetTime() - bubbleTime < 3){
            float t = (float)(glfwGetTime() - bubbleTime);
            addEllipse(r, add(bubblePos, {0, (t-0.5f) * (t-0.5f) * 0.5f}), mult(i, p16), mult(j, p16), 15, -0.5f, {0,0,1.0f, (3 - t)/2.5f});
            prev = bubble;
            bubble = bubble->next;
        
        }else{
            BubbleList *bubbleNext = bubble->next; //free outlived bubbles
            free(bubble);
            if(prev) prev->next = bubbleNext;
            else if(bubbleNext == 0){//removed the last one
                bubbles = 0;
            }

            bubble = bubbleNext;
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

    if(glfwGetTime() - lastBubbleTime > 0.1 && glfwGetKey(r->window, GLFW_KEY_E) == GLFW_PRESS){
        BubbleList *bubble = (BubbleList *) malloc(sizeof(BubbleList));
        bubble->bubbleTime = glfwGetTime();
        bubble->bubblePos = {mx, my};
        bubble->next = bubbles;
        bubbles = bubble;
        lastBubbleTime = glfwGetTime();
    }
    
    if (glfwGetKey(r->window, GLFW_KEY_V) == GLFW_PRESS)
    {
        BlazeList *blaze = (BlazeList *) malloc(sizeof(BlazeList));
        blaze->blazeTime = glfwGetTime();
        blaze->blazePos = {mx, my};
        blaze->next = blazes;
        blazes = blaze;
    }

    if (glfwGetMouseButton(r->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && creatingLine == false)
    {
        lineInProcess = {{mx, my}, origin};
        creatingLine = true;
    }

    if (glfwGetMouseButton(r->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && creatingLine == true)
    {
        lineInProcess = {lineInProcess.start, {mx, my}};
        creatingLine = false;

        LineList *list = (LineList *) malloc(sizeof(LineList));
        list->line = lineInProcess;
        list->next = lines;
        lines = list;
    }

    if (glfwGetMouseButton(r->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && creatingBall == false)
    {
        ballInProcess = {{mx, my}, origin};
        creatingBall = true;
        ballCreated = false;
    }

    if (glfwGetMouseButton(r->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && creatingBall == true)
    {
        ballInProcess = {ballInProcess.start, {mx, my}};
        creatingBall = false;
        ballCreated = true;
        ballPos = ballInProcess.start;
        
    }


    integrateBall(dt);

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

    for(BubbleList *bubble = bubbles; bubble != 0;){
        BubbleList *next = bubble->next;
        free(bubble);
        bubble = next;
    }

    for(LineList *line = lines; line != 0;){
        LineList *next = line->next;
        free(line);
        line = next;
    }

    return 0;
}

