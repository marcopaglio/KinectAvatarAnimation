#version 460 core

uniform mat4 u_MvMatrix;
uniform mat4 u_ProjMatrix;
uniform mat4 u_PositionSystemMatrix;
uniform mat4 u_NormalMatrix;

layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Color;

out vec3 v_Color;
out vec3 v_Normal;
out vec4 v_Position;

void main() {
    v_Position = u_MvMatrix * u_PositionSystemMatrix * a_Position;
    gl_Position = u_ProjMatrix * v_Position;
    v_Color = vec3(a_Color);
    v_Normal = vec3(vec4(a_Normal, 0.0) * u_NormalMatrix);
}