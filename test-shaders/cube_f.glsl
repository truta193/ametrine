#version 330 core

struct PointLight {
   vec3 position;
   float constant;
   float linear;
   float quadratic;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

in vec2 uv;
in vec3 normals;
in vec3 frag_pos;
uniform sampler2D u_tex1;
uniform PointLight u_point_light1;
uniform PointLight u_point_light2;
uniform vec3 u_cam_pos;
out vec4 frag_color;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
   vec3 nrm = normalize(normals);
   vec3 view_dir = normalize(u_cam_pos - frag_pos);
   vec3 point_light = CalcPointLight(u_point_light1, nrm, frag_pos, view_dir);
   point_light += CalcPointLight(u_point_light2, nrm, frag_pos, view_dir);
   frag_color = vec4(point_light, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
   vec3 lightDir = normalize(light.position - fragPos);
   // diffuse shading
   float diff = max(dot(normal, lightDir), 0.0);
   // specular shading
   vec3 reflectDir = reflect(-lightDir, normal);
   //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
   vec3 halfwayDir = normalize(lightDir + viewDir);
   float spec = pow(max(dot(normal, halfwayDir), 0.0), 16);
   // attenuation
   float distance = length(light.position - fragPos);
   float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
   // combine results
   vec3 ambient = light.ambient * vec3(texture(u_tex1, uv));
   vec3 diffuse = light.diffuse * diff * vec3(texture(u_tex1, uv));
   vec3 specular = light.specular * spec * vec3(texture(u_tex1, uv));
   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;
   return (ambient + diffuse + specular);
}