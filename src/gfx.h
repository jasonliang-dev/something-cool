#pragma once

#include "gl.h"
#include "language.h"

typedef GLuint VAO;

VAO VAOCreate(void); // vao bound on create
void VAODestory(VAO vao);
void VAOBind(VAO vao);

typedef struct VBO VBO;
struct VBO
{
    GLenum target;
    GLuint handle;
};

VBO VBOCreate(GLenum target, size_t n, const void *data, GLenum usage); // vbo bound on create
void VBODestroy(VBO vbo);
void VBOBind(VBO vbo);

// only GL_FLOAT attribs
void VertexAttrBegin(GLsizei stride);
void VertexAttrPush(i32 size);
