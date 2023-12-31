#version 330 core
layout(location = 0) in vec3 aPos;

uniform vec3 translation; //translates the triangle

uniform vec3 scale; //scales it

void main() {
    vec3 scaled_pos = aPos * scale;
    gl_Position = vec4(scaled_pos + translation, 1.0);
}
