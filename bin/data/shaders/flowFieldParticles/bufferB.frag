uniform float particleSpeedScale;
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
	fragColor.xy = p + (v * particleSpeedScale);
}
