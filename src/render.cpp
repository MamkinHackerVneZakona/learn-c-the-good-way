//
// Created by Ruslan Feizerakhmanov on 2019-07-23.
//

#include "render.h"
#include <cmath>

Renderer *allocRenderer(int windowWidth, int windowHeight, const char *windowName){

    FT_Library ft;

    if(FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetype library\n");
        return NULL;
    }

    FT_Face face;

    if(FT_New_Face(ft, "../assets/fonts/OpenSans-Regular.ttf", 0, &face)) {
        fprintf(stderr, "Could not open font\n");
        return NULL;
    }

    FT_Set_Pixel_Sizes(face, 0, 64);


    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, __framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return NULL;
    }


    glfwSwapInterval(1);


    GLuint programColor = loadProgram("../assets/shaders/color.vert", "../assets/shaders/color.frag");
    GLuint programText = loadProgram("../assets/shaders/text.vert", "../assets/shaders/text.frag");

    Renderer *r = (Renderer*) malloc(sizeof(Renderer));
    r->programPosColor = programColor;
    r->programText = programText;

    r->renderSimpleLines = __allocRenderSimple(programColor, GL_LINES, VERTEX_SIZE_POS_COLOR, 1000, 10000);
    r->renderSimpleTriangles = __allocRenderSimple(programColor, GL_TRIANGLES, VERTEX_SIZE_POS_COLOR, 1000, 10000);
    r->renderText = __allocRenderText(programText, window, 1000);
    r->ft = ft;
    r->face = face;
    r->window = window;
    r->onUpdate = NULL;

    return r;

}

void clearAll(Renderer *renderer){
    __freeRenderSimple(renderer->renderSimpleLines);
    __freeRenderSimple(renderer->renderSimpleTriangles);
    __freeRenderText(renderer->renderText);

    renderer->renderSimpleLines = __allocRenderSimple(renderer->programPosColor, GL_LINES, VERTEX_SIZE_POS_COLOR, 1000, 10000);
    renderer->renderSimpleTriangles = __allocRenderSimple(renderer->programPosColor, GL_TRIANGLES, VERTEX_SIZE_POS_COLOR, 1000, 10000);
    renderer->renderText = __allocRenderText(renderer->programText, renderer->window, 1000);
}

void addLine2PosColor(Renderer *renderer, Line2 line, float zLevel, Vec3f colorStart, Vec3f colorEnd){
    __addLine2PosColor(renderer->renderSimpleLines, line, zLevel, colorStart, colorEnd);
}

void addText(Renderer *r, const char *text, float x, float y, float z, float sx, float sy, Vec3f color) {
    __addText(r->renderText, r->face, text, x, y, z, sx, sy, color);
}

void addTriangle2PosColor(Renderer *r, Vec2f *vertices, float zLevel, Vec3f *colors){
    

    __ensureSizePosColor(r->renderSimpleTriangles, VERTEX_SIZE_POS_COLOR * 3, 3);
    
    for(int i = 0; i < 3; ++i){
        __addPointColor(r->renderSimpleTriangles, {vertices[i].x, vertices[i].y, zLevel}, colors[i]);
    }
}

void addTriangle2PosColor(Renderer *r, Vec2f *vertices, float zLevel, Vec4f *colors){
    

    __ensureSizePosColor(r->renderSimpleTriangles, VERTEX_SIZE_POS_COLOR * 3, 3);
    
    for(int i = 0; i < 3; ++i){
        __addPointColor(r->renderSimpleTriangles, {vertices[i].x, vertices[i].y, zLevel}, colors[i]);
    }
}

//f : [min, max] -> [-1, 1]
//f(x) = k * x + b
float linearTransformNDC(float x, float min, float max){
    float k = 2 / (max - min);
    float b = 1 - 2*max/(max - min);
    return k * x + b;
}

float linearTransform(float x, float min, float max, float minPrime, float maxPrime){
    float k = (maxPrime - minPrime) / (max - min);
    float b = maxPrime - k * max;
    return k * x + b;
}


