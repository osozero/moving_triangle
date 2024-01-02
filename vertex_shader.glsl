#version 330 core
layout(location = 0) in vec3 aPos;

uniform vec3 translation; //translates the triangle

uniform vec3 scale; //scales it

uniform mat4 z_rotation_matrix;

void main() {
    vec4 rotated = z_rotation_matrix * vec4(aPos, 1.0f);
    vec4 scaled = rotated * vec4(scale, 1.0f);
    gl_Position = scaled + vec4(translation, 0.0f);
}
