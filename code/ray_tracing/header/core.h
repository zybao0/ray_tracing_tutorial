//core(including class:ray)
#pragma once

#include<tools.h>

namespace ray_tracing
{
	class ray
	{
		public:
			ray(const vec::vec3 &origin,const vec::vec3 &direction);
			ray(const ray &r);
			const vec::vec3& origin()const;
			const vec::vec3& direction()const;
			vec::vec3 go(const vec::real &t)const;
		private:
			vec::vec3 origin_,direction_;
	};

	class camera
	{
		public:
			ray get_ray(vec::real u,vec::real v);//传入归一化后的坐标
			ray get_ray(vec::vec2 p);
	};
};