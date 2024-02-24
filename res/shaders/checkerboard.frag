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

uniform sampler2D u_Texture;

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;

void main() {
    // vec3 uv = (1.0 / 300.0) * v_FragPos;
    float zoom = 0.1;
    float fade_start = 100.0;
    float fade_end = 250.0;
    vec3 uv = fract(zoom * v_FragPos);
    float fade_coef = smoothstep(fade_start, fade_end, length(v_FragPos.xz - u_CameraPos.xz));
    vec3 noise_normal = vec3(0.0, 1.0, 0.0) * fade_coef + (1.0 - fade_coef) * normalize((texture2D(u_Texture, uv.xz)).yxz * 2.0 - 1.0);
    
    //uv = fract(uv);
    // vec3 noise_normal = vec3(0.0, 1.0, 0.0);
    //vec3 noise_normal = (texture2D(u_Texture, uv.xz)).xzy * v_Normal;
    //vec3 noise_normal = (texture2D(u_Texture, v_FragPos.xz)).xyz ;
    //gl_FragColor = vec4(uv, 1.0);
    // gl_FragColor = vec4(noise_normal, 1.0);
    

    float cx = step(0.5, uv.x);
    float cz = step(0.5, uv.z);

    vec3 col = (step(1.0, cx + cz) - step(2.0, cx + cz)) * vec3(0.627451, 0.470588, 0.333333) + vec3(0.83137, 0.72549, 0.588235);

    
    vec3 lightDir = -normalize(v_FragPos - u_Light);
    // lightDir = vec3(0.0, lightDir.y, 0.0);

    // gl_FragColor = vec4(lightDir, 1.0);

    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float ambientIntensity = 1.0;

    float diffuse = max(noise_normal.x * lightDir.x + noise_normal.y * lightDir.y + noise_normal.z * lightDir.z, 0.0);
    // float diffuse = 0.0;

    vec3 reflection = reflect(-lightDir, noise_normal);
    vec3 viewDirection = normalize(v_FragPos - u_CameraPos);
    float produitScalaire = reflection.x * viewDirection.x + reflection.y * viewDirection.y + reflection.z * viewDirection.z;
    float specular = pow(max(produitScalaire, 0.0), 32.0);
    // float specular = 0.0;
    gl_FragColor = vec4(vec3(0.83137, 0.72549, 0.588235) * lightColor * (u_Material.ambient * ambientIntensity + diffuse * u_Material.diffuse + specular * u_Material.specular), 1.0);
}
