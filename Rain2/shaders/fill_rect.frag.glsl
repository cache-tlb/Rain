#version 330

uniform vec4 fill_color;

in vec2 st;

out vec4 outputF;

void main() 
{
	outputF = fill_color;
}