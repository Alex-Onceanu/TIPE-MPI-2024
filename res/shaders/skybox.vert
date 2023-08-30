attribute vec3 a_Position;
attribute vec3 a_Color;
attribute vec3 a_Normal;

uniform mat4 u_Translation;
uniform mat4 u_Rotation;
uniform mat4 u_View;
uniform mat4 u_Proj;

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec3 v_TexCoords;

void main() {
    v_TexCoords = a_Position;
    vec4 pos = vec4(a_Position, 1.0) * mat4(mat3(u_View)) * u_Proj;
    gl_Position = pos.xyww;
}