void addFunctionGraph(Renderer *renderer, float *samples, int sampleCount, float sampleXMin, float sampleXMax, float gxMin, float gxMax, float gyMin, float gyMax, float zLevel, Vec3f color){
    for (int i = 0; i < sampleCount - 1; ++i) {
        float xPoint = sampleXMin + (sampleXMax - sampleXMin)/(sampleCount-1) * i;
        float xPointNext = sampleXMin + (sampleXMax - sampleXMin)/(sampleCount-1) * (i+1);
        float yVal = samples[i];
        float yValNext = samples[i + 1];

        addLine2PosColor(renderer, { {linearTransformNDC(xPoint, gxMin, gxMax), linearTransformNDC(yVal, gyMin, gyMax)}, {linearTransformNDC(xPointNext, gxMin, gxMax), linearTransformNDC(yValNext, gyMin, gyMax)} }, zLevel, color, color);

    }
}

void addGraphGrid(Renderer *renderer, float xmin, float xmax, float ymin, float ymax, int xCount, int yCount, float sizeXNDC, float aspect, float zLevel, Vec3f color){
    for (int i = 0; i < xCount; ++i) {
        float x = xmin + (xmax - xmin)/(xCount + 1) * (i + 1);
        float xNDC = linearTransformNDC(x, xmin, xmax);
        Line2 l = {  {xNDC, -sizeXNDC/3 * aspect / 2} , {xNDC, sizeXNDC * aspect / 3 / 2} };
        addLine2PosColor(renderer, l, zLevel, color, color);
        char str[100];
        sprintf(str, "%.3f", x);
        addText(renderer, str, xNDC - sizeXNDC / 2 * 2, -sizeXNDC * aspect, zLevel, sizeXNDC / 1.5, sizeXNDC * aspect / 1.5, {0,0,0});
    }

    for (int i = 0; i < yCount; ++i) {
        float y = ymin + (ymax - ymin)/(yCount + 1) * (i+1);
        float yNDC = linearTransformNDC(y, ymin, ymax);
        Line2 l = {  {-sizeXNDC/3 / 2, yNDC * aspect} , {sizeXNDC / 3 / 2, yNDC * aspect} };
        addLine2PosColor(renderer, l, zLevel, color, color);
        char str[100];
        sprintf(str, "%.3f", y);
        addText(renderer, str, sizeXNDC / 1.5 / 2, yNDC * aspect - sizeXNDC / 2 / 1.5 * aspect, zLevel, sizeXNDC / 1.5, sizeXNDC * aspect / 1.5, {0,0,0});
    }
}

void addFunctionGraph(Renderer *renderer, float (*f)(float), int sampleCount, float sampleXMin, float sampleXMax, float gxMin, float gxMax, float gyMin, float gyMax, float zLevel, Vec3f color){

    float *samples = (float *) malloc(sizeof(float) * sampleCount);

    for (int j = 0; j < sampleCount; ++j) {
        samples[j] = f(sampleXMin + (sampleXMax - sampleXMin)/(sampleCount-1) * j);
    }
    addFunctionGraph(renderer, samples, sampleCount, sampleXMin, sampleXMax, gxMin, gxMax, gyMin, gyMax, zLevel, color);

    free(samples);
}

void addFunctionGraph(Renderer *renderer, float (*f)(float), int sampleCount, float sampleXMin, float sampleXMax, float zLevel, Vec3f color){

    float yMax = FLT_MIN;
    float yMin = FLT_MAX;
    float *samples = (float *) malloc(sizeof(float) * sampleCount);

    for (int j = 0; j < sampleCount; ++j) {
        samples[j] = f(sampleXMin + (sampleXMax - sampleXMin)/(sampleCount-1) * j);
        if(samples[j] > yMax) yMax = samples[j];
        if(samples[j] < yMin) yMin = samples[j];
    }
    addFunctionGraph(renderer, samples, sampleCount, sampleXMin, sampleXMax, sampleXMin, sampleXMax, yMin, yMax, zLevel, color);

    free(samples);
}

