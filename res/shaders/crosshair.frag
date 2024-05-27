precision mediump float;

varying vec3 v_Color;
varying float v_Omega;

void main()
{
    gl_FragColor = vec4(v_Color, 0.6);
}