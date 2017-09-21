#version 330

uniform sampler2D tex;
uniform int x_int;
uniform int y_int;
uniform int w_int;
uniform int h_int;
uniform float x_float;
uniform float y_float;
uniform float w_float;
uniform float h_float;
in vec2 st;

out vec4 outputF;

void main() 
{
	vec2 offset = vec2(x_float - float(x_int), y_float - float(y_int));
	vec2 dst_coord = st*vec2(float(w_int), float(h_int));	// in int range
	vec2 src_coord = dst_coord - offset;
	if (src_coord.x < 0 || src_coord.y < 0 || src_coord.x > w_float || src_coord.y > h_float) {
		outputF = vec4(0.0, 0.0, 0.0, 0.0);
	} else {
		vec2 src_st = vec2(src_coord.x / w_float, src_coord.y / h_float);
		outputF = texture2D(tex, src_st);
	}
}