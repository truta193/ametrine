
#version 330 core
precision mediump float;

uniform sampler2D u_tex;
in vec2 uv;
out vec4 frag_color;

void main() {
   frag_color = texture(u_tex, uv);
}


/*
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
} */