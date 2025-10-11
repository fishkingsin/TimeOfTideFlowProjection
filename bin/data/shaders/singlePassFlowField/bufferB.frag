OF_GLSL_SHADER_HEADER
uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform float     iFrameRate;            // shader frame rate
uniform int       iFrame;                // shader playback frame
uniform sampler2DRect iChannel0;
uniform vec4 iMouse;
out vec4 oFragColor;
const float damping = 0.996;
uniform float brightness;
#define TEST 0
void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	vec2 uv = fragCoord;
	
	vec4 color = texture(iChannel0, uv) ;
	fragColor = vec4(vec3(color.b, color.b, color.b) * brightness, color.b) ;
}

void main( void ) {
	vec4 color = vec4(0.0,0.0,0.0,1.0);
	mainImage( color, gl_FragCoord.xy );
	oFragColor = color;
}
