#version 410 core

layout(location = 0) out vec4 outColour;
layout(location = 1) out int indexID;
flat in int isactive;
in vec2 textureCoords;
flat in int vertID;
uniform sampler2D textureSampler;
void main()
{
    if (isactive == 0)
        discard;
    outColour = texture(textureSampler, textureCoords);
    indexID = vertID;
}
