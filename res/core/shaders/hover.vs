#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNrm;

uniform mat3 normal_model;
uniform mat4 final_tform;
uniform mat4 deferred_tform;
uniform vec3 camera_relative;

uniform float f_coef;

out vec3 vNrm;
out vec3 vPos;

out float flogz;

uniform float grow;

void main()
{
    gl_Position = final_tform * vec4(aPos + aNrm * grow, 1.0f);
	gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * f_coef - 1.0;
	flogz = 1.0 + gl_Position.w;

	vNrm = normal_model * aNrm;
	vPos = (deferred_tform * vec4(aPos, 1.0f)).xyz;

}
