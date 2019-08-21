//
// Created by Ruslan Feizerakhmanov on 2019-07-23.
//

#ifndef LEARN_C_THE_GOOD_WAY_RENDERIMPL_H
#define LEARN_C_THE_GOOD_WAY_RENDERIMPL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "util.h"
#include "vec.h"
#include "Line.h"
#include <float.h>

const int RENDER_TYPE_TRIANGLES = GL_TRIANGLES;
const int RENDER_TYPE_LINES = GL_LINES;
const int VERTEX_SIZE_POS_COLOR = 6;
const int VERTEX_SIZE_POS_TEXTURE = 5;

struct __RenderSimple{
    int renderType;
    int vertexSize;
    float *vertexData;
    int *indexData;
    int numOfVertices;
    int numOfIndices;

    int vertexDataRealSize; //in floats
    int indexDataRealSize; //in ints

    bool transferred = false;


    GLuint VBO;
    GLuint VAO;
    GLuint EBO;
    GLuint program; //not managed by this struct

};

struct __RenderText{
    GLuint *textures;
    Vec3f *colors;
    GLuint count;
    float *vertexData;

    uint texturesAllocated;

    bool transferred;

    GLuint VBO;
    GLuint VAO;
    GLuint program;
    GLFWwindow* window;
};

void __ensureSizeText(__RenderText *info, int toBeAdded);

void __addText(__RenderText *info, FT_Face face, const char *text, float x, float y, float z, float sx, float sy, Vec3f color);

void __transferText(__RenderText *info);

void __ensureSizePosColor(__RenderSimple *info, int toBeAddedToVertexData, int toBeAddedToIndexData);

void __transferSimplePosColor(__RenderSimple *info);

void __drawText(__RenderText *info);

void __freeRenderText(__RenderText *info);

__RenderText *__allocRenderText(int program, GLFWwindow *window, int allocSpaceForThatMuchSymbols);

void __drawSimple(__RenderSimple *info);

//sizes must be ensured in advance
void __addPointColor(__RenderSimple *info, Vec3f p, Vec3f c);



void __addLine2PosColor(__RenderSimple *info, Line2 line, float zLevel, Vec3f colorStart, Vec3f colorEnd);
__RenderSimple *__allocRenderSimple(int program, int renderType, int vertexSize, int allocSpaceForThatMuchVertices,
                                    int allocSpaceForThatMuchIndices);


void __freeRenderSimple(__RenderSimple *info);

void __framebuffer_size_callback(GLFWwindow *window, int width, int height);

void __processInput(GLFWwindow *window);

#endif //LEARN_C_THE_GOOD_WAY_RENDERIMPL_H
