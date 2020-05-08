#include<png.h>
#include<vector>
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<Eigen/Eigen>
#include<Eigen/Dense>

#include<ray_tracing.h>
using namespace Eigen;
using namespace std;
using namespace vec;
using namespace cnum;
using namespace ray_tracing;
int ssp=50;//每像素采样的点数
camera cam;//相机
intersect* world;//场景
const int width=400;//图像水平长度/像素
const int height=200;//图像竖直高度/像素
const int pixel_size=3;//通道数(RGB为3,RGBA为4)
const char *file_name="hello.png";//图片名称
vec3 bitmap_rgb[height][width];//范围为[0,1]
unsigned char bitmap[height][width*pixel_size];


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

vector<intersect*>objects;
int main()
{
	objects.push_back(new sphere(vec3(3,0,0),0.5));
	objects.push_back(new sphere(vec3(10,0,0),5));

	world=new intersections(objects.data(),objects.size());

	for(int i=0;i<height;i++)
	for(int j=0;j<width;j++)
	{
		for(int k=0;k<ssp;k++)
		{
			ray sight=cam.get_ray((vec::real)(i+rnd::rand())/height/*将坐标归一化*/,(vec::real)(j+rnd::rand())/width/*将坐标归一化*/);
			bitmap_rgb[i][j]+=get_color(sight,world);
		}
		bitmap_rgb[i][j]=bitmap_rgb[i][j]/(vec::real)ssp;
	}
	write_PNG();
	system(file_name);
	return 0;
}