#pragma once

#include <vector>
#include "lab_m1/Tema2/Building.h"
#include "lab_m1/Tema2/Bullet.h"
#include "lab_m1/Tema2/Tank.h"

namespace m1
{
	class Battlefield
	{
	public:
		Tank* player;
		std::vector<Tank*> enemies;
		std::vector<Building*> buildings;
		std::vector<Bullet*> bullets;

		std::vector<std::vector<int>> field;

		float speed_rot;
		float speed_move;

		float map_x_max;
		float map_x_min;
		float map_z_max;
		float map_z_min;

		float enemy_fire_distance;
		bool player_dead;
		int points;

		Battlefield();
		~Battlefield();
		void init(int side_size, float center_exclusion, int enemy_num, int building_num);
		float tankBuildColl(Tank* t, Building* b);
		glm::vec3 GeneratePos(int side_size, float center_exclusion);
		void CheckTankBuildColl();
		void UpdateBattlefield(float deltaTimeSeconds);
		void Fire(Tank* t);
		bool bulletBuildColl(Bullet* blt, Building* build);
		void CheckBltBuildColl();
		void CheckTanksColl();
		float tanksColl(Tank* t1, Tank* t2);
		void CheckBltTankColl();
		bool bulletTankColl(Bullet* blt, Tank* t);
		void UpdateEnemiesPos(float deltaTimeSeconds);
		int RandNumInRange(const int min, const int max);
		int NextMovement(const int crtState);
		void CheckMapLimits();
		void PutBetweenLimits(Tank* t);
		void EnemyFire(float deltaTimeSeconds);

		float getAngle(glm::vec3 crt_dir, glm::vec3 init_dir, glm::vec3 ref_dir);
		glm::vec3 rotateVec(glm::vec3 vec, float angle, glm::vec3 rotDir);
	};
}
