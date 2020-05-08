#光线追踪——从入门到放弃(2)：画个球

## 引言
今天，我们尝试在场景中添加一个球

## 如何画一个物体
我们回忆一下上一节的内容，我们就知道，我们的物体应该提供至少3个功能：

1. 判断是否被光线击中（与光线求交点）
2. 提供表面任意一点的法线
3. 提供材质，颜色等与光线交互的信息

对于第一点，我不用解释太多，我们光线追踪的一切都是基于我们的光线与物体相交，判断是否有交点必不可少。当光线击中物体之后，会进行反射与折射，而反射与折射都是拿法向量来做文章的（如反射角=入射角等），因此得到光线与物体表面法向量是求出反射与折射光线的基础。求出了法向量之后，我们还需要进一步知道光线到底是在物体表面到底是进行反射还是折射，如果是折射，那是漫反射还是镜面反射；反射（折射）光线方向是什么；反射（折射）后光线的颜色究竟如何变化，这些都属于物体材质的范畴，这也是我们接下来的几篇文章的重点，而今天，我们主要搞定第1，2点内容。

## 开始画一个球

### 获得交点

我们在小学二年级就学过，我们要在坐标系中表示一个球，我们可以使用求面的方程<img src="https://www.zhihu.com/equation?tex=(x-x_0)^2+(y-y_0)^2+(z-z_0)^2=R^2" alt="[公式]" eeimg="1" data-formula="(x-x_0)^2+(y-y_0)^2+(z-z_0)^2=R^2">。当然我们可以使用向量更加简洁和方便地表示：<img src="https://www.zhihu.com/equation?tex=\left\|\vec{P_0P}\right\|=R" alt="[公式]" eeimg="1" data-formula="\left\|\vec{P_0P}\right\|=R">，其中<img src="https://www.zhihu.com/equation?tex=P_0" alt="[公式]" eeimg="1" data-formula="P_0">为圆心，<img src="https://www.zhihu.com/equation?tex=P" alt="[公式]" eeimg="1" data-formula="P">为球表面一点，<img src="https://www.zhihu.com/equation?tex=R" alt="[公式]" eeimg="1" data-formula="R">为半径。我们还可以使用参数方程

<img src="https://www.zhihu.com/equation?tex=\begin{cases}x=R\sin\theta\cos\varphi+x_0\\y=R\sin\theta\sin\varphi+y_0\\z=R\cos\theta+z_0\end{cases}" alt="[公式]" eeimg="1" data-formula="\begin{cases}x=R\sin\theta\cos\varphi+x_0\\y=R\sin\theta\sin\varphi+y_0\\z=R\cos\theta+z_0\end{cases}">

表示，不过这个我们今天用不到，但以后会用得到。

我们将球的方程与光线方程联立：

<img src="https://www.zhihu.com/equation?tex=\begin{cases}\mathbf p=\mathbf s+t\cdot\mathbf d\\\left\|\vec{P_0P}\right\|=R\end{cases}" alt="[公式]" eeimg="1" data-formula="\begin{cases}\mathbf p=\mathbf s+t\cdot\mathbf d\\\left\|\vec{P_0P}\right\|=R\end{cases}">

其中<img src="https://www.zhihu.com/equation?tex=\mathbf s" alt="[公式]" eeimg="1" data-formula="\mathbf s">表示光线起点的坐标（或者说是从世界坐标系原点到s点的向量），<img src="https://www.zhihu.com/equation?tex=\mathbf d" alt="[公式]" eeimg="1" data-formula="\mathbf d">表示光线的方向向量

消掉<img src="https://www.zhihu.com/equation?tex=\mathbf p" alt="[公式]" eeimg="1" data-formula="\mathbf p">后得：

<img src="https://www.zhihu.com/equation?tex=\mathbf d^2\cdot t^2+2\cdot\mathbf d\cdot(\mathbf s-\mathbf P_0)\cdot t+(\mathbf s-\mathbf P_0)^2-R^2=0" alt="[公式]" eeimg="1" data-formula="\mathbf d^2\cdot t^2+2\cdot\mathbf d\cdot(\mathbf s-\mathbf P_0)\cdot t+(\mathbf s-\mathbf P_0)^2-R^2=0">

其中<img src="https://www.zhihu.com/equation?tex=\mathbf P_0" alt="[公式]" eeimg="1" data-formula="\mathbf P_0">的定义与<img src="https://www.zhihu.com/equation?tex=\mathbf s" alt="[公式]" eeimg="1" data-formula="\mathbf s">一样

