#version 330 core
out vec4 FragColor;

//uniform vec3 lightCol;


void main()
{
    vec3 col = vec3(1.0, 1.0, 1.0);
    FragColor = vec4(col, 1.0);
}