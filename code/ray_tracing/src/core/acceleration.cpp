#include<core.h>
#include<algorithm>

namespace ray_tracing
{
	aabb::aabb()
	{
		min_=vec::vec3(-cnum::INF,-cnum::INF,-cnum::INF);
		max_=vec::vec3(cnum::INF,cnum::INF,cnum::INF);
	}
	aabb::aabb(const vec::vec3 &a,const vec::vec3 &b)
	{
		min_=vec::vec3(std::min(a.x(),b.x())-cnum::eps,std::min(a.y(),b.y())-cnum::eps,std::min(a.z(),b.z())-cnum::eps);
		max_=vec::vec3(std::max(a.x(),b.x())+cnum::eps,std::max(a.y(),b.y())+cnum::eps,std::max(a.z(),b.z())+cnum::eps);
		vec::vec3 len=max_-min_;
		area_=(len.x()*len.y()+len.y()*len.z()+len.x()*len.z())*2;
		// std::cout<<"max:"<<min_.x()<<" "<<min_.y()<<" "<<min_.z()<<std::endl;
		// std::cout<<"min:"<<max_.x()<<" "<<max_.y()<<" "<<max_.z()<<std::endl;
	}
	aabb::aabb(const aabb &a,const aabb &b)
	{
		min_=vec::vec3(std::min(a.min().x(),b.min().x())-cnum::eps,std::min(a.min().y(),b.min().y())-cnum::eps,std::min(a.min().z(),b.min().z())-cnum::eps);
		max_=vec::vec3(std::max(a.max().x(),b.max().x())+cnum::eps,std::max(a.max().y(),b.max().y())+cnum::eps,std::max(a.max().z(),b.max().z())+cnum::eps);
		vec::vec3 len=max_-min_;
		area_=(len.x()*len.y()+len.y()*len.z()+len.x()*len.z())*2;
		// std::cout<<"max:"<<min_.x()<<" "<<min_.y()<<" "<<min_.z()<<std::endl;
		// std::cout<<"min:"<<max_.x()<<" "<<max_.y()<<" "<<max_.z()<<std::endl;
	}
	void aabb::merge(const aabb &a)
	{
		min_=vec::vec3(std::min(a.min().x(),min_.x())-cnum::eps,std::min(a.min().y(),min_.y())-cnum::eps,std::min(a.min().z(),min_.z())-cnum::eps);
		max_=vec::vec3(std::max(a.max().x(),max_.x())+cnum::eps,std::max(a.max().y(),max_.y())+cnum::eps,std::max(a.max().z(),max_.z())+cnum::eps);
		vec::vec3 len=max_-min_;
		area_=(len.x()*len.y()+len.y()*len.z()+len.x()*len.z())*2;
	}
	bool aabb::hit(const ray &sight,vec::real tmin,vec::real tmax)const
	{
		// std::cout<<"minmax_:"<<tmin<<" "<<tmax<<std::endl;
		// std::cout<<sight.origin().x()<<" "<<sight.origin().y()<<" "<<sight.origin().z()<<std::endl;
		// std::cout<<sight.direction().x()<<" "<<sight.direction().y()<<" "<<sight.direction().z()<<std::endl;
		// std::cout<<min_.x()<<" "<<min_.y()<<" "<<min_.z()<<std::endl;
		// std::cout<<max_.x()<<" "<<max_.y()<<" "<<max_.z()<<std::endl;
		for(int i=0;i<3;i++)if(sight.direction()[i]!=0.)
		{
			vec::real t1=(min_[i]-sight.origin()[i])/sight.direction()[i],t2=(max_[i]-sight.origin()[i])/sight.direction()[i];
			if(sight.direction()[i]<0)std::swap(t1,t2);
			// std::cout<<"t:"<<t1<<" "<<t2<<std::endl;
			if((tmax=std::min(t2,tmax))<(tmin=std::max(t1,tmin)))return 0;
		}
		else if(sight.origin()[i]>max_[i]||sight.origin()[i]<min_[i]||sight.origin()[i]>tmax||sight.origin()[i]<tmin)return 0;
		// std::cout<<"hit"<<std::endl<<std::endl;
		return 1;
	}
	const vec::vec3& aabb::min()const{return min_;}
	const vec::vec3& aabb::max()const{return max_;}
	const vec::real& aabb::area()const{return area_;}
}

