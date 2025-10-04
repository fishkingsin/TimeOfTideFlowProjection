OF_GLSL_SHADER_HEADER// 2023 myth0genesis

// It turns out that davidar's wind flow map effect https://www.shadertoy.com/view/4sKBz3
// only needs a single buffer for every stage of the simulation
// without any additional overhead, provided you're okay with sacrificing
// the ability to choose any color you want for the particles and tracers.
// I used a modified version of nimitz's FBM https://www.shadertoy.com/view/3l23Rh for the field,
// normalized from -1.0 to 1.0 and sans some of the frequency-wise scaling, to drive the particles' velocities,
// as it can give you a lot of variation with very few iterations, which enabled the ability
// to sample a larger area without a performance hit.
// I tried my best to regulate the speed (via frmAdj) in order to prevent pixels from traveling
// outside of the sample area or from moving too fast if the frame rate is either
// too low or too high.

// Click to push pixels outward.
uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform float     iFrameRate;            // shader frame rate
uniform int       iFrame;                // shader playback frame
uniform sampler2DRect iChannel0;
out vec4 oFragColor;


void mainImage(out vec4 fragColor, in vec2 fragCoord) {
	vec2 uv = fragCoord;
	fragColor = texture(iChannel0, uv);
}



void main( void ) {
	vec4 color = vec4(0.0,0.0,0.0,1.0);
	mainImage( color, gl_FragCoord.xy );
	oFragColor = color;
}
