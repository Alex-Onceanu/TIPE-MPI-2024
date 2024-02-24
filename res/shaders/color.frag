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

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec3 v_CenterWorldPos;



void main() {
    vec3 light_dir = normalize(v_CenterWorldPos - u_Light);
    vec3 view_dir = normalize(v_CenterWorldPos - u_CameraPos);

    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float ambientIntensity = 1.0;

    float diffuse = max(v_Normal.x * light_dir.x + v_Normal.y * light_dir.y + v_Normal.z * light_dir.z, 0.0);

    vec3 n = normalize(light_dir + view_dir);
    float produitScalaire = v_Normal.x * n.x + v_Normal.y * n.y + v_Normal.z * n.z;
    float specular = diffuse > 0.0 ? pow(max(produitScalaire, 0.0), 128.0) : 0.0;

    gl_FragColor = vec4(v_Color * lightColor * (u_Material.ambient * ambientIntensity + diffuse * u_Material.diffuse + specular * u_Material.specular), 1.0);
}
