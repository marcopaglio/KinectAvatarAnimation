#version 460 core
#ifdef GL_ES
    precision mediump float;
#endif

in vec3 v_Color;
in vec3 v_Normal;
in vec4 v_Position;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(-v_Normal);

	vec3 distance = - vec3(v_Position);
	vec3 toLight = normalize(distance);
	float diffuseFactor = 1.0 / length(distance);
	float diffuse = max(0.0, dot(normal, toLight));
	
	vec3 intensity = v_Color * (2 * diffuse * diffuseFactor);
	fragColor = vec4(intensity, 1.0);
}