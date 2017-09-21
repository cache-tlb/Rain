#version 330

uniform vec4 fill_color;

in vec2 st;

out vec4 outputF;

void main() 
{
	vec2 offset = st - vec2(0.5, 0.5);
	float d2 = (offset.x)*(offset.x) + (offset.y)*(offset.y);
	vec4 c;
	if (d2 > .25)
		c = vec4(0.,0.,0.,0.);
	else 
		c = fill_color;

	outputF = c;
}