varying highp vec4 texc;
uniform sampler2D tex;

void main( void )
{
    highp vec4 color = texture2D( tex, texc.st ).rgba;
    gl_FragColor = color;
}