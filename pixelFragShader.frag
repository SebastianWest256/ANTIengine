#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float pixelSize;

void main()
{   
    vec2 pixelatedCoords = vec2(pixelSize * floor(TexCoords.x / pixelSize), pixelSize * floor(TexCoords.y / pixelSize));
    FragColor = texture(screenTexture, pixelatedCoords);
}
