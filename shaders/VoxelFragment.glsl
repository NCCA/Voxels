#version 410 core

layout(location = 0) out vec4 outColour;
in vec2 textureCoords;
uniform sampler2D textureSampler;
void main()
{
    outColour = texture(textureSampler, textureCoords);
}
