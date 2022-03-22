
#version 330 core
precision mediump float;

in vec3 a_pos;

uniform mat4 u_view;
out vec2 uv;

void main() {
   gl_Position = u_view * vec4(a_pos, 1.0);
}


/*
#version 330 core
layout (location = 0) in vec2 aPos;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
}*/