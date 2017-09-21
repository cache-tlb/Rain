#version 330

uniform sampler2D src;
uniform sampler2D dst;
uniform int mode;
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
	vec2 ji = st*vec2(float(w),(h)) - vec2(0.5, 0.5);
	vec2 src_st = vec2((ji.x+0.5+x)/float(tex_width), (ji.y+0.5+y)/float(tex_height));
	vec4 src_pix = texture2D(src, src_st);
	vec4 dst_pix = texture2D(dst, src_st);
	vec4 c;
	if (mode == 0) {
		c = src_pix*src_pix[3] + dst_pix;//*(1.0-src_pix[3]);
		// c[3] = max(src_pix[3], dst_pix[3]);
		c[3] = src_pix[3] + dst_pix[3] - src_pix[3]*dst_pix[3];
	} else if (mode == 1) {
		c = src_pix;
		c = src_pix*dst_pix[3];
		c[3] = dst_pix[3];
	} else if (mode == 2) {
		vec4 c1 = -src_pix + vec4(1.,1.,1.,1.);
		vec4 c2 = -dst_pix + vec4(1.,1.,1.,1.);
		c = c1*c2;
		c = -c + vec4(1.,1.,1.,1.);
	} else if (mode == 3) {
		c = dst_pix*(1.-src_pix[3]);
		// c[3] = dst_pix[3];
		
	}
	outputF = c;
}