precision mediump float;

uniform vec4 u_Light;

varying vec3 v_Color;
varying vec3 v_Normal;

void main() {
    vec3 n_Light = normalize(vec3(u_Light.x, u_Light.y, u_Light.z));
    float p = v_Normal.x * n_Light.x + v_Normal.y * n_Light.y + v_Normal.z * n_Light.z;
    gl_FragColor = vec4(v_Color + 0.25 * p, 1.0);
}
