//ray tracing core
//including:
//class:ray camera(chapter 1)
//class:hitpoint intersect intersections(chapter 2)
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
			vec::vec3 origin_,direction_;//origin_(光的起点) direction_(光的方向)
	};

	class camera
	{
		public:
			ray get_ray(vec::real u,vec::real v);//传入归一化后的坐标
			ray get_ray(vec::vec2 p);
	};

	struct hitpoint//交点的信息
	{
		vec::real t;//ray 中的系数t(direction的t倍) 
		vec::vec3 p,n;//p:相交点、撞击点的坐标;n:p点的表面的法向量(应该为单位向量)
	};

	class intersect//相交类
	{
		public:
			virtual bool hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const=0;//sight:光线;t_min,t_max:交点的距离的最小值与最大值,一般分别设为0和inf;rec交点的信息;返回值为是否相交
	};

	class intersections:public intersect//相交类的指针数组，用于保存场景
	{
		public:
			intersections(intersect** list,size_t n);
			virtual bool hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const override;//枚举每一个物体来求最近的交点，chapter 2中的第一段伪代码的实现
		private:
			intersect** list_;//一个指针数组 指向每一个实体
			size_t size_;//物体个数
	};
};