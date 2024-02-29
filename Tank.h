#pragma once

#include <vector>
#include "components/simple_scene.h"

namespace m1
{
	class Tank
	{
	public:
		/* current tank coordinates */
		glm::vec3 pos;

		glm::vec3 fwd_tank;
		glm::vec3 fwd_turret;
		glm::vec3 fwd_gun;

		/* initial color - will be displayed differently by the vertex shader */
		glm::vec3 wheels_color;
		glm::vec3 body_color;
		glm::vec3 turret_color;
		glm::vec3 gun_color;

		glm::vec3 init_dir;
		glm::vec3 ref_dir;

		float hp;
		float radius;
		float scale;
		float center_y;
		float gun_base_x;
		float gun_base_y;
		float gun_length;

		float time_since_fire; // will be added to in update function
		float fire_cooldown;

		int state; // for enemies AI system
		float time_since_state;
		float state_len;

		bool turret_pos_set;
		bool is_dead;

		Tank();
		Tank(glm::vec3 pos,
			 glm::vec3 fwd,
			 glm::vec3 wheels_color,
			 glm::vec3 body_color,
			 glm::vec3 turret_color,
			 glm::vec3 gun_color,
			 float scale = 0.025);
		~Tank();
		void updateHP(float delta);
	};
}