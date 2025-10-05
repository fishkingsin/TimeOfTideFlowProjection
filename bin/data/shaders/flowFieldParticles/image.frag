#define TESTING 0
uniform vec3 baseColor;
uniform vec3 overrideColor;
uniform float colorised;
uniform float luminance;
uniform float tolerance;

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
	vec2 uv = fragCoord;
	vec4 texCol = texture(iChannel0,uv);
	
	vec4 original_color = texCol;

	// Define the target and replacement colors (these would be parameters)
	vec3 target_color = vec3(1.0, 0.0, 0.0); // Pure red
	vec3 replacement_color = overrideColor; // Orange
	// Check if the original color is close to the target color
	float mTolerance = tolerance == 0 ? 0.4 : tolerance; // A small range around the target color
	float diff = dot(abs(original_color.rgb - target_color), vec3(1.0/3.0)); // Calculate color difference

	// If the difference is within the mTolerance, blend with the replacement color
	// retain luminance
	vec3 final_color = original_color.rgb;
	if (diff < mTolerance) {
		// Calculate luminance of the original color
		float luminance = dot(original_color.rgb, vec3(0.299, 0.587, 0.114));
		// Adjust the replacement color to match the luminance
		vec3 adjusted_replacement = replacement_color * (luminance / dot(replacement_color, vec3(0.299, 0.587, 0.114)));
		final_color = adjusted_replacement;
	}

	// Set the final pixel color
	texCol = vec4(final_color, texCol.a);
	
	
	
	float r = texture(iChannel3, uv).x;
	//r = 0.9 - 0.8 * r;
	fragColor = vec4(vec3(r,r,r)*((texCol.rgb * (1 - colorised)) + (baseColor * colorised)), 1);
	// increas luminance
	fragColor.rgb *= (1.0 + luminance);
	
	
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
