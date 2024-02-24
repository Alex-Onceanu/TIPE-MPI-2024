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
    
    // vec3 uv = 2.0 * v_FragPos;

    // vec3 col = (1.0 - uv.y) * vec3(0.588,0.8,0.89) + uv.y * vec3(0.098,0.0,0.529);

    // gl_FragColor = vec4(col, 1.0);
}