这样，所有的向量都因为点乘变成了标量，我们方程就变成了关于t的一元二次方程了。


我们可以根据上述方程解的情况分成4类：

1. 方程无实根（<img src="https://www.zhihu.com/equation?tex=\Delta<0" alt="[公式]" eeimg="1" data-formula="\Delta<0">）:光线所在直线与球无交点，对应图中<img src="https://www.zhihu.com/equation?tex=\vec{AA'}" alt="[公式]" eeimg="1" data-formula="\vec{AA'}">，这时候我们无需处理
2. 方程有两个正根：光线所在直线与球有两个交点，且两个交点都位于光线起点的前方，对应图中<img src="https://www.zhihu.com/equation?tex=\vec{BB'}" alt="[公式]" eeimg="1" data-formula="\vec{BB'}">，这时候我们应该选取较小的那个根（较大的那个解离光线起点更远，被较小的那个点给遮挡了）
3. 方程有一个负根和一个正根：光线所在直线与球有两个交点，有一个交点位于光线起点的前方，另一个交点位于光线的后方，对应图中<img src="https://www.zhihu.com/equation?tex=\vec{CC'}" alt="[公式]" eeimg="1" data-formula="\vec{CC'}">，因为我们的光线为射线，我们应该取正跟，也就是较大那个根
4. 方程有两个负根：光线所在直线与球有两个交点，且两个交点都位于光线起点的后方，对应图中<img src="https://www.zhihu.com/equation?tex=\vec{DD'}" alt="[公式]" eeimg="1" data-formula="\vec{DD'}">，这时候我们无需处理

细节看上去有点多，但实际上我们把这一部分的处理和遮挡关系（等一下讲）结合起来，细节反而就没那么多了。

### 法向量

一句话：<img src="https://www.zhihu.com/equation?tex=\vec{P_0P}" alt="[公式]" eeimg="1" data-formula="\vec{P_0P}">就是法向量！当然为了以后方便，我们将<img src="https://www.zhihu.com/equation?tex=\vec{P_0P}" alt="[公式]" eeimg="1" data-formula="\vec{P_0P}">单位化

### 遮挡关系

我们一条光线可能会与多个物体相交，甚至会和一个物体有多个交点（比如我们的球），但是并不是所有的交点都有用，事实上，只有第一个与光线相交的点才是有效的交点，其他的点都被第一个点给挡住了，所以，我们就需要求出第一个与光线相交的点。换句话说，我们需要找到位于光线起点的前方且与光线端点距离最小的那个交点，而我们在<img src="https://www.zhihu.com/equation?tex=\mathbf p=\mathbf s+t\cdot\mathbf d" alt="[公式]" eeimg="1" data-formula="\mathbf p=\mathbf s+t\cdot\mathbf d">中的t就等于那个距离，因此，我们只需要求出最小的正数t即可。

具体怎么求呢？

我们先不管t不能是负值这个条件，那我们就变成了求一堆数中的最小值。这不就是算法界的"Hello World!"级入门题吗？我们只需要枚举一遍这些数，定义一个变量m代表已经枚举了的数中的最小值（初始时设为正无穷），每枚举一个数，如果这个数小于m，就令m等于这个数，当枚举完时最小值就是m。

我们在这里无非就是多了一个大于零的条件，只需要在枚举的时候先判断是否大于0，如果小于0直接跳过，不更新m就可以了。

我们的程序伪代码如下：

```c++
m=inf
for(each_object)//枚举每一个物体
{
	if(hit_the_object)//如果光线与物体相交
	{
		get_t_value//求出t的值
		if(t>0&&t<m)m=t//如果这个t符合条件，我们就更新m的值
	}
	
}
return m
```

对于每个物体，我们可能不止有一个交点，我们可以按刚才的方法再写一遍

```c++
m=inf
for(each_hitpoint)//枚举光线与这个物体的每一个交点
{
	if(t>0&&t<m)m=t
}
return m
```

对于一些特殊的物体，我们很容易得到t的大小关系，我们可以改变策略，从小往大枚举，第一个符合条件的数就直接返回，比如说我们的球：

```c++
t_small=(-b-sqrt(delta))/2a
t_large=(-b+sqrt(delta))/2a
if(t_small>0&&t_small<m)return t_small;
else if(t_large>0&&t_large<m)return t_large;
else return not_hit;
```

