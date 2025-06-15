#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform int s;
uniform int c;
uniform vec4 color;


void main()
{
	vec4 texColor = texture(ourTexture, TexCoord);
	FragColor = mix(color, texColor, float(s));
}

