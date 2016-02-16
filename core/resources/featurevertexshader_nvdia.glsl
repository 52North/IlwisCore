attribute highp vec4 position;
attribute mediump vec3 normal;
uniform mat4 mvp;
uniform vec3 scalecenter;
uniform float scalefactor;
uniform float alpha;
attribute lowp vec4 vertexColor;
varying lowp vec4 fragmentColor;

void main() {
vec4 pos;
vec4 vColor = vertexColor;
pos = position;
if ( scalefactor != 1.0) {
	 float x = scalecenter[0] + (position[0] - scalecenter[0]) * scalefactor;
	 float y = scalecenter[1] + (position[1] - scalecenter[1]) * scalefactor;
	 float z = position[2];
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
        pos[3] = 1.0;
 }
 vColor.a = alpha * vertexColor.a;
 gl_Position =  mvp * pos;
 fragmentColor = vColor;
}
