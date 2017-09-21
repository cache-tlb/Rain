#version 330

out vec2 st;

in vec4 position;
in vec2 uv;

void main()
{
    gl_Position = position;
    st = uv;
} 
