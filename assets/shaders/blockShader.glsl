#version 330 core
out vec4 FragColor;

in vec3 colorData;
in vec2 texCoord;

uniform sampler2D texture2;

void main()
{
    FragColor = texture(texture2, texCoord);
    
}