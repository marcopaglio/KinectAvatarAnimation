#version 460 core
#ifdef GL_ES
    precision mediump float;
#endif

uniform vec3 u_LightDiffuseRight;
uniform vec3 u_LightDiffuseLeft;
uniform vec3 u_LightDirectional;
uniform vec3 u_LightColor;

in vec3 v_Color;
in vec3 v_Normal;
in vec4 v_Position;

out vec4 fragColor;

void main() {
	float ambient = 0.10f;
    vec3 normal = normalize(v_Normal);

	vec3 distanceRight = u_LightDiffuseRight - vec3(v_Position);
	vec3 toLightRight = normalize(distanceRight);
	float diffuseFactorRight = 1.0 / length(distanceRight);
	float diffuseRight = max(0.0f, dot(normal, toLightRight));

	vec3 distanceLeft = u_LightDiffuseLeft - vec3(v_Position);
	vec3 toLightLeft = normalize(distanceLeft);
	float diffuseFactorLeft = 1.0 / length(distanceLeft);
	float diffuseLeft = max(0.0f, dot(normal, toLightLeft));

	vec3 toLightDirectional = normalize(u_LightDirectional);
	float diffuseDirectional = max(0.0, dot(normal, toLightDirectional));
	
	vec3 intensity = u_LightColor * v_Color * (ambient + diffuseRight * diffuseFactorRight + diffuseLeft * diffuseFactorLeft + diffuseDirectional);
	fragColor = vec4(intensity, 1.0);
}