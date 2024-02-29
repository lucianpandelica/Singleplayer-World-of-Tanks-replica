#include "lab_m1/Tema2/Battlefield.h"
#include "lab_m1/Tema2/Helpers.h"
#include "lab_m1/Tema2/utils.h"

#include <vector>
#include <iostream>
#include <random>

using namespace std;
using namespace m1;

Battlefield::Battlefield()
{

}

Battlefield::~Battlefield()
{
	// free memory -> allocated diamonds and other elements
}

void Battlefield::init(int side_size, float center_exclusion,
					   int enemy_num, int building_num)
{
	// init field
	speed_rot = 50.0;
	speed_move = 5.0;

	map_x_max = 25.0;
	map_x_min = -25.0;
	map_z_max = 25.0;
	map_z_min = -25.0;

	enemy_fire_distance = 6.0;
	player_dead = false;
	points = 0;

	// init player
	glm::vec3 init_dir = glm::normalize(glm::vec3(0, 0, -1));
	player = new Tank();

	// generate buildings
	for (int i{ 0 }; i < building_num; i++) {
		glm::vec3 pos = GeneratePos(side_size, center_exclusion);

		float W = std::rand() % 3 + 1.0;
		float L = std::rand() % 3 + 1.0;
		float H = std::rand() % 4 + 1.0;

		Building* new_b = new Building(pos.x, pos.z, W, L, H);
		buildings.push_back(new_b);
	}

	bool found_pos = false;
	float ox;
	float oy;

	vector<glm::vec3> directions;
	directions.push_back(glm::vec3(0, 0, -1)); // facing -OZ
	directions.push_back(glm::vec3(0, 0, 1)); // facing +OZ
	directions.push_back(glm::vec3(-1, 0, 0)); // facing -OX
	directions.push_back(glm::vec3(1, 0, 0)); // facing +OX

	// generate enemies
	for (int i{ 0 }; i < enemy_num; i++) {
		// decide position

		// if it's inside building, will be checked later
		glm::vec3 pos = GeneratePos(side_size, center_exclusion);
		int dir = std::rand() % directions.size();
		
		// TODO: randomize enemy color
		Tank* new_enemy = new Tank(pos, directions[dir],
			glm::vec3(1, 1, 1), glm::vec3(0.8, 0, 0.1), glm::vec3(0.5, 0, 0.2), glm::vec3(0.2, 0, 0.1));

		enemies.push_back(new_enemy);
	}
}

float Battlefield::tankBuildColl(Tank* t, Building* b)
{
	float tank_x = t->pos.x;
	float tank_y = 0.5;
	float tank_z = t->pos.z;

	float b_min_x = b->ox;
	float b_max_x = b->ox + b->W;
	float b_min_y = 0.0;
	float b_max_y = b->H;
	float b_min_z = b->oz;
	float b_max_z = b->oz + b->L;

	float x = static_cast<float>(fmax(b_min_x, fmin(tank_x, b_max_x)));
	float y = static_cast<float>(fmax(b_min_y, fmin(tank_y, b_max_y)));
	float z = static_cast<float>(fmax(b_min_z, fmin(tank_z, b_max_z)));

	float dist = sqrt((x - tank_x) * (x - tank_x) +
		(y - tank_y) * (y - tank_y) +
		(z - tank_z) * (z - tank_z));
	
	return dist;
}

bool Battlefield::bulletBuildColl(Bullet* blt, Building* b)
{
	float blt_x = blt->pos.x;
	float blt_y = blt->pos.y;
	float blt_z = blt->pos.z;

	float b_min_x = b->ox;
	float b_max_x = b->ox + b->W;
	float b_min_y = 0.0;
	float b_max_y = b->H;
	float b_min_z = b->oz;
	float b_max_z = b->oz + b->L;

	float x = static_cast<float>(fmax(b_min_x, fmin(blt_x, b_max_x)));
	float y = static_cast<float>(fmax(b_min_y, fmin(blt_y, b_max_y)));
	float z = static_cast<float>(fmax(b_min_z, fmin(blt_z, b_max_z)));

	float dist = sqrt((x - blt_x) * (x - blt_x) +
		(y - blt_y) * (y - blt_y) +
		(z - blt_z) * (z - blt_z));

	return dist < blt->radius;
}

