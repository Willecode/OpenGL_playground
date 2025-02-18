#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 normal;
out vec3 fragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//uniform mat4 shadowBiasVP;
uniform mat4 shadowmapVP;
out vec4 shadowCoord;

void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(fragPos, 1.0);
    normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoord;

    shadowCoord = shadowmapVP * model * vec4(aPos,1.0);

    // TBN matrix
    vec3 T = normalize(vec3(model * vec4(aTangent,     0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent,   0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,      0.0)));
    TBN = mat3(T, B, N);

}