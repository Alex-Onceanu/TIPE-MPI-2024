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

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;

void main() {
    vec3 lightDir = normalize(v_FragPos - u_Light);

    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float ambientIntensity = 1.0;

    float diffuse = max(v_Normal.x * lightDir.x + v_Normal.y * lightDir.y + v_Normal.z * lightDir.z, 0.0);

    vec3 reflection = reflect(-lightDir, v_Normal);
    vec3 viewDirection = normalize(v_FragPos - u_CameraPos);
    float produitScalaire = reflection.x * viewDirection.x + reflection.y * viewDirection.y + reflection.z * viewDirection.z;
    float specular = pow(max(produitScalaire, 0.0), 64.0);

    gl_FragColor = vec4(v_Color * lightColor * (u_Material.ambient * ambientIntensity + diffuse * u_Material.diffuse + specular * u_Material.specular), 1.0);
}
