#version 120
uniform sampler2D currentTexture;
uniform float framesLeft;

void main() {
	vec2 coord = gl_TexCoord[0].xy;
	vec4 pixelColour = texture2D(currentTexture, coord);
  float rValue = (pixelColour[0] + ((1.0 - pixelColour[0]) / 19.0) * (20 - framesLeft));
  float gValue = (pixelColour[1] + ((1.0 - pixelColour[1]) / 19.0) * (20 - framesLeft));
  float bValue = (pixelColour[2] + ((1.0 - pixelColour[2]) / 19.0) * (20 - framesLeft));
  float aValue = (pixelColour[3] - (pixelColour[3] / 19.0) * (20 - framesLeft));
	gl_FragColor = vec4(rValue, gValue, bValue, aValue);
}