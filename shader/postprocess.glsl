#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 size;

out vec4 finalColor;

const float samples = 5.0;      
const float quality = 2.5;     

void main()
{
    vec4 sum = vec4(0);
    vec2 sizeFactor = vec2(1) / size * quality;

    vec4 source = texture(texture0, fragTexCoord);

    const int range = 2;

    for (int x = -range; x <= range; x++)
    {
        for (int y = -range; y <= range; y++)
        {
            sum += texture(texture0, fragTexCoord + vec2(x, y) * sizeFactor);
        }
    }

    finalColor = ((sum / (samples * samples)) * 0.4 + source) * colDiffuse;

    // finalColor.rgb = finalColor.rgb / (finalColor.rgb + vec3(1));
    finalColor.rgb = vec3(1) - exp(-finalColor.rgb);
}
