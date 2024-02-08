attribute vec3 a_Position;
attribute vec3 a_Color;
attribute vec3 a_Normal;

uniform mat4 u_Translation;
uniform mat4 u_Rotation;
uniform mat4 u_View;
uniform mat4 u_Proj;

uniform vec3 u_Light;

varying vec3 v_Color;
varying vec3 v_Normal;
varying vec3 v_FragPos;
varying vec3 v_Light;

void main() {
    float floor_height = -0.98;
    
    v_Color = a_Color;
    v_Normal = vec3(vec4(a_Normal, 1.0) * (-u_Rotation));
    v_FragPos = vec3(a_Position.x, floor_height, a_Position.z);
    v_Light = u_Light;


    vec3 obj_center = vec3(vec4(0.0, 0.0, 0.0, 1.0) * (u_Rotation * u_Translation));
    // float y_max = 10.0;
    // obj_center.y = y_max * smoothstep(0.0, y_max, obj_center.y); 
    vec3 obj_center_to_light = obj_center - u_Light;

    // Formule obtenue après résolution d'une intersection entre Vect(obj_center_to_light) et l'hyperplan "y = floor_height"
    vec3 new_center = u_Light + obj_center_to_light * (-u_Light.y + floor_height) / obj_center_to_light.y;

    float x_scale = length(obj_center.xz - new_center.xz);
    float sign_of_theta = -(2.0 * (step(1.0, step(0.0, obj_center_to_light.z))) - 1.0);
    float theta = acos(sign_of_theta * obj_center_to_light.x / length(obj_center_to_light));
    mat4 shadow_transform = mat4(cos(theta),0.0, sin(theta),           0.0,
                                0.0,        1.0, 0.0,                  0.0,
                               -sin(theta), 0.0, cos(theta), 0.0,
                                0.0,        0.0, 0.0,                  1.0);


    vec4 frag_world_pos = vec4(x_scale * a_Position.x, a_Position.y, a_Position.z, 1.0) * shadow_transform * (u_Rotation * u_Translation);
        
    // frag_world_pos_floor c'est la position du fragment projetée orthogonalement sur le sol
    vec4 frag_world_pos_floor = vec4(frag_world_pos.x, floor_height, frag_world_pos.z, frag_world_pos.w);
    
    vec3 new_frag_world_pos = frag_world_pos_floor.xyz + vec3(new_center.x - obj_center.x, 0.0, new_center.z - obj_center.z);

    gl_Position = vec4(new_frag_world_pos, 1.0) * u_View * u_Proj;
}
