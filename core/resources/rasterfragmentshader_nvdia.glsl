varying highp vec4 texc;
uniform sampler2D tex;
uniform float alpha;

void main( void )
{
    highp vec4 color = texture2D( tex, texc.st ).rgba;
	color.a = alpha * color.a;
    gl_FragColor = color;
}