void addEllipse(Renderer *renderer, Vec2f center, Vec2f right, Vec2f up, int points, float zLevel, Vec4f color){
    

    __ensureSizePosColor(renderer->renderSimpleTriangles, VERTEX_SIZE_POS_COLOR * 3 * points, 3 * points);
    
    float a = mag(right);
    float b = mag(up);
    
     Vec2f vi = mult(right, 1.0f/a);
     Vec2f vj = mult(up, 1.0f/b);
    
    for(int i = 0; i < points; ++i){
        float xPrime = a * cos((float)i * (2 * M_PI) / points);
        float yPrime = b * sin((float)i * (2 * M_PI) / points);
        Vec2f v = {vi.x * xPrime + vj.x * yPrime, vi.y * xPrime + vj.y * yPrime};
        
        float xPrimeNext = a * cos((float)(i+1) * (2 * M_PI) / points);
        float yPrimeNext = b * sin((float)(i+1) * (2 * M_PI) / points);
        Vec2f vNext = {vi.x * xPrimeNext + vj.x * yPrimeNext, vi.y * xPrimeNext + vj.y * yPrimeNext};
        
        Vec2f vertices[] = {add(center, {v.x, v.y}), add(center, {vNext.x, vNext.y}), {center.x, center.y}};
        Vec4f colors[] = {color, color, color};
        
        addTriangle2PosColor(renderer, vertices, zLevel, colors);
    }
}

void addBlaze(Renderer *renderer, Vec2f center, float size, float t, float tMax, int particleCount, float zLevel, Vec4f (*getColor)(int, float) ){
    
    Vec4f ellipseColor = {1.0f, 1.0f, 0.0f, 1};
    
    if(t < tMax/3){
        addEllipse(renderer, center, {size * t, 0}, {0, size * t}, 100, zLevel, ellipseColor);
    }else {
        if(t < 2*tMax/3){
            addEllipse(renderer, center, {size, 0}, {0, size}, 100, zLevel, ellipseColor);
        }else{
            addEllipse(renderer, center, {size * (tMax - t), 0}, {0, size * (tMax - t)}, 100, zLevel, ellipseColor);
        }
        
        t = t - tMax/3;
        float a = size / 8;
        float b = size / 32;
        float speed = 0.3;
        
        for(int i = 0; i < particleCount; ++i){
            float gamma = (float)i * (2 * M_PI) / particleCount;
            float vx = size * (-sin(gamma));
            float vy = size * cos(gamma);
            Vec2f v = {vx, vy};
            Vec2f vN = mult(v, 1.0f/mag(v));
            Vec2f qN = {-vN.y, vN.x};
            Vec2f p = {center.x + size * (float)cos(gamma) + vN.x * speed * t*t, center.y + size * (float)sin(gamma) + vN.y * speed * t * t};
            addEllipse(renderer, p, mult(vN, a), mult(qN, b), 100, zLevel, getColor(i, t));
        }
    }
    
    
}

void loop(Renderer *renderer){
    GLFWwindow *window = renderer->window;
    while (!glfwWindowShouldClose(window))
    {

        __processInput(window);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if(renderer->onUpdate){
            renderer->onUpdate(renderer);
        }

        glUseProgram(renderer->programPosColor);
        if(!renderer->renderSimpleLines->transferred) __transferSimplePosColor(renderer->renderSimpleLines);
        __drawSimple(renderer->renderSimpleLines);

        glUseProgram(renderer->programPosColor);
        if(!renderer->renderSimpleTriangles->transferred) __transferSimplePosColor(renderer->renderSimpleTriangles);
        __drawSimple(renderer->renderSimpleTriangles);

        glUseProgram(renderer->programText);
        if(!renderer->renderText->transferred) __transferText(renderer->renderText);
        __drawText(renderer->renderText);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}

void setOnUpdate(Renderer *r, void (*f)(Renderer*)){
    r->onUpdate = f;
}

void freeRenderer(Renderer *renderer){
    __freeRenderSimple(renderer->renderSimpleLines);
    __freeRenderSimple(renderer->renderSimpleTriangles);
    __freeRenderText(renderer->renderText);
    free(renderer);


    glfwTerminate();
}