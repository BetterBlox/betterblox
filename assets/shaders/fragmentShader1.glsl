#version 330 core
out vec4 FragColor;

in vec3 colorData;
in vec2 texCoord;

uniform sampler2D ourTexture;
uniform sampler2D texture2;
uniform int combine;

void main()
{
    if (combine == 0) {
        FragColor = texture(texture2, texCoord);
    }
    else if (combine == 1) { // mix
        FragColor = (texture(ourTexture, texCoord) + vec4(colorData, 1.0))/vec4(2, 2, 2, 1);
    }
    else if (combine == 2) { // multiply
        FragColor = texture(ourTexture, texCoord) * vec4(colorData, 1);
    }
    else if (combine == 3) { // add
        FragColor = texture(ourTexture, texCoord) + vec4(colorData, 1);
    }
    else if (combine == 4) { // screen
        FragColor = vec4(1, 1, 1, 1) - (vec4(1, 1, 1, 1) - texture(ourTexture, texCoord)) * (vec4(1, 1, 1, 1) - vec4(colorData, 1));
    }
    else if (combine == 5) { // color dodge
        FragColor = texture(ourTexture, texCoord) / (vec4(1) - vec4(colorData, 1));
    }
    else if (combine == 9) {
        FragColor = vec4(colorData, 1);
    }
}