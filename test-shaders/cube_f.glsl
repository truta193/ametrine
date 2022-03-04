#version 330 core

in vec2 uv;
in vec3 normals;
in vec3 pos;
uniform sampler2D u_tex1;
uniform vec3 u_ambient_col;
uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
out vec4 frag_color;

void main() {

   float ambient_strenght = 0.1;
   vec3 ambient = ambient_strenght * u_ambient_col;

   vec3 light_dir = normalize(u_light_pos - pos);
   float diff = max(dot(normals, light_dir), 0.0);
   vec3 diffuse = diff * u_ambient_col;

   float specular_strenght = 0.5;
   vec3 view_dir = normalize(u_cam_pos - pos);
   vec3 reflect_dir = reflect(-light_dir, normals);
   float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 64);
   vec3 specular = specular_strenght * spec * u_ambient_col;


   frag_color = (vec4(ambient, 1.0) + vec4(diffuse, 1.0) + vec4(specular, 1.0)) * texture(u_tex1, uv);
}