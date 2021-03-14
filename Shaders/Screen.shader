#shader vertex
#version 460 core
layout(location = 0) in vec3 Position;


void main()
{
    gl_Position = vec4(Position, 1.0);
}

#shader fragment
#version 460 core

out vec4 FragColor;

void main()
{
    FragColor = vec4(1, 0, 0, 1);
}
