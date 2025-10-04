uniform int smpDst;
void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	fragColor = vec4(0);
	
	for(int i = -smpDst; i <= smpDst; i++) {
		for(int j = -smpDst; j <= smpDst; j++) {
			vec4 c = texture(iChannel1, (fragCoord + vec2(i,j)));
			if(abs(c.x - fragCoord.x) < 0.5 && abs(c.y - fragCoord.y) < 0.5) {
				fragColor = c;
				return;
			}
		}
	}
}
