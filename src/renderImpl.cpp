//
// Created by Ruslan Feizerakhmanov on 2019-07-23.
//

#include "renderImpl.h"

#include <string>
#include <codecvt>


void __ensureSizeText(__RenderText *info, int toBeAdded){
    if(info->count + toBeAdded > info->texturesAllocated){
        int newSize = __max__(info->texturesAllocated * 2, info->texturesAllocated + toBeAdded) * sizeof(GLuint);
        info->textures = (GLuint *) realloc((void *) info->textures, newSize);
        int newSizeVertexData = __max__(info->texturesAllocated * 4 * 5 * 2, (info->texturesAllocated + toBeAdded) * 4 * 5 ) * sizeof(float);
        info->vertexData = (float *) realloc((void *) info->vertexData, newSizeVertexData);
        int newSizeColors = __max__(info->texturesAllocated * 2, info->texturesAllocated + toBeAdded) * sizeof(Vec3f);
        info->colors = (Vec3f *) realloc((void *) info->colors, newSizeColors);

        info->texturesAllocated = newSize / sizeof(float);

    }

}

void __addText(__RenderText *info, FT_Face face, const char *text, float x, float y, float z, float sx, float sy, Vec3f color) {
    unsigned int lenRaw = strlen(text);
    __ensureSizeText(info, lenRaw);

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
    const std::wstring wideText = convert.from_bytes(text);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(wideText.size(), &info->textures[info->count]);

    FT_Select_Charmap(face , ft_encoding_unicode);


    const char *p;

    int ww;
    int wh;
    glfwGetWindowSize(info->window, &ww, &wh);
    //sx /= ww;
    //sy /= wh;

    //return std::u32string(reinterpret_cast<char32_t const *>(asInt.data()), asInt.length());


    for(int i = 0; i < wideText.size(); ++i) {
        //p = text + i;

        if(FT_Load_Char(face, wideText[i], FT_LOAD_RENDER)){
            printf("could not load char\n");
            exit(1);
        }

        FT_GlyphSlot g = face->glyph;


        glBindTexture(GL_TEXTURE_2D, info->textures[info->count + i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                g->bitmap.width,
                g->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                g->bitmap.buffer
        );

        float x2 = x + g->bitmap_left * sx /64;// + g->bitmap_left * sx;
        float y2 = -y - g->bitmap_top * sy /64;// - g->bitmap_top * sy;
        float w = sx * g->bitmap.width/64;//g->bitmap.width * sx;
        float h = sy * g->bitmap.rows/64;//g->bitmap.rows * sy;


        GLfloat box[4][VERTEX_SIZE_POS_TEXTURE] = {
                {x2,     -y2,  z, 0, 0},
                {x2 + w, -y2,  z, 1, 0},
                {x2,     -y2 - h, z, 0, 1},
                {x2 + w, -y2 - h, z, 1, 1},
        };


        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < VERTEX_SIZE_POS_TEXTURE; ++k) {
                info->vertexData[(4*(i + info->count) + j) * VERTEX_SIZE_POS_TEXTURE + k] = box[j][k];
            }
        }

        info->colors[info->count + i] = color;





        x += g->advance.x / 64 / 64.0f * sx;
        y += g->advance.y / 64 / 64.0f * sy;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    info->count += wideText.size();

}

