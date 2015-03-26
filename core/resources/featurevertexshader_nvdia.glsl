attribute highp vec4 position;
attribute mediump vec3 normal;
uniform mat4 mvp;
uniform vec3 scalecenter;
uniform float scalefactor;
uniform float alpha;
attribute lowp vec4 vertexColor;
varying lowp vec4 fragmentColor;
void main() {
if ( scalefactor != 1) {
	 float x = scalecenter[0] + (position[0] - scalecenter[0]) * scalefactor;
	 float y = scalecenter[1] + (position[1] - scalecenter[1]) * scalefactor;
	 float z = position[2];
	position[0] = x;
	position[1] = y;
	position[2] = z;
	position[3] = 1;
 }
 vertexColor.a = alpha * vertexColor.a;
 gl_Position =  mvp * position;
 fragmentColor = vertexColor;
}