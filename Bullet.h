#pragma once

#include <vector>
#include "components/simple_scene.h"

namespace m1
{
	class Bullet
	{
	public:
		/* current bullet coordinates */
		glm::vec3 pos;

		/* bullet direction */
		glm::vec3 dir;

		/* time since fire */
		float time_threshold;
		float time_since_fire;

		float scale;
		float speed;
		float radius;

		Bullet(glm::vec3 pos, glm::vec3 dir);
		~Bullet();
	};
}