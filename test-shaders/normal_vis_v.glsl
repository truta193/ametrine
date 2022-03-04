#version 330 core

in vec3 a_normal;
uniform mat4 u_view;

void main() {
    gl_Position = u_view * vec4(a_normal, 1.0);
}