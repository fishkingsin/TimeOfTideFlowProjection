uniform float particleSpeedScale;

// 25 position
uniform vec3 positions[25];
// The amount of velocity added by clicking.
const float impulse = 15.0;

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	vec2 uv = fragCoord;
	vec2 p = texture(iChannel2, uv).xy;
	float prevFrame = float(iFrame) - 1.;
	float mFrame = float(prevFrame); // iFrame
	if(p == vec2(0)) {
		if (hash13(vec3(fragCoord, mFrame)) > 2e-4) return;
		p = fragCoord + hash21(float(mFrame)) - 0.5;
	}
	vec2 v = 2. * texture(iChannel0, uv).xy - 1.;
	
	for(int i = 0 ; i < 25; i++) {
		if (positions[i].z > 0.0) {
			vec2 pos = positions[i].xy;
			float md = distance(pos, uv);
			float vFac = smoothstep(0.15, 0.0, md);
			v = mix(v, (uv - pos) * impulse, vFac);
		}
	}
	
	fragColor.xy = p + (v * particleSpeedScale);
}
