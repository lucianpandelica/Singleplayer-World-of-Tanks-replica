#version 330

// Input
// TODO(student): Get color value from vertex shader
in vec3 color; // the new object_color
in float hp;

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 second_light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms

uniform int light_type;
uniform float cut_angle;
uniform int toon_shader;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    vec3 N = normalize(world_normal);
    vec3 L = normalize(light_position - world_position); // vector spre directia luminii
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);
    vec3 R = normalize(reflect(L, world_normal));

    vec3 L2 = normalize(second_light_position - world_position);
    vec3 H2 = normalize(L2 + V);
    vec3 R2 = normalize(reflect(L2, world_normal));
    
    float ambient_light = 1.0;
    float diffuse_light = material_kd * 2 * max(dot(world_normal, L), 0);
    float diffuse_light2 = material_kd * 2 * max(dot(world_normal, L2), 0);
    float specular_light = 0;
    float specular_light2 = 0;

    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
    }

    if (diffuse_light2 > 0)
    {
        specular_light2 = material_ks * pow(max(dot(N, H2), 0), material_shininess);
    }

    float dist2 = distance(second_light_position, world_position);
	float a_f2 = 1.0 / (dist2 * dist2 + 1.0);
	float light2 = a_f2 * (diffuse_light2 + specular_light2);

    float light = 0;

    if (light_type == 0)
    {
        float dist = distance(light_position, world_position);
	    float a_f = 1.0 / (dist * dist + 1.0);
	    light = ambient_light + a_f * (diffuse_light + specular_light);
    } else {
        float cut_off = radians(cut_angle);
        float spot_light = dot(-L, light_direction);
        float spot_light_limit = cos(cut_off);

        if (spot_light > spot_light_limit)
        {
            // Quadratic attenuation
            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            float light_att_factor = pow(linear_att, 2);
            light = ambient_light + light_att_factor * (diffuse_light + specular_light);
        } else {
            light = ambient_light;
        }
    }

    light += light2;

    int colors = 3;
    float res_light = light;
    
    if (toon_shader == 1)
    {
        res_light *= max(ceil(diffuse_light * colors) / colors, 0);
    }

    vec3 res_color = color * res_light;
    out_color = vec4(color, 1);

    if (hp == 200) {
        out_color = vec4(res_color, 1);
    } else {
        out_color = (0.2 + 0.8 * (hp / 100.0)) * vec4(res_color, 1);
    }
}
