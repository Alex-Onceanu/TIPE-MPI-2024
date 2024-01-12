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
    // gl_FragColor = textureCube(u_Cubemap, v_TexCoords).yzxw;
    
    vec3 uv = 2.0 * v_FragPos;

    vec3 col = (1.0 - uv.y) * vec3(1.0, 1.0, 1.0) + uv.y * vec3(0.0, 0.749, 1.0);

    // vec3 col = vec3(0.0, cx, cz);

    gl_FragColor = vec4(col, 1.0);
}