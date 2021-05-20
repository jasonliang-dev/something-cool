#version 330 core

in vec2 v_TexCoord;

out vec4 f_Color;

uniform sampler2D u_Texture;

void main()
{
    f_Color = texture(u_Texture, v_TexCoord);
}
