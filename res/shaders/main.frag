precision mediump float;

uniform vec4 u_Brightness;
varying vec3 v_Color;

void main()
{
    gl_FragColor = u_Brightness * vec4(v_Color, 1.0);
}