bool Battlefield::bulletTankColl(Bullet* blt, Tank* t)
{
	return glm::distance(blt->pos, t->pos) < blt->radius + t->radius;
}

float Battlefield::tanksColl(Tank* t1, Tank* t2)
{
	/* greater than zero -> collision */
	return t1->radius + t2->radius - glm::distance(t1->pos, t2->pos);
}

glm::vec3 Battlefield::GeneratePos(int side_size, float center_exclusion) {
	float ox = static_cast<float>(std::rand() % side_size) + center_exclusion;
	float oz = static_cast<float>(std::rand() % side_size) + center_exclusion;

	int sign_ox = (std::rand() % 2 == 0) ? 1 : -1;
	ox = sign_ox * ox;

	int sign_oz = (std::rand() % 2 == 0) ? 1 : -1;
	oz = sign_oz * oz;

	return glm::vec3(ox, 0, oz);
}

void Battlefield::CheckTankBuildColl()
{
	/* for player tank */
	for (Building* build : buildings) {
		float dist = tankBuildColl(player, build);

		if (dist < player->radius) {
			/* collision detected */
			float build_center_x = build->ox + (build->W / 2.0);
			float build_center_z = build->oz + (build->L / 2.0);
			glm::vec3 b_center = glm::vec3(build_center_x, 0, build_center_z);

			glm::vec3 dif = glm::normalize(b_center - player->pos);
			player->pos -= dif * (2 * (player->radius - dist));
		}
	}

	/* for enemy tanks */
	for (Building* build : buildings) {
		for (Tank* e : enemies) {
			float dist = tankBuildColl(e, build);

			if (dist < e->radius) {
				/* collision detected */
				float build_center_x = build->ox + (build->W / 2.0);
				float build_center_z = build->oz + (build->L / 2.0);
				glm::vec3 b_center = glm::vec3(build_center_x, 0, build_center_z);

				glm::vec3 dif = glm::normalize(b_center - e->pos);
				e->pos -= dif * (2 * (e->radius - dist));
			}
		}
	}
}

void Battlefield::CheckBltBuildColl()
{
	for (Building* build : buildings) {
		for (auto it = bullets.begin(); it != bullets.end(); ) {
			if (bulletBuildColl((*it), build)) {
				/* remove bullet */
				delete (*it);
				it = bullets.erase(it);
			}
			else {
				it++;
			}
		}
	}
}

void Battlefield::CheckTanksColl()
{
	float p;

	/* player and enemies */
	for (Tank* t : enemies) {
		p = tanksColl(player, t);

		if (p > 0.f) {
			/* collision */
			glm::vec3 dif = (t->pos - player->pos);
			glm::vec3 res = p * glm::normalize(dif);
			player->pos += res * -0.5f;
			t->pos += res * 0.5f;
		}
	}

	/* enemies with enemies */
	for (int i{ 0 }; i < enemies.size() - 1; i++) {
		for (int j{ i + 1 }; j < enemies.size(); j++) {
			p = tanksColl(enemies[i], enemies[j]);

			if (p > 0.f) {
				/* collision */
				glm::vec3 dif = (enemies[i]->pos - enemies[j]->pos);
				glm::vec3 res = p * glm::normalize(dif);
				enemies[j]->pos += res * -0.5f;
				enemies[i]->pos += res * 0.5f;
			}
		}
	}
}

void Battlefield::CheckBltTankColl()
{
	/* for player */
	for (auto it = bullets.begin(); it != bullets.end(); ) {
		if (bulletTankColl((*it), player)) {
			/* bullet dissapears */
			delete (*it);
			it = bullets.erase(it);

			/* decrease hp */
			player->updateHP(-20);

			if (player->hp <= 0) {
				player_dead = true;
			}
		}
		else {
			it++;
		}
	}

	/* for enemies */
	for (Tank* e : enemies) {
		for (auto it = bullets.begin(); it != bullets.end(); ) {
			if (bulletTankColl((*it), e)) {
				/* bullet dissapears */
				delete (*it);
				it = bullets.erase(it);

				/* decrease hp */
				e->updateHP(-20);

				if (e->is_dead == false && e->hp <= 0) {
					points += 100;
					e->is_dead = true;
				}
			}
			else {
				it++;
			}
		}
	}
}

