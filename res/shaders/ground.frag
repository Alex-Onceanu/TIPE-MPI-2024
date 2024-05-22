precision mediump float;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

uniform vec3 u_Light;
uniform vec3 u_CameraPos;
uniform vec3 u_LightColor;
uniform Material u_Material;

uniform sampler2D u_Texture;

uniform float u_AmbientIntensity;

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;

void main() {
    float zoom = 0.05;
    float fade_start = 100.0;
    float fade_end = 250.0;
    vec3 uv = fract(zoom * v_FragPos);
    float fade_coef = smoothstep(fade_start, fade_end, length(v_FragPos.xz - u_CameraPos.xz));
    vec3 noise_normal = vec3(0.0, 1.0, 0.0) * fade_coef + (1.0 - fade_coef) * normalize((texture2D(u_Texture, uv.xz)).yxz * 2.0 - 1.0);
    
    float cx = step(0.5, uv.x);
    float cz = step(0.5, uv.z);
    vec3 col = (step(1.0, cx + cz) - step(2.0, cx + cz)) * vec3(0.627451, 0.470588, 0.333333) + vec3(0.83137, 0.72549, 0.588235);
    
    vec3 lightDir = -normalize(v_FragPos - u_Light);
    float diffuse = max(noise_normal.x * lightDir.x + noise_normal.y * lightDir.y + noise_normal.z * lightDir.z, 0.0);

    vec3 reflection = reflect(-lightDir, noise_normal);
    vec3 viewDirection = normalize(v_FragPos - u_CameraPos);
    float produitScalaire = reflection.x * viewDirection.x + reflection.y * viewDirection.y + reflection.z * viewDirection.z;
    float specular = pow(max(produitScalaire, 0.0), 32.0);

    gl_FragColor = vec4(vec3(0.83137, 0.72549, 0.588235) * u_LightColor * (u_Material.ambient * u_AmbientIntensity + diffuse * u_Material.diffuse + specular * u_Material.specular), 1.0);
}
