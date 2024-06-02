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

uniform samplerCube u_Skybox;
uniform samplerCube u_NormalMap;

uniform float u_AmbientIntensity;
uniform float u_Reflectivity;
uniform float u_Radius;

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec3 v_LocalPos;



void main() {
    vec3 light_dir = normalize(v_FragPos - u_Light);
    vec3 view_dir = normalize(v_FragPos - u_CameraPos);

    vec3 normal = normalize(textureCube(u_NormalMap, v_LocalPos).yzx * 2.0 - 1.0);

    vec3 new_z = normalize(v_Normal);
    vec3 new_y = normalize(vec3(0.0, v_Normal.z, -v_Normal.y));
    vec3 new_x = cross(new_y, new_z);

    mat3 PT = mat3( vec2(new_x.x, new_x.y), new_x.z,
                    vec2(new_y.x, new_y.y), new_y.z,
                    vec2(new_z.x, new_z.y), new_z.z);
                    
    vec3 true_normal = u_Radius > 0.9 ? normalize(PT * normal) : v_Normal;

    float diffuse = max(dot(true_normal, light_dir), 0.0);

    vec3 n = normalize(light_dir + view_dir);
    float produitScalaire = dot(true_normal, n);
    float specular = diffuse > 0.0 ? pow(max(produitScalaire, 0.0), 128.0) : 0.0;


    vec3 frag_to_sky = reflect(view_dir, true_normal);
    vec3 sky_color = textureCube(u_Skybox, frag_to_sky).yzx;
    float ground_blend = smoothstep(0.1, -0.1, -frag_to_sky.y);
    sky_color = sky_color * ground_blend + (1.0 - ground_blend) * vec3(0.83137, 0.72549, 0.588235);

    if(u_Radius > 0.9)
    {
        if((1.0 - max(0.0, dot(true_normal, v_Normal))) > 0.01)
        {
            gl_FragColor = vec4(vec3(0.4, 0.4, 0.4), 0.7);
        }
        else
            gl_FragColor = vec4(((1.0 - u_Reflectivity) * v_Color + u_Reflectivity * sky_color) * u_LightColor * (u_Material.ambient * u_AmbientIntensity + diffuse * u_Material.diffuse + specular * u_Material.specular), 1.0);
    }
    else
        gl_FragColor = vec4(v_Color * u_LightColor * (u_Material.ambient * u_AmbientIntensity + diffuse * u_Material.diffuse + specular * u_Material.specular), 1.0);
}
