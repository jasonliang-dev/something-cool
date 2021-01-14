#version 330 core

in vec2 texCoords;
out vec4 color;

uniform sampler2D bitmap;
uniform vec4 source;

void main()
{
    color = vec4(1, 1, 1, texture(bitmap, texCoords * source.zw + source.xy).r);
}
