#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform int combine;

void main()
{
    if (combine == 0) {
        FragColor = texture(ourTexture, TexCoord);
    }
    else if (combine == 1) { // mix
        FragColor = (texture(ourTexture, TexCoord) + vec4(ourColor, 1.0))/vec4(2, 2, 2, 1);
    }
    else if (combine == 2) { // multiply
        FragColor = (texture(ourTexture, TexCoord) * vec4(ourColor, 1.0));
    }
    else if (combine == 3) { // add
        FragColor = (texture(ourTexture, TexCoord) + vec4(ourColor, 1.0));
    }
    else if (combine == 4) { // screen combine 1 - (1 - a) * (1 - b)
        FragColor = vec4(1, 1, 1, 1) - (vec4(1, 1, 1, 1) - texture(ourTexture, TexCoord)) * (vec4(1, 1, 1, 1) - vec4(ourColor, 1.0));
    }
    else if (combine == 5) { // color dodge  a / (1 - b)
        FragColor = texture(ourTexture, TexCoord) / (vec4(1) - vec4(ourColor, 1));
    }
    else if (combine == 9) { // color only
        FragColor = vec4(ourColor, 1);
    }
    
}