#pragma once
#include "stdlib.h"
#include <string>

class Helpers
{
private:
	Helpers();

public:
	template <class T>T static RandomInRange(T min, T max)
	{
		double value = min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));

		return static_cast<T>(value);
	};
};