void Battlefield::UpdateBattlefield(float deltaTimeSeconds)
{
	/* update bullets */
	for (auto it = bullets.begin(); it != bullets.end(); )
	{
		if ((*it)->time_since_fire > (*it)->time_threshold) {
			delete (*it);
			it = bullets.erase(it);
		}
		else {
			(*it)->pos = (*it)->pos + (*it)->dir * (*it)->speed * deltaTimeSeconds;
			(*it)->time_since_fire += deltaTimeSeconds;
			it++;
		}
	}

	/* update player */
	player->time_since_fire += deltaTimeSeconds;

	/* update enemy tanks */
	for (Tank* e : enemies) {
		e->time_since_fire += deltaTimeSeconds;
	}
}

void Battlefield::Fire(Tank* t) {
	/* bullet will spawn at the top of the gun */

	if (t->time_since_fire > t->fire_cooldown) {
		/* compute gun top position */
		glm::vec3 gun_base_pos = t->pos + t->fwd_tank * t->gun_base_x + glm::vec3(0, t->gun_base_y, 0);
		
		glm::vec3 gun_dir = t->init_dir;

		float tank_angle = getAngle(t->fwd_tank, t->init_dir, t->ref_dir);
		float turret_angle = getAngle(t->fwd_turret, t->init_dir, t->ref_dir);
		float gun_angle = getAngle(t->fwd_gun, t->init_dir, t->ref_dir);

		gun_dir = rotateVec(gun_dir, -tank_angle, glm::vec3(0, 1, 0));
		gun_dir = rotateVec(gun_dir, -turret_angle, glm::vec3(0, 1, 0));
		gun_dir = rotateVec(gun_dir, -gun_angle, glm::vec3(1, 0, 0));
		
		/* compute top of gun position */
		glm::vec3 gun_top_pos = gun_base_pos + gun_dir * t->gun_length;

		/* add new bullet to battlefield */
		Bullet* new_blt = new Bullet(gun_top_pos, gun_dir);
		bullets.push_back(new_blt);

		t->time_since_fire = 0.0;
	}
}

float Battlefield::getAngle(glm::vec3 crt_dir, glm::vec3 init_dir, glm::vec3 ref_dir)
{
	float angleCos = glm::dot(init_dir, crt_dir);
	float angle = acos(angleCos);
	float dir = glm::dot(ref_dir, crt_dir);
	if (dir < 0) {
		angle = -angle;
	}

	return angle;
}

glm::vec3 Battlefield::rotateVec(glm::vec3 vec, float angle, glm::vec3 rotDir)
{
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::rotate(modelMatrix, angle, rotDir);
	glm::vec4 aux = glm::vec4(vec.x, vec.y, vec.z, 1);
	glm::vec4 res = aux * modelMatrix;

	return glm::normalize(glm::vec3(res.x, res.y, res.z));
}

int Battlefield::RandNumInRange(const int min, const int max)
{
	static std::random_device randomDevice;
	static std::default_random_engine randomEngine(randomDevice());

	std::uniform_int_distribution<int> uniformDist(min, max);

	return uniformDist(randomEngine);
}

int Battlefield::NextMovement(const int crtState)
{
	int randomChange = RandNumInRange(0, 1);
	int nextState = crtState;

	switch (crtState)
	{
	case FORWARD:
	case BACKWARDS:
		nextState = (randomChange == 1)
			? ROT_LEFT
			: ROT_RIGHT;
		break;

	case ROT_LEFT:
	case ROT_RIGHT:
		nextState = (randomChange == 1)
			? FORWARD
			: BACKWARDS;
		break;

	default:
		break;
	}

	return nextState;
}

