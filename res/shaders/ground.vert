attribute vec3 a_Position;
attribute vec3 a_Color;
attribute vec3 a_Normal;

uniform mat4 u_Translation;
uniform mat4 u_Rotation;
uniform mat4 u_View;
uniform mat4 u_Proj;

varying vec3 v_FragPos;

void main() {
    gl_Position = vec4(a_Position, 1.0) * (u_Rotation * u_Translation) * u_View * u_Proj;
    v_FragPos = vec3(vec4(a_Position, 1.0) * (u_Rotation * u_Translation));
}
