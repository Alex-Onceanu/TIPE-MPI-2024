attribute vec4 position;
attribute vec3 a_Color;
varying vec3 v_Color;

uniform vec4 u_Transform;

void main()
{
    gl_Position = u_Transform * position;
    v_Color = a_Color;
}
