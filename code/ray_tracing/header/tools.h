//define vec(type_name) cnum(const_number) rnd(random number)
#pragma once

#include<Eigen/Eigen>
#include<Eigen/Dense>

namespace vec
{
	#ifdef HIGHPRECISION       //set the high precision 
		using real=long double;
	#elif defined LOWPRECISION //set the low preciion
		using real=float;
	#else
		using real=double;     //default precision
	#endif                     //set precision

	using vec2=Eigen::Matrix<real,2,1>;
	using vec3=Eigen::Matrix<real,3,1>;
	using vec4=Eigen::Matrix<real,4,1>;

	using mat2=Eigen::Matrix<real,2,2>;
	using mat3=Eigen::Matrix<real,3,3>;
	using mat4=Eigen::Matrix<real,4,4>;
};

namespace cnum//const_number
{
	const vec::real INF=0x3f3f3f3f;
	const vec::real eps=1e-9;
	const vec::real pi=acos(-1);
};

namespace rnd
{
	vec::real rand();
};