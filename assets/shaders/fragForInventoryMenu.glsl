#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;
uniform sampler2D texturein;
uniform int combine;


void main()
{
    vec2 t = TextureCoord;
    combine == 1 ? t = vec2(TextureCoord.x + .05f, TextureCoord.y - .05f) : t = TextureCoord;
    FragColor = texture(texturein, t); // If combine is 1 the shader will show it normally
}