但是我在我的代码里并没有把判断是否大于0这个“0”写死在程序中，也没有把<img src="https://www.zhihu.com/equation?tex=m=inf" alt="[公式]" eeimg="1" data-formula="m=inf">写死在程序中，而是在函数中传来一个参数“t_min”和“t_max”，然后比较<img src="https://www.zhihu.com/equation?tex=t" alt="[公式]" eeimg="1" data-formula="t">是否大于<img src="https://www.zhihu.com/equation?tex=t\_min" alt="[公式]" eeimg="1" data-formula="t\_min">，如果<img src="https://www.zhihu.com/equation?tex=t" alt="[公式]" eeimg="1" data-formula="t">大于<img src="https://www.zhihu.com/equation?tex=t\_min" alt="[公式]" eeimg="1" data-formula="t\_min">，则用<img src="https://www.zhihu.com/equation?tex=t" alt="[公式]" eeimg="1" data-formula="t">去更新<img src="https://www.zhihu.com/equation?tex=t\_max" alt="[公式]" eeimg="1" data-formula="t\_max">，如果<img src="https://www.zhihu.com/equation?tex=t\_max>t" alt="[公式]" eeimg="1" data-formula="t\_max>t">的话（也就是说<img src="https://www.zhihu.com/equation?tex=t\_min" alt="[公式]" eeimg="1" data-formula="t\_min">的作用跟上文的“0”是一样的，<img src="https://www.zhihu.com/equation?tex=t\_max" alt="[公式]" eeimg="1" data-formula="t\_max">的作用跟上文的“m”是一样的）。这样的话我们就可以求出大于<img src="https://www.zhihu.com/equation?tex=t\_min" alt="[公式]" eeimg="1" data-formula="t\_min">，且小于<img src="https://www.zhihu.com/equation?tex=t\_max" alt="[公式]" eeimg="1" data-formula="t\_max">的最小的t是多少了（如果我们跟上文代码一样写死了“0”和“m”的话，我们就只能求大于0，且小于无穷远的最小的t了）。你可能会好奇为什么要这么写，其实我个人觉得怎么写没有什么所谓，在大多数时候我们就只需要求大于0，且小于无穷远的最小的t，虽然我在后面一小部分地方用到了不固定0和m的特性，但即使是把0和m写死在程序中也可以通过其他办法实现相同的功能。

### 代码实现

虽然我们这里只讲画一个球，且我们会有很长一段时间不会添加任何的东西，但我们的渲染器总不会止步于只画一个球，我们总还是会要添加一些其他的物体的，但是如果我们把每一个物体都写成一个独立的类的话，我们渲染器部分处理起来可能会非常繁琐，解决这类的问题一个很有效的方法是把我们渲染器所需的接口提取出来，形成一个父类，这样，不管这个东西是球还是立方体，还是什么别的，只要他是我们那个父类的子对象，他就必需有我们需要的接口，而只要他有我们需要的接口，我们的渲染器就能处理。

那我们渲染器所需要的接口是什么呢？再瞄一眼上文我们就知道答案了：求交点！因此我们创建的基类就要提供一个求交点的的接口（当然，我们以后可能会再这个基类中添加其他的接口，但目前我们只用得到求交点这一个接口），至于基类的名字，那就随便了，因为我们是要求交点，使用我就把它叫做相交（intersect）类了，如果你们想把他叫做物体（object）类也没有什么问题。

当然，我们还需要创建一个结构体来记录我们交点（最近的那个）的信息，包括上面的t的值，交点的坐标，交点处的法向量（再之后可能还会记录交点的材质等，但我们目前还没实现，所有就先不写进去）。

好了，我们可以开始写代码了。

先是交点信息的结构体

```c++
struct hitpoint//交点的信息
{
	real t;//ray 中的系数t(direction的t倍) 
	vec3 p,n;//p:相交点、撞击点的坐标;n:p点的表面的法向量(应该为单位向量)
};
```

接着是我们的相交类

```c++
class intersect//相交类
{
	public:
		virtual bool hit(const ray &sight,real t_min,real t_max,hitpoint &rec)const=0;//sight:光线;t_min,t_max:交点的距离的最小值与最大值,一般分别设为0和inf;rec交点的信息;返回值为是否相交
};
```

这里我们使用纯虚函数，因为所有的物体都必须要实现这个接口。函数的参数我们上文都讲过了。

