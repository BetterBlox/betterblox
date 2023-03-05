#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;
uniform sampler2D texturein;


void main()
{
    FragColor = texture(texturein, TextureCoord);
}
