#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// TODO(student): Declare any other uniforms
uniform int spotlightFragment;
uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    vec3 L = normalize(light_position - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L + V);

    // TODO(student): Define ambient, diffuse and specular light components
    float ambient_light = 0.25 * material_kd;
    float diffuse_light = material_kd * 1 * max (dot(world_normal,L), 0);
    vec3 R = reflect(-L, world_normal);
    float specular_light = 0;

    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(V, R), 0), material_shininess);
    }

    // TODO(student): If (and only if) the light is a spotlight, we need to do
    // some additional things.
    vec3 culoare;
    if(spotlightFragment == 0)
    {
        float d = distance(world_position, light_position);
		float factorAtenuare = 1/(pow(d,2)+1);
		culoare = object_color * ((ambient_light + factorAtenuare * (diffuse_light + specular_light)));
    }
    else
    {
		float cut_off = radians(30.0);
		float spot_light = dot(-L, light_direction);
		float spot_light_limit = cos(cut_off);
		if (spot_light > cos(cut_off))
		{
		    float d = distance(world_position, light_position);
			float factorAtenuare = 1/(pow(d,2)+1);
			// Se calculeaza valoarea luminii conform  modelului Phong, fragmentul este iluminat de spot
			// Se calculeaza atenuarea luminii
			float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
			float light_att_factor = pow(linear_att, 2);
			culoare = object_color * ((ambient_light + factorAtenuare * light_att_factor * (diffuse_light + specular_light)));
		}
        else
        {
			culoare = object_color * ambient_light;
		}
	}
	

    // TODO(student): Compute the total light. You can just add the components
    // together, but if you're feeling extra fancy, you can add individual
    // colors to the light components. To do that, pick some vec3 colors that
    // you like, and multiply them with the respective light components.
    




    // TODO(student): Write pixel out color
    out_color = vec4(culoare, 1);

}
