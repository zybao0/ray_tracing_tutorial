#include<core.h>
#include<algorithm>
#include<iostream>

namespace ray_tracing
{
	aabb::aabb()
	{
		min_=vec::vec3(-cnum::INF,-cnum::INF,-cnum::INF);
		max_=vec::vec3(cnum::INF,cnum::INF,cnum::INF);
	}
	aabb::aabb(const vec::vec3 &a,const vec::vec3 &b)
	{
		min_=vec::vec3(std::min(a.x(),b.x()),std::min(a.y(),b.y()),std::min(a.z(),b.z()));
		max_=vec::vec3(std::max(a.x(),b.x()),std::max(a.y(),b.y()),std::max(a.z(),b.z()));
		vec::vec3 len=max_-min_;
		area_=(len.x()*len.y()+len.y()*len.z()+len.x()*len.z())*2;
		// std::cout<<"max:"<<min_.x()<<" "<<min_.y()<<" "<<min_.z()<<std::endl;
		// std::cout<<"min:"<<max_.x()<<" "<<max_.y()<<" "<<max_.z()<<std::endl;
	}
	aabb::aabb(const aabb &a,const aabb &b)
	{
		min_=vec::vec3(std::min(a.min().x(),b.min().x()),std::min(a.min().y(),b.min().y()),std::min(a.min().z(),b.min().z()));
		max_=vec::vec3(std::max(a.max().x(),b.max().x()),std::max(a.max().y(),b.max().y()),std::max(a.max().z(),b.max().z()));
		vec::vec3 len=max_-min_;
		area_=(len.x()*len.y()+len.y()*len.z()+len.x()*len.z())*2;
		// std::cout<<"max:"<<min_.x()<<" "<<min_.y()<<" "<<min_.z()<<std::endl;
		// std::cout<<"min:"<<max_.x()<<" "<<max_.y()<<" "<<max_.z()<<std::endl;
	}
	void aabb::merge(const aabb &a)
	{
		min_=vec::vec3(std::min(a.min().x(),min_.x()),std::min(a.min().y(),min_.y()),std::min(a.min().z(),min_.z()));
		max_=vec::vec3(std::max(a.max().x(),max_.x()),std::max(a.max().y(),max_.y()),std::max(a.max().z(),max_.z()));
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


	int get_max_asix(intersect **world,const int n)//获得极差最大的那根轴
	{
		aabb worldbox_(world[0]->get_box());
		for(int i=1;i<n;i++)worldbox_.merge(world[i]->get_box());
		vec::vec3 tmp=worldbox_.max()-worldbox_.min();
		if((tmp.x()+cnum::eps>tmp.y())&&(tmp.x()+cnum::eps>tmp.z()))return AXIS_X;
		else if((tmp.y()+cnum::eps>tmp.x())&&(tmp.y()+cnum::eps>tmp.z()))return AXIS_Y;
		else return AXIS_Z;
	}	

	bvh_node::bvh_node(intersect** world,const int n)
	{
		// for(int i=0;i<n;i++)std::cout<<world[i]<<" ";
		// std::cout<<std::endl;
		if(n==1)left_=world[0],box_=world[0]->get_box(),right_=0;
		else
		{
			int axis=get_max_asix(world,n);
			std::sort(world,world+n,[=](intersect *a,intersect *b)->bool{return a->get_box().min()[axis]<b->get_box().min()[axis];});//将每个包围盒左边界排序

			// std::cout<<n<<std::endl;

			int div=0;
			vec::real min_cost=cnum::INF;
			aabb aabb_left(world[0]->get_box());//左节点的包围盒
			aabb *aabb_right=new aabb[n];//右节点的包围盒

			aabb_right[n-1]=world[n-1]->get_box();
			for(int i=n-2;i>=0;i--)aabb_right[i]=aabb(world[i]->get_box(),aabb_right[i+1]);
			for(int i=0;i<n-1;i++)//在i和i+1之间划分左右子树
			{
				aabb_left=aabb(aabb_left,world[i]->get_box());//在第i个物体划分时左边的包围盒
				// vec::real now_cost=(i+1)*aabb_left.area()/aabb_right[0].area()+(n-i-1)*aabb_right[i+1].area()/aabb_right[0].area()+0.125;
				//cost(A,B)=S(A)/S(C)*N(A)+S(B)/S(C)*N(B)+t_trav,但S(C)和t_trav相同,可以忽略(吧)
				vec::real now_cost=(i+1)*aabb_left.area()+(n-i-1)*aabb_right[i+1].area();//计算当前代价
				// std::cout<<now_cost<<std::endl;
				if(now_cost<min_cost){min_cost=now_cost;div=i;}
			}//我所理解的SAH(Surface Area Heoristic)

			delete aabb_right;

			// std::cout<<div<<std::endl;
			
			left_=new bvh_node(world,div+1);
			right_=new bvh_node(world+div+1,n-div-1);
			// std::cerr<<"tree:"<<this<<" "<<left_<<" "<<right_<<std::endl;
			box_=aabb(left_->get_box(),right_->get_box());
		}
	}
	bool bvh_node::hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const
	{
		// std::cerr<<"hello"<<std::endl;
		if(!box_.hit(sight,t_min,t_max))return 0;
		// std::cerr<<"hit:"<<this<<std::endl; 
		hitpoint linfo,rinfo;
		bool lhit=0,rhit=0;
		lhit=left_->hit(sight,t_min,t_max,linfo);//似乎左边一定有儿子，不用判断
		if(lhit)t_max=linfo.t,rec=linfo;//如果左边有交点，则将最远距离设置为交点
		if(right_)rhit=right_->hit(sight,t_min,t_max,rinfo);//与右子树求交点，因为左右子树有可能有重叠，因此右子树一定要求
		if(!lhit&&!rhit)return 0;
		else if(rhit)rec=rinfo;
		if(rhit&&lhit&&rinfo.t<linfo.t)rec=rinfo;//比较左右交点哪个更优
		return 1;
	}

	void sort_asix(aabb box,int *f)//对每根轴按极差从小到大排序
	{
		int tmp[3]={0,1,2};
		std::sort(tmp,tmp+3,[=](int u,int v)->bool{return box.max()[u]-box.min()[u]>box.max()[v]-box.min()[v];});
		for(int i=0;i<3;i++)f[i]=tmp[i];
	}
	KD_tree_node::KD_tree_node(intersect**world,const int n,const int depth,aabb bound)
	{
		// std::cout<<n<<" "<<depth<<std::endl;
		// std::cout<<"min:"<<bound.min().x()<<" "<<bound.min().y()<<" "<<bound.min().z()<<" "<<std::endl;
		// std::cout<<"max:"<<bound.max().x()<<" "<<bound.max().y()<<" "<<bound.max().z()<<" "<<std::endl;
		box_=bound;
		intersect **objects=new intersect*[n];//objects in the bound
		vec::real **bound_list=new vec::real*[6];
		int num=0;
		for(int i=0;i<n;i++)
		{
			bool flag=1;
			for(int j=0;flag&&j<3;j++)if(bound.min()[j]>=world[i]->get_box().max()[j]||bound.max()[j]<=world[i]->get_box().min()[j])flag=0;//判断这个物体是不是在当前划分出来的空间中
			// if(flag)for(int j=0;flag&&(j<3);j++)//if(bound.min()[j]>=world[i]->get_box().max()[j]-cnum::eps||bound.max()[j]<=world[i]->get_box().min()[j]+cnum::eps)
			// {
			// 	// std::cout<<"bound_min:"<<bound.min()[j]<<std::endl;
			// 	// std::cout<<"world_max:"<<world[i]->get_box().max()[j]-cnum::eps<<std::endl;
			// 	// std::cout<<(bound.min()[j]>=world[i]->get_box().max()[j]-cnum::eps)<<std::endl;
			// 	// std::cout<<"bound_max:"<<bound.max()[j]<<std::endl;
			// 	// std::cout<<"world_min:"<<world[i]->get_box().min()[j]+cnum::eps<<std::endl;
			// 	// std::cout<<(bound.max()[j]<=world[i]->get_box().min()[j]+cnum::eps)<<std::endl;
			// 	// flag=0;//判断这个物体是不是在当前划分出来的空间中	
			// }
			if(flag)objects[num++]=world[i];
			// {
			// 	std::cout<<"min1:"<<world[i]->get_box().min().x()<<" "<<world[i]->get_box().min().y()<<" "<<world[i]->get_box().min().z()<<" "<<std::endl;
			// 	std::cout<<"max1:"<<world[i]->get_box().max().x()<<" "<<world[i]->get_box().max().y()<<" "<<world[i]->get_box().max().z()<<" "<<std::endl;
			// 	objects[num++]=world[i];
			// }
		}
		// std::cout<<"num:"<<num<<std::endl;
		if(!num)return;
		if(num==1)//如果当前划分出来的格子中只有一个物体
		{
			split=0;
			left_=new KD_tree_leaf(objects,num,bound);
			right_=0;
		}
		else
		{
			for(int i=0;i<6;i++)bound_list[i]=new vec::real[num];
			for(int i=0;i<3;i++)
			for(int j=0;j<num;j++)
			{
				bound_list[i*2][j]=std::max(objects[j]->get_box().min()[i],bound.min()[i]);//每个物体的左端点
				bound_list[i*2+1][j]=std::min(objects[j]->get_box().max()[i],bound.max()[i]);//每个物体的右端点
			}
			int f[3];
			sort_asix(bound,f);
			vec::real min_cost=cnum::INF;
			int split_size_l=0,split_size_r=num;
			split=0;
			// std::cout<<"oKKKKKKKKKKK1111111111"<<std::endl;
			for(int i=0;i<3;i++)//f[i]为当前的轴
			{
				split_pos_=bound.min()[f[i]]-cnum::eps;
				// std::cout<<i<<std::endl;
				std::sort(bound_list[f[i]*2],bound_list[f[i]*2]+num);
				std::sort(bound_list[f[i]*2+1],bound_list[f[i]*2+1]+num);
				int p=0,q=0,size_l=0,size_r=num;
				// std::cout<<"oKKKKKKKKKK222222222"<<std::endl;
				while(p<num)
				//枚举每个物体aabb的边界作为切割点
				//类似于归并排序遍历 由于物体左端点最大值一定小于物体右端点 p一定先遍历完
				{
					// std::cerr<<"p:"<<p<<" "<<bound_list[f[i]*2][p]<<std::endl;
					// std::cerr<<"q:"<<q<<" "<<bound_list[f[i]*2+1][q]<<std::endl;
					bool flag=bound_list[f[i]*2][p]<bound_list[f[i]*2+1][q];
					// if(!flag&&q&&bound_list[f[i]*2+1][q]==bound_list[f[i]*2+1][q-1])//20210616：疑似有误，if(!flag&&q<num-1&&bound_list[f[i]*2+1][q]==bound_list[f[i]*2+1][q+1])，待验证
					while((!flag)&&q<num-1&&bound_list[f[i]*2+1][q]>=bound_list[f[i]*2+1][q+1]-cnum::eps)//将bound_list[f[i]*2+1][q]作为切割点,物体右边界大于(不含等于)它的作为右子树
					{
						size_r--;
						q++;
						// break;//20210616:疑似有误，应为continue，待验证
					}
					if(!flag)size_r--;

					// std::cout<<"111111111111111111"<<std::endl;
					
					vec::vec3 tmp_min=bound.min(),tmp_max=bound.max();
					tmp_min[f[i]]=tmp_max[f[i]]=flag?bound_list[f[i]*2][p]:bound_list[f[i]*2+1][q];
					aabb lbox_(bound.min(),tmp_max),rbox_(tmp_min,bound.max());
					//仅当两个待划分节点中的任意一个为空节点时增加一个奖励项 
					//vec::real now_cost=(p==0&&(bound_list[f[i]*2][p]>bound.min()[f[i]]+cnum::eps)?(bound.max()[f[i]]-bound_list[f[i]*2][p])/(bound.max()[f[i]]-bound.min()[f[i]]):1.)*(size_l*lbox_.area()+size_r*rbox_.area());
					// std::cout<<now_cost<<std::endl;
					vec::real now_cost=0;
					if(p)now_cost=size_l*lbox_.area()+size_r*rbox_.area();
					else if(bound_list[f[i]*2][p]>bound.min()[f[i]]+cnum::eps)(bound.max()[f[i]]-bound_list[f[i]*2][p])/(bound.max()[f[i]]-bound.min()[f[i]])*size_l*lbox_.area()+size_r*rbox_.area();
					else now_cost=log2(depth)*(size_l*lbox_.area()+size_r*rbox_.area());
					if(now_cost<min_cost)
					{
						min_cost=now_cost;
						split_pos_=flag?bound_list[f[i]*2][p]:bound_list[f[i]*2+1][q];
						split_axis_=f[i];
						split_size_l=size_l;
						split_size_r=size_r;
						// std::cout<<size_l<<" "<<size_r<<" "<<split_pos_<<std::endl;
					}
					// std::cout<<"33333333333333333333333"<<std::endl;
					while(flag&&p<num-1&&bound_list[f[i]*2][p]>=bound_list[f[i]*2][p+1]-cnum::eps)//将bound_list[f[i]*2][p]作为切割点,物体左边界小于(不含等于)它的作为左子树
					{
						size_l++;
						p++;
						// break;//20210616:疑似有误，应为continue，待验证
					}
					// std::cout<<"2222222222222222222222"<<std::endl;
					if(flag)size_l++;
					// flag?size_l++:size_r--;
					flag?p++:q++;
				}
				// std::cout<<"oKKKKKKKKKKK33333333333333333"<<std::endl;
				while(q<num)
				{
					/*20210616:疑似有误，应为
					if(!flag&&q<num-1&&bound_list[f[i]*2+1][q]==bound_list[f[i]*2+1][q+1])，待验证
					{
						size_r--;
						q++;
						continue;
					}
					待验证*/
					while(q<num-1&&bound_list[f[i]*2+1][q]>=bound_list[f[i]*2+1][q+1]-cnum::eps)//将bound_list[f[i]*2+1][q]作为切割点,物体右边界大于(不含等于)它的作为右子树
					{
						size_r--;
						q++;
						// break;//20210616:疑似有误，应为continue，待验证
					}
					size_r--;
					vec::vec3 tmp_min=bound.min(),tmp_max=bound.max();
					tmp_min[f[i]]=tmp_max[f[i]]=bound_list[f[i]*2+1][q];
					aabb lbox_(bound.min(),tmp_max),rbox_(tmp_min,bound.max());
					// vec::real now_cost=(q==num-1&&(bound_list[f[i]*2+1][1]<bound.min()[f[i]]-cnum::eps)?(bound_list[f[i]*2+1][q]-bound.min()[f[i]])/(bound.max()[f[i]]-bound.min()[f[i]]):1.)*(size_l*lbox_.area()+size_r*rbox_.area());
					vec::real now_cost=0;
					if(p)now_cost=size_l*lbox_.area()+size_r*rbox_.area();
					else if(bound_list[f[i]*2][p]>bound.min()[f[i]]+cnum::eps)(bound.max()[f[i]]-bound_list[f[i]*2][p])/(bound.max()[f[i]]-bound.min()[f[i]])*size_l*lbox_.area()+size_r*rbox_.area();
					else now_cost=log2(depth)*(size_l*lbox_.area()+size_r*rbox_.area());
					if(now_cost<min_cost)
					{
						min_cost=now_cost;
						split_pos_=bound_list[f[i]*2+1][q];
						split_axis_=f[i];
						split_size_l=size_l;
						split_size_r=size_r;
						// std::cout<<size_l<<" "<<size_r<<std::endl;
						// std::cout<<size_l<<" "<<size_r<<" "<<split_pos_<<std::endl;
					}
					q++;
				}
				//if(split_pos_>bound_list[f[i]*2][0]+eps&&split_pos_<bound_list[f[i]*2+1][num-1]-eps)
				if(split_pos_>bound.min()[f[i]]+cnum::eps&&split_pos_<bound.max()[f[i]]-cnum::eps)
				{

					split=1;
					break;
				}
			}
			// std::cout<<bound.min().x()<<" "<<bound.min().y()<<" "<<bound.min().z()<<" "<<std::endl;
			// std::cout<<bound.max().x()<<" "<<bound.max().y()<<" "<<bound.max().z()<<" "<<std::endl;
			// std::cout<<"split: "<<depth<<" "<<split<<" "<<split_axis_<<" "<<split_pos_<<std::endl<<std::endl;

			if(!split)
			{
				// std::cout<<"1"<<std::endl;
				left_=new KD_tree_leaf(objects,num,bound);
				right_=0;
			}
			else if(!depth)
			{
				// std::cout<<"2"<<std::endl;
				int left_num=0,right_num=0;
				intersect **left_objects=new intersect*[num],**right_objects=new intersect*[num];
				for(int i=0;i<num;i++)
				{
					if(objects[i]->get_box().min()[split_axis_]<=split_pos_+cnum::eps)left_objects[left_num++]=objects[i];
					if(objects[i]->get_box().max()[split_axis_]>=split_pos_-cnum::eps)right_objects[right_num++]=objects[i];
				}
				vec::vec3 tmp_min=bound.min(),tmp_max=bound.max();
				tmp_min[split_axis_]=tmp_max[split_axis_]=split_pos_;
				aabb lbox_(bound.min(),tmp_max),rbox_(tmp_min,bound.max());
				if(left_num)left_=new KD_tree_leaf(left_objects,left_num,lbox_);
				if(right_num)right_=new KD_tree_leaf(right_objects,right_num,rbox_);
				delete []left_objects;
				delete []right_objects;
			}
			else 
			{
				// std::cout<<"3"<<std::endl;
				vec::vec3 tmp_min=bound.min(),tmp_max=bound.max();
				tmp_min[split_axis_]=tmp_max[split_axis_]=split_pos_;
				aabb lbox_(bound.min(),tmp_max),rbox_(tmp_min,bound.max());
				// std::cout<<split_size_l<<" "<<split_size_r<<std::endl<<std::endl;
				if(split_size_l&&(lbox_.max()-lbox_.min()).minCoeff()>2*cnum::eps)left_=new KD_tree_node(world,n,depth-1,lbox_);
				if(split_size_r&&(rbox_.max()-rbox_.min()).minCoeff()>2*cnum::eps)right_=new KD_tree_node(world,n,depth-1,rbox_);
			}
			// std::cout<<"OOOOK"<<std::endl;
			for(int i=0;i<6;i++)delete []bound_list[i];
		}
		delete []bound_list;
		delete []objects;
	}
	bool KD_tree_node::hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const
	{
		// std::cout<<"light"<<sight.origin()[split_axis_]<<" "<<sight.direction()[split_axis_]<<std::endl;
		// std::cout<<"split:"<<split_axis_<<" "<<split_pos_<<" "<<split<<std::endl;
		// std::cout<<box_.min().x()<<" "<<box_.min().y()<<" "<<box_.min().z()<<" "<<std::endl;
		// std::cout<<box_.max().x()<<" "<<box_.max().y()<<" "<<box_.max().z()<<"\n"<<std::endl;
		if(!box_.hit(sight,t_min,t_max))return 0;
		// std::cout<<"11111111"<<std::endl;
		if(!split)return left_->hit(sight,t_min,t_max,rec);
		// std::cout<<"22222222"<<std::endl;
		if(right_&&sight.direction()[split_axis_]>=0&&sight.origin()[split_axis_]>=split_pos_)return right_->hit(sight,t_min,t_max,rec);
		if(left_&&sight.direction()[split_axis_]<=0&&sight.origin()[split_axis_]<=split_pos_)return left_->hit(sight,t_min,t_max,rec);
		// std::cout<<"33333333"<<std::endl;
		vec::real t_hit_split=(split_pos_-sight.origin()[split_axis_])/sight.direction()[split_axis_];
		// std::cout<<"t_hit:"<<t_hit_split<<std::endl;
		if(sight.direction()[split_axis_]>0&&sight.origin()[split_axis_]<split_pos_)
		{
			// std::cout<<"444444444"<<std::endl;
			if(left_&&left_->hit(sight,t_min,std::min(t_max,t_hit_split),rec))return 1;
			// std::cout<<"change1"<<std::endl;
			// std::cout<<"555555555"<<std::endl;
			if(right_)return right_->hit(sight,std::max(t_min,t_hit_split),t_max,rec);
		}
		if(sight.direction()[split_axis_]<0&&sight.origin()[split_axis_]>split_pos_)
		{
			// std::cout<<"666666666"<<std::endl;
			if(right_&&right_->hit(sight,t_min,std::min(t_max,t_hit_split),rec))return 1;
			// std::cout<<"777777777"<<std::endl;
			// std::cout<<"change2"<<std::endl;
			if(left_)return left_->hit(sight,std::max(t_min,t_hit_split),t_max,rec);
		}
		return 0;
	}

	KD_tree_leaf::KD_tree_leaf(intersect **world,const int n,aabb bound):size_(n)
	{
		
		box_=bound;
		list_=new intersect*[n];
		for(int i=0;i<n;i++)list_[i]=world[i];
		// for(int i=0;i<n;i++)list_[i]->vis=1;
		// for(int i=0;i<n;i++)std::cout<<world[i]<<" ";
		// std::cout<<std::endl;
		// std::cout<<box_.min().x()<<" "<<box_.min().y()<<" "<<box_.min().z()<<" "<<std::endl;
		// std::cout<<box_.max().x()<<" "<<box_.max().y()<<" "<<box_.max().z()<<" "<<std::endl;
		// for(int i=0;i<n;i++)
		// {
		// 	std::cout<<"OBJECT:"<<i<<"\n";
		// 	std::cout<<list_[i]->get_box().min().x()<<" "<<list_[i]->get_box().min().y()<<" "<<list_[i]->get_box().min().z()<<" "<<std::endl;
		// 	std::cout<<list_[i]->get_box().max().x()<<" "<<list_[i]->get_box().max().y()<<" "<<list_[i]->get_box().max().z()<<" "<<std::endl;
		// }
		// std::cout<<"\n";
	}
	bool KD_tree_leaf::hit(const ray &sight,vec::real t_min,vec::real t_max,hitpoint &rec)const
	{

		// std::cout<<box_.min().x()<<" "<<box_.min().y()<<" "<<box_.min().z()<<" "<<std::endl;
		// std::cout<<box_.max().x()<<" "<<box_.max().y()<<" "<<box_.max().z()<<"\n"<<std::endl;
		// for(int i=0;i<size_;i++)std::cout<<list_[i]<<" ";
		// std::cout<<std::endl;
		if(!box_.hit(sight,t_min,t_max))return 0;
		hitpoint t_rec;
		bool hitSomething=0;
		vec::real far=t_max;//刚开始可以看到无限远
		for(int i=0;i<size_;i++)if(list_[i]->hit(sight,t_min,far,t_rec))
		{
			hitSomething=1;
			far=t_rec.t;//将上一次的最近撞击点作为视线可达最远处
			rec=t_rec;
		}
		return hitSomething;
	}
}

