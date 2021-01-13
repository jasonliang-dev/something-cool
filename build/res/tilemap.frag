#version 330 core

in vec2 texCoords;
in vec2 tileCoords;
out vec4 color;

uniform sampler2D atlas;
uniform vec2 atlasSize;

void main()
{
    vec2 uv = tileCoords / atlasSize;
    vec2 offset = texCoords / atlasSize;
    color = texture(atlas, uv + offset);
}
