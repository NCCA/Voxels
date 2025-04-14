#version 410 core

layout(location = 0) in vec4 inPosition;
flat out int textureIndex;
void main()
{
    textureIndex = int(inPosition.w);
    gl_Position = vec4(inPosition.xyz, 1.0);
}
