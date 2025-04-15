#version 410 core
flat in int vertID;
layout(location = 0) out vec4 outColour;
layout(location = 1) out int indexID;
in vec2 textureCoords;
uniform sampler2D textureSampler;
void main()
{
    outColour = texture(textureSampler, textureCoords);
    indexID = vertID;
}
