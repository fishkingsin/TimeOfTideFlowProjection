void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	vec2 uv = fragCoord;
	
	fragColor = texture(iChannel2, uv);
}
