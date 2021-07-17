#include<objects/sphere.h>
ray_tracing::sphere::sphere(const vec::vec3 &o,const vec::real &r):o_(o),r_(r){}
const vec::real& ray_tracing::sphere::r()const{return r_;}
const vec::vec3& ray_tracing::sphere::o()const{return o_;}
bool ray_tracing::sphere::hit(const ray_tracing::ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const
{
	vec::vec3 trace=sight.origin()-o_;//公式中的s-p_0
	vec::real a=sight.direction().dot(sight.direction()),b=2.*trace.dot(sight.direction()),c=trace.dot(trace)-r_*r_,delt=b*b-4.*a*c;//公式中的各项系数与delta
	if(delt>0)//如果delta大于0才有交点
	{
		vec::real x=(-b-sqrt(delt))/(2.*a);//计算较小的那个t的值
		if(x<t_max&&x>t_min)//如果这个t满足条件
		{
			rec.t=x;//记录下t的值
			rec.p=sight.go(rec.t);//根据t的值算出交点
			rec.n=(rec.p-o_)/r_;//求出法向量，记得单位化（除以半径）
			return 1;//返回击中了
		}

		//如果较小的根不符合条件，则计算较大的根
		x=(-b+sqrt(delt))/(2.*a);
		if(x<t_max&&x>t_min)
		{
			rec.t=x;//记录下t的值
			rec.p=sight.go(rec.t);//根据t的值算出交点
			rec.n=(rec.p-o_)/r_;//求出法向量，记得单位化（除以半径）
			return 1;//返回击中了
		}
	}
	return 0;//如果没有交点或交点不符合条件，这返回没有击中
}