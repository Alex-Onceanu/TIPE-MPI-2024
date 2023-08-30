precision mediump float;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

uniform vec3 u_Light;
uniform vec3 u_CameraPos;
uniform Material u_Material;
uniform samplerCube u_Cubemap;

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec3 v_TexCoords;

void main() {
    // L'image est chargée sous format GBR et non pas RGB
    gl_FragColor = textureCube(u_Cubemap, v_TexCoords).yzxw;
}