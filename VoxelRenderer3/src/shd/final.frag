#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform ivec2 screenResolution;

void main()
{
    vec4 col = texture(screenTexture, TexCoords).rgba;
    FragColor = vec4(col.xyz, 1.0);
} 