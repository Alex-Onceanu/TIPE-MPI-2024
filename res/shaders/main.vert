attribute vec4 position;
attribute vec3 a_Color;
attribute vec2 a_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;
varying vec3 v_Color;
varying vec2 v_TexCoords;

void main() {
    gl_Position = position * u_Model * u_View * u_Proj;
    v_Color = a_Color;
    v_TexCoords = a_TexCoords;
}
