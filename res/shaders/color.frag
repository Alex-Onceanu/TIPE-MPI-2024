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

uniform samplerCube u_Cubemap;

uniform float u_AmbientIntensity;
uniform float u_Reflectivity;

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec3 v_CenterWorldPos;
varying vec3 v_LocalPos;



void main() {
    float frag_theta = atan(v_LocalPos.y / v_LocalPos.x);
    float frag_phi = acos(v_LocalPos.z);

    vec3 light_dir = normalize(v_FragPos - u_Light);
    vec3 view_dir = normalize(v_FragPos - u_CameraPos);

    float diffuse = max(v_Normal.x * light_dir.x + v_Normal.y * light_dir.y + v_Normal.z * light_dir.z, 0.0);

    vec3 n = normalize(light_dir + view_dir);
    float produitScalaire = v_Normal.x * n.x + v_Normal.y * n.y + v_Normal.z * n.z;
    float specular = diffuse > 0.0 ? pow(max(produitScalaire, 0.0), 128.0) : 0.0;


    vec3 frag_to_sky = reflect(view_dir, v_Normal);
    vec3 sky_color = textureCube(u_Cubemap, frag_to_sky).yzx;
    float ground_blend = smoothstep(0.1, -0.1, -frag_to_sky.y);
    sky_color = sky_color * ground_blend + (1.0 - ground_blend) * vec3(0.83137, 0.72549, 0.588235);

    if(fract(frag_phi) <= 0.1 || fract(frag_theta) <= 0.1)
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        gl_FragColor = vec4(((1.0 - u_Reflectivity) * v_Color + u_Reflectivity * sky_color) * u_LightColor * (u_Material.ambient * u_AmbientIntensity + diffuse * u_Material.diffuse + specular * u_Material.specular), 1.0);
    }
}
