#include "lab_m1/Tema2/Tank.h"
#include "lab_m1/Tema2/utils.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;

/* for player */
Tank::Tank()
	: pos {glm::vec3(0)}, wheels_color{glm::vec3(0,0,0)},
	body_color {glm::vec3(0, 0.5, 0)}, turret_color{ glm::vec3(0, 0.8, 0) },
	gun_color{ glm::vec3(0.8, 0.8, 0) }, hp{ 100.0 }, scale{ 0.025 } {
	
	init_dir = glm::vec3(0, 0, -1);
	ref_dir = glm::vec3(-1, 0, 0);

	fwd_tank = init_dir;
	fwd_turret = init_dir;
	fwd_gun = init_dir;
	
	radius = 40.0 * scale;
	center_y = 15.0 * scale;

	gun_base_x = 0.0;
	gun_base_y = 30.5 * scale;
	gun_length = 60.0 * scale;

	fire_cooldown = 1.0;
	time_since_fire = fire_cooldown;

	/* for enemies */
	state = FORWARD;
	time_since_state = 0.0;
	state_len = 1.0;

	turret_pos_set = false;

	is_dead = false;
}

/* for enemies */
Tank::Tank(glm::vec3 pos,
		   glm::vec3 fwd,
		   glm::vec3 wheels_color,
		   glm::vec3 body_color,
		   glm::vec3 turret_color,
		   glm::vec3 gun_color,
		   float scale)
	: pos{ pos }, wheels_color{ wheels_color },
	  body_color{ body_color }, turret_color{ turret_color },
	  gun_color{ gun_color }, hp{ 100.0 },
	fwd_tank{ fwd }, fwd_turret{ fwd }, fwd_gun{ fwd }, init_dir{ init_dir }, ref_dir{ ref_dir }, scale { scale } {
	
	init_dir = fwd;

	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(0, 1, 0));
	glm::vec4 aux = glm::vec4(init_dir.x, init_dir.y, init_dir.z, 1);
	ref_dir = aux * modelMatrix;
	
	radius = 40.0 * scale;
	center_y = 15.0 * scale;

	gun_base_x = 0.0;
	gun_base_y = 30.5 * scale;
	gun_length = 60.0 * scale;

	fire_cooldown = 3.0;
	time_since_fire = fire_cooldown;

	state = FORWARD;
	time_since_state = 0.0;
	state_len = 1.0;

	turret_pos_set = false;

	is_dead = false;
}

Tank::~Tank()
{
	// free memory
}

void Tank::updateHP(float delta)
{
	this->hp += delta;

	/* set lower limit */
	if (this->hp < 0) {
		this->hp = 0;
	}
}