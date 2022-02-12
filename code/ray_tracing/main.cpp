#include<png.h>
#include<ctime>
#include<mutex>
#include<thread>
#include<vector>
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<Eigen/Eigen>
#include<Eigen/Dense>
// #include <GLFW\glfw3.h>

#include<ray_tracing.h>
using namespace Eigen;
using namespace std;
using namespace vec;
using namespace cnum;
using namespace my_thread_pool;
using namespace ray_tracing;
int spp=50;//每像素采样的点数
intersect* world;//场景
const int width=320;//图像水平长度/像素
const int height=180;//图像竖直高度/像素
//将图像分块,每个线程每次处理一块
const int thread_num=20;
const int tile_w=16;//块的水平长度/像素
const int tile_h=16;//块的竖直高度/像素
// const int tile_w_num=(width-1)/tile_w;//块在竖直方向的个数(减一是因为从0开始计算)
// const int tile_h_num=(height-1)/tile_h;//块在水平方向的个数(减一是因为从0开始计算)
const int pixel_size=3;//通道数(RGB为3,RGBA为4)
const char *file_name="hello.png";//图片名称
vec3 bitmap_rgb[height][width];//范围为[0,1]
unsigned char bitmap[height][width*pixel_size];

camera cam(90,(vec::real)width/(vec::real)height,vec3(0,0,0),vec3(0,0,1),vec3(-6,-6,-6));//相机,参数分别为视角,长宽比,照相机正对的点的坐标,照相机坐标中的y轴,照相机位置,焦距


void write_PNG()//输出图像
{
	png_structp png_ptr=png_create_write_struct(PNG_LIBPNG_VER_STRING,nullptr,nullptr,nullptr);
	png_infop info_ptr=png_create_info_struct(png_ptr);
	FILE* f=fopen(file_name,"wb");
	png_init_io(png_ptr,f);
	png_set_IHDR(png_ptr,info_ptr,width,height,8,pixel_size==3?PNG_COLOR_TYPE_RGB:PNG_COLOR_TYPE_RGBA,PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
	png_set_packing(png_ptr);
	png_write_info(png_ptr,info_ptr);

	for(int i=0;i<height;i++)
	for(int j=0;j<width;j++)
	for(int k=0;k<pixel_size;k++)bitmap[i][j*pixel_size+k]=(int)(bitmap_rgb[i][j][k]*255);

	png_bytepp rows=(png_bytepp)png_malloc(png_ptr,height*sizeof(png_bytep));
	for(int i=0;i<height;i++)rows[i]=(png_bytep)bitmap[i];

	png_write_image(png_ptr,rows);
	png_write_end(png_ptr,info_ptr);
	png_free(png_ptr,rows);//cleanup
	png_destroy_write_struct(&png_ptr,&info_ptr);
	fclose(f);
}

vec3 get_color(const ray &sight,const intersect *world)
{
	hitpoint rec;
	if(world->hit(sight,0,INF,rec))//求交点，交点的范围为零到正无穷
	{
		vec3 tmp=0.5*(rec.n+vec3(1,1,1));
		// vec3 tmp(rec.n.x()>0,rec.n.y()>0,rec.n.z()>0);
		return tmp;
	}
	else return vec3(1,1,1);
}
void render(int x1,int y1,int x2,int y2)//渲染范围为[x1,x2),[y1,y2)的像素
{
	// printf("hello%d %d %d %d %d\n",x1,y1,x2,y2,std::this_thread::get_id());
	for(int i=x1;i<x2;i++)
	for(int j=y1;j<y2;j++)
	{
		for(int k=0;k<spp;k++)
		{
			ray sight=cam.get_ray((vec::real)(i+rnd::rand())/height/*将坐标归一化*/,(vec::real)(j+rnd::rand())/width/*将坐标归一化*/);
			bitmap_rgb[i][j]+=get_color(sight,world);
		}
		bitmap_rgb[i][j]=bitmap_rgb[i][j]/(vec::real)spp;
	}

}
vector<intersect*>objects;
int main()
{
	clock_t start=clock(),finish;
	// objects.push_back(new sphere(vec3(3,0,0),0.5));
	// objects.push_back(new sphere(vec3(10,0,0),5));
	for(int i=-5;i<=5;i++)
	for(int j=-5;j<=5;j++)
	for(int k=-5;k<=5;k++)objects.push_back(new sphere(vec3(i,j,k),0.3));
	for(int i=0;i<1000;i++)objects.push_back(new sphere(vec3(0,0,0),1000+i));
	// for(int i=0;i<objects.size();i++)cout<<objects[i]<<" ";
	// cout<<endl;

	world=new intersections(objects.data(),objects.size(),NONE);

	thread_pool<int,int,int,int> my_thread_pool_(thread_num,render);
	for(int i=0;i<height;i+=tile_h)
	for(int j=0;j<width;j+=tile_w)my_thread_pool_.push_task(i,j,min(i+tile_h,height),min(j+tile_w,width));
	// int cnt=0;
	// for(int i=0;i<1331;i++)
	// {
	// 	// int cnt=0;
	// 	cout<<objects[i]->vis<<" ";
	// 	if(!objects[i]->vis)cnt++;
	// }
	// cout<<cnt<<endl;
	mutex mx;
	unique_lock<mutex> lk(mx);
	while(!my_thread_pool_.queue_empty())my_thread_pool_.queue_empty_msg.wait(lk);
	lk.unlock();
	cout<<"finish"<<endl;

	// for(int i=0;i<height;i++)
	// for(int j=0;j<width;j++)
	// // int i=133,j=82;
	// {
	// 	// cout<<i<<" "<<j<<endl;
	// 	for(int k=0;k<spp;k++)
	// 	{
	// 		ray sight=cam.get_ray((vec::real)(i+rnd::rand())/height/*将坐标归一化*/,(vec::real)(j+rnd::rand())/width/*将坐标归一化*/);
	// 		bitmap_rgb[i][j]+=get_color(sight,world);
	// 	}
	// 	bitmap_rgb[i][j]=bitmap_rgb[i][j]/(vec::real)spp;
	// }

	write_PNG();
	system(file_name);

	finish=clock();
	cout<<"time="<<double(finish-start)/CLOCKS_PER_SEC<<"s"<<endl;
	return 0;
}