precision mediump float;

varying vec3 v_Color;
varying float v_Omega;

void main() {
    gl_FragColor = vec4(v_Color * max(1.0 - v_Omega, 0.0) + vec3(1.0, 0.0, 0.0) * min(1.0, v_Omega), 1.0);
}