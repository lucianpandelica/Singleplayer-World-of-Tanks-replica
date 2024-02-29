#pragma once

#include <vector>
#include "components/simple_scene.h"

namespace m1
{
	class Helpers
	{
	public:
		static float GetAngle(glm::vec3 crt_dir, glm::vec3 init_dir, glm::vec3 ref_dir);
		static glm::vec3 RotateVec(glm::vec3 vec, float speed_rot, float deltaTime, glm::vec3 rotDir);
	};
}