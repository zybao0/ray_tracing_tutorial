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
			camera(const vec::real &vfov=30,const vec::real &scale=16./9.,const vec::vec3 &look_at=vec::vec3(1,0,0),const vec::vec3 &camera_up=vec::vec3(0,0,1),const vec::vec3 &look_from=vec::vec3(0,0,0),const vec::real &dis=1);//vfov:竖直方向视场角(单位:度);scale:胶片长比高;camera_up:照相机坐标中的y轴(向上的轴);dis:焦距			
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

	class aabb//平行轴包围盒
	{
		public:
			aabb();
			aabb(const vec::vec3 &a,const vec::vec3 &b);//任意对角坐标
			aabb(const aabb &a,const aabb &b);//移动的物体
			void merge(const aabb &a);//将两个包围盒合并成更大的包围盒
			bool hit(const ray &sight,vec::real tmin,vec::real tmax)const;
			const vec::vec3& min()const;
			const vec::vec3& max()const;
			const vec::real& area()const;
		private:
			vec::vec3 min_,max_;//min_(max_):长方体8个顶点中坐标的x,y,z均最小(大)的点
			vec::real area_;//长方体表面积
	};

	class intersect//相交类
	{
		public:
			virtual bool hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const=0;//sight:光线;t_min,t_max:交点的距离的最小值与最大值,一般分别设为0和inf;rec交点的信息;返回值为是否相交
			const aabb& get_box()const;
			const bool not_optimization()const;
		protected:
			bool not_optimization_;//有些物体（如无穷大平面）返回aabb没有意义，说以选择不优化(当true是不优化)
			aabb box_;//物体的包围盒
		public:
			bool vis;
	};

	const size_t AUTO=0;
	const size_t NONE=1;
	const size_t BVH =2;
	const size_t KDTREE=3;//加速方式

	class intersections:public intersect//相交类的指针数组，用于保存场景
	{
		public:
			intersections(intersect** list,size_t n,int acceleration_type=AUTO);
			virtual bool hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const override;//枚举每一个物体来求最近的交点，chapter 2中的第一段伪代码的实现
		private:
			intersect** list_;//一个指针数组 指向每一个实体
			size_t size_,num_,acceleration_;//size_:物体个数;num_:参与加速结构的物体的个数;acceleration_:加速类型
			intersect *root;//加速结构的根节点
	};

	const int AXIS_X=0;
	const int AXIS_Y=1;
	const int AXIS_Z=2;

	class bvh_node:public intersect//bvh加速结构
	{
		public:
			bvh_node(intersect**world,const int n);
			virtual bool hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const override;
		private:
			intersect *left_,*right_;
	};

	class KD_tree_node:public intersect//kd-tree加速结构
	{
		public:
			KD_tree_node(intersect**world,const int n,const int depth,aabb bound);
			virtual bool hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const override;
		private:
			bool split;
			int split_axis_;
			vec::real split_pos_;
			intersect *left_,*right_;
	};

	class KD_tree_leaf:public intersect
	{
		public:
			KD_tree_leaf(intersect **world,const int n,aabb bound);
			virtual bool hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const override;
		private:
			intersect **list_;
			size_t size_;//叶节点物体数量
	};
};