#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float HP;

// Object color
uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;
out float hp;
// Output values to fragment shader
out vec3 world_position;
out vec3 world_normal;

void main()
{
	// Send color light output to fragment shader
	world_position = vec3 (Model * vec4 (v_position, 1));
    world_normal = normalize(vec3 (Model * vec4 (v_normal, 0)));
	
	color = object_color;
	hp = HP;

	if (hp == 200) {
		gl_Position = Projection * View * Model * vec4(v_position, 1.0);
	} else {
	
		gl_Position = Projection * View * Model * mat4(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 10 * sin(HP / 100), 0, 1) * vec4(v_position, 1.0);
						
		// gl_Position = Projection * View * Model * vec4(v_position + sin(HP / 100) * v_normal, 1.0);
	}
}