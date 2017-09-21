#version 330

uniform sampler2D tex;
uniform int tex_width;
uniform int tex_height;
uniform int x;
uniform int y;
uniform int w;
uniform int h;

in vec2 st;

out vec4 outputF;

void main() 
{
	vec2 ji = st*vec2(w,h) - vec2(0.5, 0.5);
	vec2 src_st = vec2((ji.x+0.5+x)/tex_width, (ji.y+0.5+y)/tex_height);
	outputF = texture2D(tex, src_st);
}