球的类：
再复习一遍我们的公式：<img src="https://www.zhihu.com/equation?tex=\mathbf d^2\cdot t^2+2\cdot\mathbf d\cdot(\mathbf s-\mathbf P_0)\cdot t+(\mathbf s-\mathbf P_0)^2-R^2=0" alt="[公式]" eeimg="1" data-formula="\mathbf d^2\cdot t^2+2\cdot\mathbf d\cdot(\mathbf s-\mathbf P_0)\cdot t+(\mathbf s-\mathbf P_0)^2-R^2=0">
```c++
class sphere:public intersect
{
	public:
		sphere(const vec3 &h,const real &r):heart_(h),radius_(r){}//h为圆心，r为半径
		virtual bool hit(const ray &sight,real t_min,real t_max,hitpoint &rec)const override;
		{
			vec3 trace=sight.origin()-heart_;//公式中的s-P_0
			real a=sight.direction().dot(sight.direction()),b=2.*trace.dot(sight.direction()),c=trace.dot(trace)-radius_*radius_,delt=b*b-4.*a*c;//公式中的各项系数与delta
			if(delt>0)//如果delta大于0才有交点
			{
				real x=(-b-sqrt(delt))/(2.*a);//计算较小的那个t的值
				if(x<t_max&&x>t_min)//如果这个t满足条件
				{
					rec.t=x;//记录下t的值
					rec.p=sight.go(rec.t);//根据t的值算出交点
					rec.n=(rec.p-heart_)/radius_;//求出法向量，记得单位化（除以半径）
					return 1;//返回击中了
				}

				//如果较小的根不符合条件，则计算较大的根
				x=(-b+sqrt(delt))/(2.*a);
				if(x<t_max&&x>t_min)
				{
					rec.t=x;//记录下t的值
					rec.p=sight.go(rec.t);//根据t的值算出交点
					rec.n=(rec.p-heart_)/radius_;//求出法向量，记得单位化（除以半径）
					return 1;//返回击中了
				}
			}
			return 0;//如果没有交点或交点不符合条件，这返回没有击中
		}
		const real& r()const{return radius_;}
		const vec3& heart()const{return heart_;}
	private:
		vec3 heart_;
		real radius_;
};
```
如果稍微有一点编程经验的人都会发现这样写会有一点不大不小的问题，但由于现在这个问题还没有出现，所以我就先留着，等到出现问题了我再改。


我们还需要一个intersect的数组来存放我们的场景，顺便实现了整个场景中的求最近交点的功能（算法见第一段伪代码）

```c++
class intersections:public intersect//相交类的指针数组，用于保存场景
{
	public:
		intersections(intersect** list,size_t n):list_(list),size_(n){}
		virtual bool hit(const ray &sight,real t_min,real t_max,hitpoint &rec)const override//枚举每一个物体来求最近的交点，chapter 2中的第一段伪代码的实现
		{
			ray_tracing::hitpoint t_rec;//一个临时的交点消息记录变量
			bool hitSomething=0;//记录是否击中物体
			real far=t_max;//刚开始可以看到最远距离，chapter 2 中的m的作用
			for(int i=0;i<size_;i++)if(list_[i]->hit(sight,t_min,far,t_rec))//枚举每一个物体，如果这个物体存在距离大于t_min小于far的交点，则更新far
			{
				hitSomething=1;
				far=t_rec.t;//将上一次的最近撞击点作为视线可达最远处(更新far)
				rec=t_rec;
			}
			return hitSomething;
		}
	private:
		intersect** list_;//一个指针数组 指向每一个实体
		size_t size_;//物体个数
};
```

由于我们仍然没有完成光线的反射之类的内容，想要检验我们代码的正确性还得靠上一节提到的颜色图可视化向量的方法。这次我们检验交点表面的法向量。
```c++
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

vec3 get_color(const ray &sight,const intersect *world)//
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

	world=new intersections(objects.data(),objects.size());

	for(int i=0;i<height;i++)
	for(int j=0;j<width;j++)
	{
		ray sight=cam.get_ray((vec::real)i/height/*将坐标归一化*/,(vec::real)j/width/*将坐标归一化*/);
		bitmap_rgb[i][j]=get_color(sight,world);
	}
	write_PNG();
	system(file_name);
	return 0;
}
```

效果图如下：


看着这图我好像也看不出来对不对（逃

大概是对的吧。

我们再加一个球看一下遮挡关系：


好像没有什么问题

### 抗锯齿

如果我们仔细观看上面的效果图，我们会发现，我们的球的边缘的锯齿很严重，棱角非常的明显。

当然，由于我们这个是位图，我们不可能在位图中画出一个标准的圆，但我们有没有办法让锯齿感没那么严重呢。

