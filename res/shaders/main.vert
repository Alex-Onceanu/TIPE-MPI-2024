attribute vec3 position;
attribute vec3 a_Color;

uniform mat4 u_Translation;
uniform mat4 u_Rotation;
uniform mat4 u_View;
uniform mat4 u_Proj;

varying vec3 v_Color;
varying vec3 v_Normal;

void main() {
    gl_Position = vec4(position, 1.0) * (u_Rotation * u_Translation) * u_View * u_Proj;
    v_Color = a_Color;
    v_Normal = vec3(vec4(normalize(position), 1.0) * (-u_Rotation));
}
