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
const float brightness = 0.5;
#define TEST 0
void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	vec2 uv = fragCoord;
	if(TEST == 0) {
		vec4 color = texture(iChannel0, uv) ;
		fragColor = vec4(vec3(color.b, color.b, color.b) * brightness, color.b) ;
	} else {
		vec4 color = texture(iChannel0, uv) ;
		float b = damping * color.b;
		fragColor = vec4(color.b, color.b, color.b, color.b * 0.9) * brightness;
	}
}

void main( void ) {
	vec4 color = vec4(0.0,0.0,0.0,1.0);
	mainImage( color, gl_FragCoord.xy );
	oFragColor = color;
}
