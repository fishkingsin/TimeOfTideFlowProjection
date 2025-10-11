OF_GLSL_SHADER_HEADER
uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform float     iFrameRate;            // shader frame rate
uniform int       iFrame;                // shader playback frame
uniform sampler2DRect iChannel0;
uniform vec4 iMouse;
out vec4 oFragColor;
// 2023 myth0genesis

// Feel free to modify the number of iterations in the FBM function.
// ztot and iztot will compensate for it and normalize the result from -1.0 to 1.0.
const int MAX_ITERS = 8;
uniform int iters;

// Noise speed and position scale.
uniform float tScale;
uniform float scale;

// Particle velocity.
uniform float pVel;

// Tracer decay rate.
uniform float decay;

// Particle spawn rate.
uniform float spawnRate;

uniform int birthRate;

// Distance (in pixels) sampled around point.
const int MAX_SMP_DST = 4;
uniform int smpDst;

// The amount of velocity added by clicking.
uniform float impulse;

const uvec4 shft = uvec4(14U, 15U, 16U, 17U);
const float imf = 1.0 / float(0xFFFFFFFFU);

const mat3 m3 = mat3( 0.3338,  0.56034, -0.71817,
					 -0.87887, 0.32651, -0.15323,
					  0.15162, 0.69596,  0.61339) * 1.93;


float tm;
vec2 invRes;
float frmAdj;
#define MAX_POS 30

uniform vec4 positions[MAX_POS];


// My own 128-bit bijective hash https://www.shadertoy.com/view/mstXD2
vec4 bjhash128(vec4 p0) {
	uvec4 p = floatBitsToUint(p0);;
	p ^= p >> shft;
	p *= uvec4(0xEAF649A9U, 0x6AF649A9U, 0x050C2D35U, 0xAAF649A9U);
	p ^= p.yzwx ^ p.zwxy;
	p ^= p >> shft.yzwx;
	p *= uvec4(0x21F0AAADU, 0x0D0C2D35U, 0xE8F649A9U, 0xD30332D3U);
	p ^= p.yzwx ^ p.zwxy;
	return vec4(p ^ p >> shft.wxyz) * imf;
}

// nimitz's gyroid-based FBM technique https://www.shadertoy.com/view/3l23Rh
vec2 gFBM(vec3 p0) {
	vec3 p1 = p0 + vec3(1385.382, 2769.299, 7631.838);
	float d0 = 0.0;
	float d1 = 0.0;
	p0.z -= tm;
	p1.z -= tm;
	
	// Note: Changing z's initial value without correcting
	// for it will denormalize the field's return value.
	float z = 1.0;
	
	float prd = 1.0;
	for(int i = 0; i < MAX_ITERS; i++) {
if (i >= iters) break;
		p0 += sin(p0.yzx * prd);
		p1 += sin(p1.yzx * prd);
		d0 += abs(dot(cos(p0), sin(p0.zxy)) * z);
		d1 += abs(dot(cos(p1), sin(p1.zxy)) * z);
		z *= 0.5;
		prd *= 1.5;
		p0 = p0 * m3;
		p1 = p1 * m3;
	}
	int its = max(iters, 1);
float ztot = 0.5 * (2.0 - 1.0 / pow(2.0, float(its) - 1.0));
float iztot = 1.0 / ztot;
return (vec2(d0, d1) - ztot) * iztot;
}

// Much of the code in sc(), ss(), and mainImage() is derived from davidar's
// wind flow map https://www.shadertoy.com/view/4sKBz3
vec2 sc(vec2 pos) {
	for(int i = -MAX_SMP_DST; i <= MAX_SMP_DST; i++) {
		for(int j = -MAX_SMP_DST; j <= MAX_SMP_DST; j++) {
if (abs(i) > smpDst || abs(j) > smpDst) continue;
		
			// These two lines subtract the current fragCoord's position (pos) from the sum
			// of the previous frame's pixel coordinate and its velocity at the sampled point (res)
			// which is stored in the .xy component of the buffer to get a velocity vector
			// relative to the sampled point, then checks whether that vector points to the
			// center of the current pixel or not.
			vec2 res = texture(iChannel0, pos + vec2(i,j)).xy;
			if(all(lessThan(abs(res - pos), vec2(0.5)))) return res;
		}
	}
	return vec2(0.0);
}

vec3 ss(vec2 pos, vec2 scr) {
	vec2 uv0 = pos * invRes.x;
	
	// This scales the position so that the noise pattern is consistent across different resolutions.
	vec2 uv1 = scale * (scr - 0.5 * iResolution.xy) * invRes.y;
	
	float frame = float(iFrame);
	vec4 hash = bjhash128(vec4(pos, frame, 1.738765));
	
	// This adds random particles with a random initial velocity offset.
	scr = (hash.w <= spawnRate * frmAdj * birthRate / iResolution.x) ? (pos + hash.xy) : scr;
	
	// The velocity field.
	vec2 v = gFBM(vec3(uv1, -3875.27)) * pVel * frmAdj;
	
	for (int i = 0 ; i < MAX_POS; i ++) {
		vec4 pos = positions[i];
		// Mouse velocity offset when clicked.
		if (pos.z > 0.0) {
			vec2 position = pos.xy * invRes.x;
			float md = distance(position, uv0);
			float vFac = smoothstep(0.2, 0.0, md);
			v = mix(v, (uv0 - position) * impulse * pos.w, vFac);
		}
	}
	
	// Mouse velocity offset when clicked.
	if (iMouse.z > 0.0) {
		vec2 mousePos = iMouse.xy * invRes.x;
		float md = distance(mousePos, uv0);
		float vFac = smoothstep(0.2, 0.0, md);
		v = mix(v, (uv0 - mousePos) * impulse, vFac);
	}
	
	// This just adds the position and velocity of the particle that will move into the current pixel to its new velocity.
	// Every on-screen particle at this point except for the one occupying the bottom-leftmost pixel
	// will have a value greater than zero and thus will persist into the next frame,
	// and likewise the resulting sum of the particle's position and its velocity and the new velocity will
	// always be greater than 1.0, which, when put into a vector whose .b component is 1.0, makes it white.
	return (any(greaterThan(scr, vec2(0.0)))) ? vec3(scr + v, 1.0) : vec3(0.0);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
	frmAdj = min(1.0, 144.0 * iTimeDelta);
	tm = iTime * tScale;
	invRes = 1.0 / iResolution.xy;
	vec2 uv = fragCoord;
	
	// Tracer state is calculated here. Decay by a small amount every frame
	// until it reaches zero.
	float r = texture(iChannel0, uv).z - decay * frmAdj;
	
	vec2 scRes = sc(fragCoord);
	r = (any(greaterThan(scRes, vec2(0.0)))) ? 1.0 : clamp(r, 0.0, 1.0);
	vec3 ssRes = ss(fragCoord, scRes);
	
	// The color of the tracer is added to the current frame's color.
	fragColor = vec4(ssRes + vec3(0.0, 0.0, r * 0.998), 1.0);
}



void main( void ) {
	vec4 color = vec4(0.0,0.0,0.0,1.0);
	mainImage( color, gl_FragCoord.xy );
	oFragColor = color;
}
