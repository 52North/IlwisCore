attribute highp vec4 position;
attribute highp vec4 texCoord;
uniform mediump mat4 mvp;
uniform mediump mat4 texmat;
varying highp vec4 texc;

void main( void )
{
    gl_Position = mvp * position;
    texc = texmat * texCoord;
}
