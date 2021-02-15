#version 330 core

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 tileData;

out vec2 texCoords;
out vec2 tileCoords;

uniform mat4 model;
uniform mat4 projection;
uniform ivec2 mapSize;

void main()
{
    texCoords = vertex.zw;
    tileCoords = tileData;
    ivec2 tileOffset = ivec2(gl_InstanceID % mapSize.x, gl_InstanceID / mapSize.x);
    gl_Position = projection * model * vec4(vertex.xy + (1.0 * tileOffset), 0.0, 1.0);
}
