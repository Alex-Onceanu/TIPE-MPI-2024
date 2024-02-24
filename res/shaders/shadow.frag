precision mediump float;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
};

uniform vec3 u_CameraPos;
uniform Material u_Material;

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;

varying vec3 v_Light;

varying float v_ObjY;
varying vec3 v_FragWorldPos;
varying vec3 v_CenterWorldPos;
varying float v_Xscale;

float smoothmax(float a, float b, float k)
{
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(a, b, h) + k * h * (1.0 - h);
}

void main() {
    vec3 lightDir = normalize(v_FragPos - v_Light);

    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float ambientIntensity = 1.0;

    float diffuse = max(v_Normal.x * lightDir.x + v_Normal.y * lightDir.y + v_Normal.z * lightDir.z, 0.0);

    vec3 reflection = reflect(-lightDir, v_Normal);
    vec3 viewDirection = normalize(v_FragPos - u_CameraPos);
    float produitScalaire = reflection.x * viewDirection.x + reflection.y * viewDirection.y + reflection.z * viewDirection.z;
    float specular = pow(max(produitScalaire, 0.0), 32.0);

    float smooth_edges = 0.4;
    float drop_shadow = (smoothstep(0.0, 1.0, 4.0 / v_Xscale)) * (1.0 - smoothstep(smooth_edges, 1.0, v_FragPos.x * v_FragPos.x + v_FragPos.z * v_FragPos.z));
    float ortho_shadow = 1.0 - smoothstep(smooth_edges, 1.0, length(v_FragWorldPos.xz - v_CenterWorldPos.xz));
    float is_frag_transparent = smoothstep(0.1, 1.0, smoothmax(drop_shadow, ortho_shadow, smooth_edges));
    // float is_frag_transparent = 1.0;
    
    gl_FragColor = vec4(v_Color * lightColor * (u_Material.ambient * ambientIntensity + diffuse * u_Material.diffuse + specular * u_Material.specular), 0.4 * smoothstep(0.0, 1.0, 3.0 / v_ObjY) * is_frag_transparent);
}