#version 330 core

in vec2 uv;
in vec3 normals;
in vec3 frag_pos;
uniform sampler2D u_tex1;
uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;
uniform int blinn_toggle;
out vec4 frag_color;

void main() {
   float ambient_strenght = 0.05;
   vec3 color = texture(u_tex1, uv).rgb;
   vec3 ambient = ambient_strenght * color;

   vec3 light_dir = normalize(u_light_pos - frag_pos);
   float diff = max(dot(normals, light_dir), 0.0);
   vec3 diffuse = diff * color;

   float spec = 0.0;
   float specular_strenght = 0.3;
   vec3 view_dir = normalize(u_cam_pos - frag_pos);
   if (bool(blinn_toggle)) {
      vec3 reflect_dir = reflect(-light_dir, normals);
      spec = pow(max(dot(view_dir, reflect_dir), 0.0), 16);
   } else {
      vec3 halfDir = normalize(light_dir + view_dir);
      spec = pow(max(dot(normals, halfDir), 0.0), 32);
   };
   vec3 specular = vec3(specular_strenght) * spec;


   frag_color = vec4(ambient, 1.0) + vec4(diffuse, 1.0) + vec4(specular, 1.0);
}