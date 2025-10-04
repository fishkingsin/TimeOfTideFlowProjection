#define TESTING 0
uniform vec3 baseColor;
uniform float colorised;

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	vec2 uv = fragCoord;
	vec4 texCol = texture(iChannel0,uv);
//	
//	vec4 original_color = texCol;
//
//	// Define the target and replacement colors (these would be parameters)
//	vec3 target_color = vec3(1.0, 0.0, 0.0); // Pure red
//	vec3 replacement_color = vec3(1.0, 0.5, .0); // Orange
//
//	// Check if the original color is close to the target color
//	float tolerance = 0.5; // A small range around the target color
//	float diff = dot(abs(original_color.rgb - target_color), vec3(1.0/3.0)); // Calculate color difference
//
//	// If the difference is within the tolerance, blend with the replacement color
//	vec3 final_color = mix(original_color.rgb, replacement_color, step(diff, tolerance));
//
//	// Set the final pixel color
//	texCol = vec4(final_color, texCol.a);
	
	
	
	float r = texture(iChannel3, uv).x;
	//r = 0.9 - 0.8 * r;
	fragColor = vec4(vec3(r,r,r)*((texCol.rgb * (1 - colorised)) + (baseColor * colorised)), 1);
	
	float Y = slideValY * iResolution.y;
	float X = slideValX * iResolution.x;
	if (TESTING==1){
		if(uv.x < X && uv.y > Y){
			fragColor = texture(iChannel0, uv);
		}
		
		if(uv.x > X && uv.y > Y){
			fragColor = texture(iChannel1, uv);
		}
		
		if(uv.x < X && uv.y < Y){
			fragColor = texture(iChannel3, uv);
		}
		
		if(uv.x > X && uv.y < Y){
			fragColor = texture(iChannel2, uv);
		}
	}
	
	
	
}
