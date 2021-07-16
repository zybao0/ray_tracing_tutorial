#include<core.h>

ray_tracing::ray::ray(const vec::vec3 &origin,const vec::vec3 &direction):origin_(origin),direction_(direction.normalized()){}
ray_tracing::ray::ray(const ray &r):origin_(r.origin()),direction_(r.direction()){}
const vec::vec3& ray_tracing::ray::origin()const{return origin_;}
const vec::vec3& ray_tracing::ray::direction()const{return direction_;}
vec::vec3 ray_tracing::ray::go(const vec::real &t)const{return origin_+direction_*t;}

ray_tracing::camera::camera(vec::real vfov,vec::real scale,vec::vec3 look_at,vec::vec3 camera_up,vec::vec3 look_from,vec::real dis):look_from_(look_from),look_at_(look_at),f_(dis)
{
	camera_z_=(look_from-look_at).normalized();//为了保证右手系，z轴与照相机方向相反
	camera_x_=camera_up.cross(camera_z_).normalized();
	camera_y_=camera_z_.cross(camera_x_).normalized();
	h_=2*f_*tan(vfov/360.*cnum::pi);
	w_=h_*scale;
}
ray_tracing::ray ray_tracing::camera::get_ray(vec::real u,vec::real v){return ray(look_from_,(0.5-v)*w_*camera_x_+(0.5-u)*h_*camera_y_-f_*camera_z_);}
ray_tracing::ray ray_tracing::camera::get_ray(vec::vec2 p){return ray(look_from_,(0.5-p.y())*w_*camera_x_+(0.5-p.x())*h_*camera_y_-f_*camera_z_);}

ray_tracing::intersections::intersections(ray_tracing::intersect** list,size_t n):list_(list),size_(n){}
bool ray_tracing::intersections::hit(const ray_tracing::ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const
{
	ray_tracing::hitpoint t_rec;//一个临时的交点消息记录变量
	bool hitSomething=0;//记录是否击中物体
	vec::real far=t_max;//刚开始可以看到最远距离，chapter 2 中的m的作用
	for(int i=0;i<size_;i++)if(list_[i]->hit(sight,t_min,far,t_rec))//枚举每一个物体，如果这个物体存在距离大于t_min小于far的交点，则更新far
	{
		hitSomething=1;
		far=t_rec.t;//将上一次的最近撞击点作为视线可达最远处(更新far)
		rec=t_rec;
	}
	return hitSomething;
}