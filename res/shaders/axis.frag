precision mediump float;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

uniform vec3 u_Light;
uniform vec3 u_LightColor;
uniform vec3 u_CameraPos;
uniform Material u_Material;

uniform float u_AmbientIntensity;
uniform float u_Reflectivity;

varying vec3 v_Color;
varying float v_Omega;

void main() {
    gl_FragColor = vec4(v_Color * max(1.0 - v_Omega, 0.0) + vec3(1.0, 0.0, 0.0) * min(1.0, v_Omega), 1.0);
}