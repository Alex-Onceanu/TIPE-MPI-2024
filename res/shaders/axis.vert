attribute vec3 a_Position;
attribute vec3 a_Color;
attribute vec3 a_Normal;

uniform mat4 u_Translation;
uniform mat4 u_Rotation;
uniform mat4 u_View;
uniform mat4 u_Proj;

uniform float u_Omega;

varying vec3 v_Color;
varying float v_Omega;

void main() {
    mat4 rot = u_Rotation;
    float scaling = min(2.0, u_Omega);
    rot[0][0] = rot[0][0] * scaling;
    rot[1][0] = rot[1][0] * scaling;
    rot[2][0] = rot[2][0] * scaling;

    gl_Position = vec4(a_Position, 1.0) * (rot * u_Translation) * u_View * u_Proj;
    
    v_Color = a_Color;
    v_Omega = u_Omega / 3.0;
}
