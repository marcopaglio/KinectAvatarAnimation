#version 460 core

uniform mat4 u_ModelMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjMatrix;
uniform mat4 u_NormalMatrix;

layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Color;

out vec3 v_Color;
out vec3 v_Normal;
out vec4 v_Position;

void main() {
    v_Position = u_ModelMatrix * a_Position;
    gl_Position = u_ProjMatrix * u_ViewMatrix * v_Position;
    v_Color = a_Color;
    v_Normal = vec3(u_NormalMatrix * vec4(a_Normal, 0.0));
}