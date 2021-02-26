#version 330

in vec2 v_TexCoord;
out vec4 color;

uniform vec4 u_DrawColor;

void main()
{
    color = u_DrawColor;
}
