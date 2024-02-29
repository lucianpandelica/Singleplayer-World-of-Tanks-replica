#include "lab_m1/Tema2/Bullet.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;

Bullet::Bullet(glm::vec3 pos, glm::vec3 dir)
	: pos{ pos }, dir{ dir } {
	time_since_fire = 0.0;
	time_threshold = 20.0;
	scale = 0.25;
	speed = 20.0;
	radius = 0.5 * scale;
}

Bullet::~Bullet()
{
	// free memory
}