void Battlefield::UpdateEnemiesPos(float deltaTimeSeconds)
{
	for (Tank* e : enemies) {
		/* update only enemies with positive hp */
		if (e->hp <= 0)
			continue;

		switch (e->state) {
			case FORWARD:
				e->pos += e->fwd_tank * speed_move * deltaTimeSeconds;
				break;
			case BACKWARDS:
				e->pos -= e->fwd_tank * speed_move * deltaTimeSeconds;
				break;
			case ROT_LEFT:
				e->fwd_tank = Helpers::RotateVec(e->fwd_tank, -speed_rot, deltaTimeSeconds, glm::vec3(0, 1, 0));
				break;
			case ROT_RIGHT:
				e->fwd_tank = Helpers::RotateVec(e->fwd_tank, speed_rot, deltaTimeSeconds, glm::vec3(0, 1, 0));
				break;
			default:
				break;
		}

		e->time_since_state += deltaTimeSeconds;

		if (e->state != NO_MOVEMENT && e->time_since_state > e->state_len) {
			/* compute new state and len */
			e->state = NextMovement(e->state);
			e->state_len = static_cast<float>(RandNumInRange(2, 4));
			e->time_since_state = 0.0;
		}
	}
}

void Battlefield::PutBetweenLimits(Tank* t)
{
	if (t->pos.x > map_x_max) {
		t->pos.x = map_x_max;
	}

	if (t->pos.x < map_x_min) {
		t->pos.x = map_x_min;
	}

	if (t->pos.z > map_z_max) {
		t->pos.z = map_z_max;
	}

	if (t->pos.z < map_z_min) {
		t->pos.z = map_z_min;
	}
}

void Battlefield::CheckMapLimits()
{
	PutBetweenLimits(player);

	for (Tank* e : enemies) {
		PutBetweenLimits(e);
	}
}

void Battlefield::EnemyFire(float deltaTimeSeconds)
{
	for (Tank* e : enemies) {
		if (e->hp <= 0)
			continue;

		/* if player was in range and is not anymore */
		if (e->turret_pos_set == true && glm::distance(player->pos, e->pos) > enemy_fire_distance) {
			e->turret_pos_set = false;
			//e->state = FORWARD;
		}

		/* check if distance less than threshold */
		if (glm::distance(player->pos, e->pos) < enemy_fire_distance) {
			if (e->turret_pos_set == true) {
				/* change pos to match player pos and fire */
				float angle_enemy_tank = Helpers::GetAngle(e->fwd_tank, glm::vec3(0, 0, -1), glm::vec3(-1, 0, 0));
				
				glm::vec3 dir = glm::normalize(player->pos - e->pos);
				float angle_dir = Helpers::GetAngle(dir, glm::vec3(0, 0, -1), glm::vec3(-1, 0, 0));

				glm::vec3 new_fwd_turret = rotateVec(e->init_dir, angle_enemy_tank - angle_dir, glm::vec3(0, 1, 0));
				e->fwd_turret = new_fwd_turret;

				Fire(e);
			}
			else {
				/* begin changing turret pos */
				float angle_enemy_tank = Helpers::GetAngle(e->fwd_tank, glm::vec3(0, 0, -1), glm::vec3(-1, 0, 0));

				glm::vec3 dir = glm::normalize(player->pos - e->pos);
				float angle_dir = Helpers::GetAngle(dir, glm::vec3(0, 0, -1), glm::vec3(-1, 0, 0));

				glm::vec3 new_fwd_turret = rotateVec(e->init_dir, angle_enemy_tank - angle_dir, glm::vec3(0, 1, 0));

				glm::vec3 aux = rotateVec(e->fwd_turret, RADIANS(-90), glm::vec3(0, 1, 0));
				float diff = Helpers::GetAngle(e->fwd_turret, new_fwd_turret, aux);
			
				if (diff < 0) {
					e->fwd_turret = rotateVec(e->fwd_turret, fmax(- 0.1 * speed_rot * deltaTimeSeconds, diff), glm::vec3(0, 1, 0));
					diff += 0.1 * speed_rot * deltaTimeSeconds;
				}
				else {
					e->fwd_turret = rotateVec(e->fwd_turret, fmin(0.1 * speed_rot * deltaTimeSeconds, diff), glm::vec3(0, 1, 0));
					diff -= 0.1 * speed_rot * deltaTimeSeconds;
				}

				if (abs(diff) <= 0.1) {
					e->turret_pos_set = true;
				}
			}
		}
	}
}