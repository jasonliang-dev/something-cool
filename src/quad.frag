#version 330 core

in vec2 texCoords;
out vec4 color;

uniform vec4 drawColor;

void main()
{
    color = drawColor;
}
