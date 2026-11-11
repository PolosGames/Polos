#version 450

layout(set = 1, binding = 1) uniform sampler2D uTexture[32];

layout(location = 0) in vec4 iColor;
layout(location = 1) in vec2 iTexCoord;
layout(location = 2) in flat uint iTexIndex;
layout(location = 3) in float iTilingFactor;

layout(location = 0) out vec4 oColor;

void main()
{
    vec4 color = iColor;

    switch (int(iTexIndex))
    {
        case 0: color *= texture(uTexture[0], iTexCoord * iTilingFactor); break;
        case 1: color *= texture(uTexture[1], iTexCoord * iTilingFactor); break;
        case 2: color *= texture(uTexture[2], iTexCoord * iTilingFactor); break;
        case 3: color *= texture(uTexture[3], iTexCoord * iTilingFactor); break;
        case 4: color *= texture(uTexture[4], iTexCoord * iTilingFactor); break;
        case 5: color *= texture(uTexture[5], iTexCoord * iTilingFactor); break;
        case 6: color *= texture(uTexture[6], iTexCoord * iTilingFactor); break;
        case 7: color *= texture(uTexture[7], iTexCoord * iTilingFactor); break;
        case 8: color *= texture(uTexture[8], iTexCoord * iTilingFactor); break;
        case 9: color *= texture(uTexture[9], iTexCoord * iTilingFactor); break;
        case 10: color *= texture(uTexture[10], iTexCoord * iTilingFactor); break;
        case 11: color *= texture(uTexture[11], iTexCoord * iTilingFactor); break;
        case 12: color *= texture(uTexture[12], iTexCoord * iTilingFactor); break;
        case 13: color *= texture(uTexture[13], iTexCoord * iTilingFactor); break;
        case 14: color *= texture(uTexture[14], iTexCoord * iTilingFactor); break;
        case 15: color *= texture(uTexture[15], iTexCoord * iTilingFactor); break;
        case 16: color *= texture(uTexture[16], iTexCoord * iTilingFactor); break;
        case 17: color *= texture(uTexture[17], iTexCoord * iTilingFactor); break;
        case 18: color *= texture(uTexture[18], iTexCoord * iTilingFactor); break;
        case 19: color *= texture(uTexture[19], iTexCoord * iTilingFactor); break;
        case 20: color *= texture(uTexture[20], iTexCoord * iTilingFactor); break;
        case 21: color *= texture(uTexture[21], iTexCoord * iTilingFactor); break;
        case 22: color *= texture(uTexture[22], iTexCoord * iTilingFactor); break;
        case 23: color *= texture(uTexture[23], iTexCoord * iTilingFactor); break;
        case 24: color *= texture(uTexture[24], iTexCoord * iTilingFactor); break;
        case 25: color *= texture(uTexture[25], iTexCoord * iTilingFactor); break;
        case 26: color *= texture(uTexture[26], iTexCoord * iTilingFactor); break;
        case 27: color *= texture(uTexture[27], iTexCoord * iTilingFactor); break;
        case 28: color *= texture(uTexture[28], iTexCoord * iTilingFactor); break;
        case 29: color *= texture(uTexture[29], iTexCoord * iTilingFactor); break;
        case 30: color *= texture(uTexture[30], iTexCoord * iTilingFactor); break;
        case 31: color *= texture(uTexture[31], iTexCoord * iTilingFactor); break;
    }

    if (color.a == 0)
    {
        discard;
    }

    oColor = color;
}
