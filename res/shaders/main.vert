attribute vec4 position;
attribute vec3 a_Color;
attribute vec2 a_TexCoords;

uniform mat4 u_Transform;
uniform mat4 u_Proj;
varying vec3 v_Color;
varying vec2 v_TexCoords;

void main() {
    gl_Position = vec4(position.x, position.y, position.z, 1.0) * u_Transform;
    v_Color = a_Color;
    v_TexCoords = a_TexCoords;
}
