#version 120
uniform sampler2D currentTexture;
uniform float time;

void main() {
	vec2 coord = gl_TexCoord[0].xy;
	vec4 pixelColour = texture2D(currentTexture, coord);
	vec4 poisonOverlay = {0.0, 0.6, 0.0, abs(sin(time))};
	float gValue = (abs(sin(time)) * 0.25) + 0.6;
	gl_FragColor = vec4(pixelColour[0]*0.8, gValue, pixelColour[2]*0.8, pixelColour[3]);
}