首先，我们要明确一个概念：像素。虽然我们总是说像素点，但像素并不是数学意义下的点，因为前者是有大小（面积），而后者是没有大小的。所以我说像素的时候，你不应该把它理解成一个点，而应该把它想成一个小方块。

既然我们的像素点是一个小方块，那他里面就有无数个数学意义上的点，这些点的颜色可能各不相同，而我们的一个像素点只能涂一个颜色，这样，我们的图像就和现实中产生了偏差。我们要想办法改变这种偏差。

第一个办法是提高我们图像的分辨率，这样我们一个原来大像素就会变成很多的小像素，而我们的大锯齿就会变成小锯齿，然后就越来越逼近一个圆形了。

第二种方法在不提高分辨率的情况下，让边缘的像素的点过度的好一些，来减少锯齿感。

我们刚刚提到，我们以上帝视角来看一个像素的话，这个像素有很多很多的颜色，但我们只能涂一种颜色来代表这个像素。在我们上一节中，我们的照相机选择了这个像素里的一个点的颜色（具体来讲是左上那个点）来代表整个像素的颜色，但这样很显然是很不好的，会有一种“以偏概全”的感觉，如果这个像素内的点颜色差别不是很大（比如在红色圆的内部）那问题还不是很大，但如果这个像素中的点的颜色差别很大（在圆的边界部分）比如说有一半的点为（1，1，1）的白色，而有一半的点是（1，0，0）的红色，那无论我的像素是红色（1，0，0）还是白色（1，1，1）都不太好，不能代表整个像素所有点的颜色。

那有什么方法能代表整个像素内所有的点的颜色呢？我猜你已经想到了：平均数。我们都知道平均数在一定程度上可以代表一组数据的整体特征，虽然它生成的数据可能从来没有出现过，比如还是上述的红色与白色的例子：我们平均出来的颜色为粉色，而我们的格子内就没有粉色这种色，这也是我们的无奈之举了，而且生成的粉色可以看成圆内部与圆外部的过度部分，看上去还是挺自然的。

然而，事情还没有结束，我们还面临着一个问题，怎么求平均值？这里有无数个点，我们不可能求出每一个像素的颜色然后平均，因此我们还得想别的方法。

虽然我们不能求所有的点，但我们可以随机选其中若干个点带表所有点，很显然，如果我们选的点越多，我们这些点就越能代表整体，但我们计算所花费的时间也就越多，所以，我们自己还要做一个权衡。

我们把我们在一个像素内选的点的个数叫做SPP。

原来，我们使用照相机的时候，我们传给照相机是<img src="https://www.zhihu.com/equation?tex=(i,j)" alt="[公式]" eeimg="1" data-formula="(i,j)">代表第<img src="https://www.zhihu.com/equation?tex=i" alt="[公式]" eeimg="1" data-formula="i">行第<img src="https://www.zhihu.com/equation?tex=j" alt="[公式]" eeimg="1" data-formula="j">列的像素的左上角，而我们现在要在整个像素内采样，所以我们传给照相机的坐标为<img src="https://www.zhihu.com/equation?tex=(i+rand(),j+rand())" alt="[公式]" eeimg="1" data-formula="(i+rand(),j+rand())">，其中<img src="https://www.zhihu.com/equation?tex=rand()" alt="[公式]" eeimg="1" data-formula="rand()">生成的是<img src="https://www.zhihu.com/equation?tex=[0,1]" alt="[公式]" eeimg="1" data-formula="[0,1]">的随机数。

随机数实现：
```c++
real rand()
{
	static std::random_device device;
    static std::mt19937 generator(device());
    static std::uniform_real_distribution<vec::real>random;
    return random(generator);
}
```

把主函数中的

```c++
for(int i=0;i<height;i++)
for(int j=0;j<width;j++)
{
	ray sight=cam.get_ray((vec::real)i/height/*将坐标归一化*/,(vec::real)j/width/*将坐标归一化*/);
	bitmap_rgb[i][j]=get_color(sight,world);
}
```

改为

```c++
for(int i=0;i<height;i++)
for(int j=0;j<width;j++)
{
	for(int k=0;k<ssp;k++)
	{
		ray sight=cam.get_ray((vec::real)(i+rnd::rand())/height/*将坐标归一化*/,(vec::real)(j+rnd::rand())/width/*将坐标归一化*/);
		bitmap_rgb[i][j]+=get_color(sight,world);
	}
	bitmap_rgb[i][j]/=ssp;
}
```

效果好多了：
