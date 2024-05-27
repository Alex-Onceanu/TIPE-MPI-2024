precision mediump float;

uniform samplerCube u_Skybox;

varying vec3 v_TexCoords;

void main() {
    // L'image est chargée sous format GBR et non pas RGB
    gl_FragColor = textureCube(u_Skybox, v_TexCoords).yzxw;
}