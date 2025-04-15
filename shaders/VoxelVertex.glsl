#version 410 core
uniform samplerBuffer posSampler; // index 1

flat out int textureIndex;
flat out int vertexID;
void main()
{
    vec4 inputData = texelFetch(posSampler, gl_VertexID);
    textureIndex = int(inputData.w);
    vertexID = textureIndex;
    gl_Position = vec4(inputData.xyz, 1.0);
}
