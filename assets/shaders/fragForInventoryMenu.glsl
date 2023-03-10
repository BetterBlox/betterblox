#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;
uniform sampler2D texturein;
uniform int combine;


void main()
{
    FragColor = texture(texturein, TextureCoord) - (vec4(0.2f) * (1 - combine)); // If combine is 1 the shader will show it normally
}
