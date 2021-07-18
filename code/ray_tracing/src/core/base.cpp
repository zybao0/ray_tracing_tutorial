#include<core.h>
// #include<iostream>
namespace ray_tracing
{
	ray::ray(const vec::vec3 &origin,const vec::vec3 &direction):origin_(origin),direction_(direction.normalized()){}
	ray::ray(const ray &r):origin_(r.origin()),direction_(r.direction()){}
	const vec::vec3& ray::origin()const{return origin_;}
	const vec::vec3& ray::direction()const{return direction_;}
	vec::vec3 ray::go(const vec::real &t)const{return origin_+direction_*t;}

	camera::camera(const vec::real &vfov,const vec::real &scale,const vec::vec3 &look_at,const vec::vec3 &camera_up,const vec::vec3 &look_from,const vec::real &dis):look_from_(look_from),look_at_(look_at),f_(dis)
	{
		camera_z_=(look_from-look_at).normalized();//为了保证右手系，z轴与照相机方向相反
		camera_x_=camera_up.cross(camera_z_).normalized();
		camera_y_=camera_z_.cross(camera_x_).normalized();
		h_=2*f_*tan(vfov/360.*cnum::pi);
		w_=h_*scale;
	}
	ray camera::get_ray(vec::real u,vec::real v){return ray(look_from_,(0.5-v)*w_*camera_x_+(0.5-u)*h_*camera_y_-f_*camera_z_);}
	ray camera::get_ray(vec::vec2 p){return ray(look_from_,(0.5-p.y())*w_*camera_x_+(0.5-p.x())*h_*camera_y_-f_*camera_z_);}

	const aabb& intersect::get_box()const{return box_;}
	const bool intersect::not_optimization()const{return not_optimization_;};

	intersections::intersections(intersect** list,size_t n,int acceleration_type):list_(list),size_(n),acceleration_(acceleration_type)
	{
		if(acceleration_==AUTO&&size_<=5)acceleration_=NONE;//如果物体个数足够少,就不加速
		if(acceleration_type==NONE)return;//不加速就直接返回

		std::sort(list,list+n,[=](intersect *a,intersect *b)->bool{return a->not_optimization()<b->not_optimization();});
		for(num_=0;num_<n;num_++)if(list[num_]->not_optimization()==1)break;

		if(!num_)
		{
			acceleration_=NONE;
			return;
		}
					
		if(acceleration_==AUTO&&num_<=100)acceleration_=BVH;
		else if(acceleration_==AUTO)acceleration_=KDTREE;

		// std::cout<<num_<<std::endl;
		if(acceleration_==BVH)root=new bvh_node(list,num_);
		else if(acceleration_==KDTREE)
		{
			aabb bound=list[0]->get_box();
			for(int i=1;i<num_;i++)bound=aabb(bound,list[i]->get_box());
			root=new KD_tree_node(list,num_,(int)(8+1.3*log2(num_)),bound);
		}
		// std::cerr<<acceleration_<<std::endl;
	}
	bool intersections::hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const
	{
		// std::cerr<<"hhhhhhhh1"<<std::endl;
		hitpoint t_rec;//一个临时的交点消息记录变量
		bool hitSomething=0;//记录是否击中物体
		vec::real far=t_max;//刚开始可以看到最远距离，chapter 2 中的m的作用
		if(acceleration_==NONE)
		{
			for(int i=0;i<size_;i++)if(list_[i]->hit(sight,t_min,far,t_rec))//枚举每一个物体，如果这个物体存在距离大于t_min小于far的交点，则更新far
			{

				// std::cerr<<"hhhhhhhh1"<<std::endl;
				hitSomething=1;
				far=t_rec.t;//将上一次的最近撞击点作为视线可达最远处(更新far)
				rec=t_rec;
			}
		}
		else if(acceleration_==BVH||acceleration_==KDTREE)
		{

			// std::cerr<<"hhhhhhhh2"<<std::endl;
			for(int i=num_;i<size_;i++)if(list_[i]->hit(sight,t_min,t_max,t_rec))
			{
				hitSomething=1;
				far=t_rec.t;//将上一次的最近撞击点作为视线可达最远处
				rec=t_rec;
			}//对不可加速的进行暴力枚举
			
			if(root->hit(sight,t_min,far,t_rec))
			{
				hitSomething=1;
				far=t_rec.t;//将上一次的最近撞击点作为视线可达最远处
				rec=t_rec;
			}//对可加速的进行判断
		}
		return hitSomething;
	}
}