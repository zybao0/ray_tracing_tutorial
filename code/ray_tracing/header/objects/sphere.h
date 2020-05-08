#pragma once
#include"../core.h"
namespace ray_tracing
{
	class sphere:public intersect
	{
		public:
			sphere(const vec::vec3 &h,const vec::real &r);//h为圆心，r为半径
			virtual bool hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const override;
			const vec::real& r()const;
			const vec::vec3& heart()const;
		private:
			vec::vec3 heart_;
			vec::real radius_;
	};
};