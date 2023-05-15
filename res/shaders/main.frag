precision mediump float;

uniform vec4 u_Brightness;
varying vec3 v_Color;
varying vec2 v_TexCoords;

uniform sampler2D u_Texture;

void main() {
    gl_FragColor = texture2D(u_Texture, v_TexCoords);// * vec4(v_Color, 1.0);
    // gl_FragColor = vec4(v_Color, 1.0);
}
