//
// Created by Ruslan Feizerakhmanov on 2019-07-23.
//

#ifndef LEARN_C_THE_GOOD_WAY_RENDER_H
#define LEARN_C_THE_GOOD_WAY_RENDER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "util.h"
#include "vec.h"
#include "Line.h"
#include <float.h>

#include "renderImpl.h"


//==============================================
//==========ACTUAL CALLABLE THINGS==============
//==============================================

struct Renderer {
    __RenderSimple *renderSimpleLines;
    __RenderSimple *renderSimpleTriangles;
    __RenderText *renderText;
    GLuint programPosColor;
    GLuint programText;
    GLFWwindow *window;

    void (*onUpdate)(Renderer *);

    FT_Library ft;
    FT_Face face;
};


Renderer *allocRenderer(int windowWidth, int windowHeight, const char *windowName);

void clearAll(Renderer *renderer);

void addLine2PosColor(Renderer *renderer, Line2 line, float zLevel, Vec3f colorStart, Vec3f colorEnd);
void addTriangle2PosColor(Renderer *r, Vec2f *vertices, float zLevel, Vec3f *colors);
void addText(Renderer *r, const char *text, float x, float y, float z, float sx, float sy, Vec3f color);

//f : [min, max] -> [-1, 1]
//f(x) = k * x + b
float linearTransformNDC(float x, float min, float max);
float linearTransform(float x, float min, float max, float minPrime, float maxPrime);

void addFunctionGraph(Renderer *renderer, float *samples, int sampleCount, float sampleXMin, float sampleXMax, float gxMin, float gxMax, float gyMin, float gyMax, float zLevel, Vec3f color);

void addGraphGrid(Renderer *renderer, float xmin, float xmax, float ymin, float ymax, int xCount, int yCount, float sizeXNDC, float aspect, float zLevel, Vec3f color);

void addFunctionGraph(Renderer *renderer, float (*f)(float), int sampleCount, float sampleXMin, float sampleXMax, float gxMin, float gxMax, float gyMin, float gyMax, float zLevel, Vec3f color);

void addFunctionGraph(Renderer *renderer, float (*f)(float), int sampleCount, float sampleXMin, float sampleXMax, float zLevel, Vec3f color);

void addEllipse(Renderer *renderer, Vec2f center, Vec2f right, Vec2f up, int points, float zLevel, Vec4f color);
void addBlaze(Renderer *renderer, Vec2f center, float size, float t, float tMax, int particleCount, float zLevel, Vec4f (*getColor)(int, float) );

void loop(Renderer *renderer);

void setOnUpdate(Renderer *r, void (*f)(Renderer*));

void freeRenderer(Renderer *renderer);

#endif //LEARN_C_THE_GOOD_WAY_RENDER_H
