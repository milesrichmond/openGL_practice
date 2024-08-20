#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

uniform sampler2D currentTexture1;
uniform sampler2D currentTexture2;

void main() {
    FragColor = mix(texture(currentTexture1, texCoord), texture(currentTexture2, texCoord), 0.2);
};