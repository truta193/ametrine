#version 330 core

in vec3 a_pos;
in vec2 a_uv;
in vec3 a_normals;
uniform mat4 u_view;
out vec2 uv;
out vec3 normals;
out vec3 frag_pos;

void main() {
   gl_Position = u_view * vec4(a_pos, 1.0);
   uv = a_uv;
   normals = a_normals;
   frag_pos = a_pos;
}