#pragma once
#include"../core.h"
namespace ray_tracing
{
	class sphere:public intersect
	{
		public:
			sphere(const vec::vec3 &o,const vec::real &r);//o为球心，r为半径
			virtual bool hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const override;
			const vec::real& r()const;
			const vec::vec3& o()const;
		private:
			vec::vec3 o_;
			vec::real r_;
	};
};