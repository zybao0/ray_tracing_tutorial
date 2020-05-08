#include<ctime>
#include<random>
#include<cstdlib>

#include<tools.h>

vec::real rnd::rand()
{
	static std::random_device device;
	static std::mt19937 generator(device());
	static std::uniform_real_distribution<vec::real>random;
	return random(generator);
}