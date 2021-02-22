#version 330

in vec2 v_TexCoord;
out vec4 color;

uniform sampler2D u_Image;

void main()
{
    color = texture(u_Image, v_TexCoord);
}
