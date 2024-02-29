#pragma once

#include <vector>

namespace m1
{
	class Building
	{
	public:
		/* XOZ coord - bottom left corner */
		float ox;
		float oz;

		float W; // width - ox
		float L; // length - oz
		float H; // height - oy

		Building(float ox, float oz, float W, float L, float H);
		~Building();
	};
}