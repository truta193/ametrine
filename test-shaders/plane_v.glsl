#version 330 core
/*
         {.uniform_id = view_mat_id, .data = view.elements},
                {.uniform_id = sampler_id, .data = &plane_tex_id},
                {.uniform_id = ambient_id, .data = light_color.xyz},
                {.uniform_id = light_pos_id, .data = light_pos.xyz},
                {.uniform_id = camera_pos_id, .data = test_cam.cam.transform.position.xyz}*/
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