void __transferText(__RenderText *info){
    if(info->transferred){
        fprintf(stderr, "trying to transfer data that has been previously transferred\n");exit(1);
    }

    glGenVertexArrays(1, &info->VAO);
    glGenBuffers(1, &info->VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(info->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, info->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_SIZE_POS_TEXTURE * 4 * info->count, info->vertexData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    info->transferred = true;
}

void __ensureSizePosColor(__RenderSimple *info, int toBeAddedToVertexData, int toBeAddedToIndexData){
    if(info->numOfVertices * info->vertexSize + toBeAddedToVertexData > info->vertexDataRealSize){
        int newSize = __max__(info->vertexDataRealSize * 1.5, info->vertexDataRealSize + toBeAddedToVertexData) * sizeof(float);
        info->vertexData = (float*) realloc((void *) info->vertexData, newSize);
        info->vertexDataRealSize = newSize / sizeof(float);
    }
    if(info->numOfIndices + toBeAddedToIndexData > info->indexDataRealSize){
        int newSize = __max__(info->indexDataRealSize * 1.5, info->indexDataRealSize + toBeAddedToIndexData) * sizeof(int);
        info->indexData = (int*) realloc((void *) info->indexData, newSize);
        info->indexDataRealSize = newSize / sizeof(int);
    }
}

void __transferSimplePosColor(__RenderSimple *info){
    if(info->transferred){
        fprintf(stderr, "trying to transfer data that has been previously transferred\n");exit(1);
    }

    glGenVertexArrays(1, &info->VAO);
    glGenBuffers(1, &info->VBO);
    glGenBuffers(1, &info->EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(info->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, info->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * info->numOfVertices * info->vertexSize, info->vertexData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * info->numOfIndices, info->indexData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE_POS_COLOR * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_SIZE_POS_COLOR * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    info->transferred = true;
}

void __drawText(__RenderText *info){
    if(!info->transferred){
        fprintf(stderr, "trying to draw data that has not yet been transferred\n");exit(1);
    }

    glEnable(GL_TEXTURE);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(info->VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glUniform1i(glGetUniformLocation(info->program, "tex"), 0);
    for (int i = 0; i < info->count; ++i) {
        glBindTexture(GL_TEXTURE_2D, info->textures[i]);
        glUniform4f(glGetUniformLocation(info->program, "color"), info->colors[i].x, info->colors[i].y, info->colors[i].z, 1);
        glDrawArrays(GL_TRIANGLE_STRIP, 4*i, 4);

    }
    glBindVertexArray(0);
}

void __freeRenderText(__RenderText *info){

    if(info->transferred){
        glDeleteVertexArrays(1, &(info->VAO));
        glDeleteBuffers(1, &(info->VBO));
        glDeleteTextures(info->count, info->textures);
    }

    free(info->vertexData);
    free(info->colors);
    free(info->textures);


    free(info);
}

__RenderText *__allocRenderText(int program, GLFWwindow *window, int allocSpaceForThatMuchSymbols){
    __RenderText *info = (__RenderText *) malloc(sizeof(__RenderText));
    info->vertexData = (float*) malloc(allocSpaceForThatMuchSymbols * 5 * 4 * sizeof(float));
    info->textures = (GLuint *) malloc(sizeof(GLuint) * allocSpaceForThatMuchSymbols);
    info->colors = (Vec3f *) malloc(sizeof(Vec3f) * allocSpaceForThatMuchSymbols);
    info->count = 0;
    info->texturesAllocated = allocSpaceForThatMuchSymbols;
    info->transferred = false;
    info->program = program;
    info->VAO = 0;
    info->VBO = 0;
    info->window = window;

    return info;
}

void __drawSimple(__RenderSimple *info){
    if(!info->transferred){
        fprintf(stderr, "trying to draw data that has not yet been transferred\n");exit(1);
    }

    glBindVertexArray(info->VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(info->renderType, info->numOfIndices, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

//sizes must be ensured in advance
void __addPointColor(__RenderSimple *info, Vec3f p, Vec3f c){
    info->vertexData[info->numOfVertices * info->vertexSize + 0] = p.x;
    info->vertexData[info->numOfVertices * info->vertexSize + 1] = p.y;
    info->vertexData[info->numOfVertices * info->vertexSize + 2] = p.z;
    info->vertexData[info->numOfVertices * info->vertexSize + 3] = c.x;
    info->vertexData[info->numOfVertices * info->vertexSize + 4] = c.y;
    info->vertexData[info->numOfVertices * info->vertexSize + 5] = c.z;
    info->vertexData[info->numOfVertices * info->vertexSize + 6] = 1.0f;

    info->indexData[info->numOfIndices] = info->numOfVertices;
    info->numOfVertices += 1;
    info->numOfIndices += 1;
}

void __addPointColor(__RenderSimple *info, Vec3f p, Vec4f c){
    info->vertexData[info->numOfVertices * info->vertexSize + 0] = p.x;
    info->vertexData[info->numOfVertices * info->vertexSize + 1] = p.y;
    info->vertexData[info->numOfVertices * info->vertexSize + 2] = p.z;
    info->vertexData[info->numOfVertices * info->vertexSize + 3] = c.x;
    info->vertexData[info->numOfVertices * info->vertexSize + 4] = c.y;
    info->vertexData[info->numOfVertices * info->vertexSize + 5] = c.z;
    info->vertexData[info->numOfVertices * info->vertexSize + 6] = c.w;

    info->indexData[info->numOfIndices] = info->numOfVertices;
    info->numOfVertices += 1;
    info->numOfIndices += 1;
}



void __addLine2PosColor(__RenderSimple *info, Line2 line, float zLevel, Vec3f colorStart, Vec3f colorEnd){
    if(info->renderType != GL_LINES || info->vertexSize != VERTEX_SIZE_POS_COLOR){
        fprintf(stderr, "trying to add LINE to incompatible RenderInfo\n");exit(1);
    }

    __ensureSizePosColor(info, VERTEX_SIZE_POS_COLOR * 2, 2);

    __addPointColor(info, {line.start.x, line.start.y, zLevel}, colorStart);
    __addPointColor(info, {line.end.x, line.end.y, zLevel}, colorEnd);
}

__RenderSimple *__allocRenderSimple(int program, int renderType, int vertexSize, int allocSpaceForThatMuchVertices,
                                    int allocSpaceForThatMuchIndices){
    __RenderSimple *info = (__RenderSimple *) malloc(sizeof(__RenderSimple));
    info->renderType = renderType;
    info->vertexSize = vertexSize;
    info->vertexData = (float*) malloc(allocSpaceForThatMuchVertices * vertexSize * sizeof(float));
    info->indexData = (int*) malloc(allocSpaceForThatMuchIndices * sizeof(int));
    info->numOfVertices = 0;
    info->numOfIndices = 0;
    info->vertexDataRealSize = allocSpaceForThatMuchVertices * vertexSize;
    info->indexDataRealSize = allocSpaceForThatMuchIndices;
    info->transferred = false;
    info->program = program;
    info->VAO = 0;
    info->EBO = 0;
    info->VBO = 0;

    return info;
}


void __freeRenderSimple(__RenderSimple *info){
    free(info->vertexData);
    free(info->indexData);

    if(info->transferred){
        glDeleteVertexArrays(1, &(info->VAO));
        glDeleteBuffers(1, &(info->VBO));
        glDeleteBuffers(1, &(info->EBO));
    }


    free(info);
}

void __framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void __processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
