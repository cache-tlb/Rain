#version 330

uniform sampler2D tex;
uniform int type;
uniform int flip_y;

in vec2 st;

out vec4 outputF;

void main() 
{
	vec2 new_st = st;
	if (flip_y > 0) {
		new_st.y = 1.0 - st.y;
	}
	vec4 c = texture2D(tex, new_st);
	if (type == 0) { 
		outputF = vec4(c.rgb, 1.0);
	} else {
		outputF = vec4(c.a, c.a, c.a, 1.0);
	}
}