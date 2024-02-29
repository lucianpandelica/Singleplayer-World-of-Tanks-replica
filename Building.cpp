#include "lab_m1/Tema2/Building.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;

Building::Building(float ox, float oz, float W, float L, float H)
	: ox {ox}, oz {oz}, W {W}, L {L}, H{H} {
}

Building::~Building()
{
	// free memory
}