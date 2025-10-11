OF_GLSL_SHADER_HEADER
uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform float     iFrameRate;            // shader frame rate
uniform int       iFrame;                // shader playback frame
uniform sampler2DRect iChannel0;
uniform vec4 iMouse;
out vec4 oFragColor;

 // Colorization: 0-70% blue, 70-95% orange, 95-100% white
const vec3 COLOR_DARK_BLUE = vec3(0.00, 0.45, 1.00);
const vec3 COLOR_ORANGE    = vec3(1.00, 0.65, 0.40);
const vec3 COLOR_WHITE     = vec3(1.00, 0.85, 0.70); // pale orange-white

#// Mapping controls
const float LOW_THRESH  = 0.0;  // brightness lower bound
const float HIGH_THRESH = 1.0;  // brightness upper bound
const float GAMMA       = 1.0;  // gamma on normalized brightness
uniform float brightness;

// Smooth threshold parameters (t after normalization and gamma)
const float T_BLUE_ORANGE   = 0.9;  // transition center (set 0.80 for later handoff)
const float T_ORANGE_WHITE  = 0.99;  // transition center (set 0.999 for very late white)
const float W_BLUE_ORANGE   = 0.04;  // half-width of transition window
const float W_ORANGE_WHITE  = 0.01;  // half-width of transition window near white

// If source encodes brightness in blue channel (as existing code suggests)
float toBrightness(vec4 c) {
    return c.b;
}

// Neighborhood mean brightness with adjustable sample size (odd integer, e.g. 3, 5, 7)
const int SAMPLE_SIZE = 3; // must be odd
float neighborhoodBrightness(vec2 uv) {
    float sum = 0.0;
    int sampleStep = (SAMPLE_SIZE) / 2;
    int count = 0;
    for (int j = -sampleStep; j <= sampleStep; ++j) {
        for (int i = -sampleStep; i <= sampleStep; ++i) {
            sum += toBrightness(texture(iChannel0, uv + vec2(float(i), float(j))));
            count++;
        }
    }
    return sum / float(count);
}

vec3 colorize(float b) {
    float t = clamp((b - LOW_THRESH) / max(1e-6, HIGH_THRESH - LOW_THRESH), 0.0, 1.0);
    t = pow(t, GAMMA);

    // Smooth piecewise mapping using smoothstep around thresholds.
    // Blue -> Orange near T_BLUE_ORANGE, then Orange -> White near T_ORANGE_WHITE.
    float blueToOrange  = smoothstep(T_BLUE_ORANGE - W_BLUE_ORANGE, T_BLUE_ORANGE + W_BLUE_ORANGE, t);
    float orangeToWhite = smoothstep(T_ORANGE_WHITE - W_ORANGE_WHITE, T_ORANGE_WHITE + W_ORANGE_WHITE, t);

    vec3 c = mix(COLOR_DARK_BLUE, COLOR_ORANGE, blueToOrange);
    c = mix(c, COLOR_WHITE, orangeToWhite);
    return c;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fragCoord; // sampler2DRect uses pixel coords
    float b = neighborhoodBrightness(uv);
    vec3  col = colorize(b) * brightness;

    // Keep alpha tied to brightness so downstream passes can use it if needed
    fragColor = vec4(col, b);
}

void main(void) {
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    mainImage(color, gl_FragCoord.xy);
    oFragColor = color;
}
