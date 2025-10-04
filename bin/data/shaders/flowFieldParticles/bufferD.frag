uniform float damping;
void mainImage( out vec4 fragColor, in vec2 fragCoord ) {
    vec2 uv = fragCoord;
    float r = damping * texture(iChannel3, uv).x;
    if (texture(iChannel2, uv).x > 0.) r = 1.;
    fragColor.x = r;
}
