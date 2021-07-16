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
			camera(vec::real vfov=30,vec::real scale=16./9.,vec::vec3 look_at=vec::vec3(1,0,0),vec::vec3 camera_up=vec::vec3(0,0,1),vec::vec3 look_from=vec::vec3(0,0,0),vec::real dis=1);//vfov:竖直方向视场角(单位:度);scale:胶片长比高;camera_up:照相机坐标中的y轴(向上的轴);dis:焦距			
			ray get_ray(vec::real u,vec::real v);//传入归一化后的坐标
			ray get_ray(vec::vec2 p);
		private:
			vec::vec3 look_from_,look_at_,camera_x_,camera_y_,camera_z_;//look_from_:照相机位置;look_at_:照相机正对的点的坐标;camera_x_:照相机直播中x轴向量在世界坐标系的值(y,z同理)
			vec::real f_,h_,w_;//f_:焦距，在针孔照相机中指的是针孔到胶片的距离，默认为一;h_:摄像头胶片高度;w_:摄像头胶片宽度

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