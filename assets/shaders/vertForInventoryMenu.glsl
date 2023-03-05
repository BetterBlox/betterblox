# version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 texCoord;

out vec2 TextureCoord;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;


void main() 
{
    gl_Position = model * vec4(aPos, 0.0f, 1.0f);
    TextureCoord = texCoord;

}