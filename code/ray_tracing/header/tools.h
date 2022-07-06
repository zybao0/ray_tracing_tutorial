//define vec(type_name) cnum(const_number) rnd(random number)
#pragma once

#include<queue>
#include<mutex>
#include<thread>
#include<vector>
#include<atomic> 
#include<cstddef>
#include<functional>
#include<condition_variable>

#include<Eigen/Eigen>
#include<Eigen/Dense>

namespace vec
{
	#ifdef HIGHPRECISION       //set the high precision 
		using real=long double;
	#elif defined LOWPRECISION //set the low preciion
		using real=float;
	#else
		using real=double;     //default precision
	#endif                     //set precision

	using vec2=Eigen::Matrix<real,2,1>;
	using vec3=Eigen::Matrix<real,3,1>;
	using vec4=Eigen::Matrix<real,4,1>;

	using mat2=Eigen::Matrix<real,2,2>;
	using mat3=Eigen::Matrix<real,3,3>;
	using mat4=Eigen::Matrix<real,4,4>;
};

namespace cnum//const_number
{
	const vec::real INF=1e18;
	const vec::real eps=1e-9;
	const vec::real pi=acos(-1);
};

namespace rnd
{
	vec::real rand();
};

namespace my_thread_pool
{
	class task
	{
		public:
			virtual bool empty()const=0;
			virtual std::function<void(void)> get()=0;
	};

	class single_task:public task
	{
		public:
			template<typename T,typename ...Args>
			single_task(T&& func,Args&&... args);
			virtual bool empty()const override;
			virtual std::function<void(void)> get()override;
		private:
			std::function<void(void)>_func;
			bool _empty;
	};

	template<typename T,typename ...Args>
	single_task::single_task(T&& func,Args&&... args):_func(std::bind(std::forward<T>(func),std::forward<Args>(args)...)),_empty(0){}

	class multi_tasks:public task
	{
		public:
			multi_tasks();
			multi_tasks(multi_tasks &&tsk);
			template<typename T,typename ...Args>
			void push(T&& func,Args&&... args);
			virtual bool empty()const override;
			virtual std::function<void(void)> get()override;
		private:
			std::queue<std::function<void(void)>>_tasks;
	};

	template<typename T,typename ...Args>
	void multi_tasks::push(T&& func,Args&&... args)
	{
		_tasks.push(std::bind(std::forward<T>(func),std::forward<Args>(args)...));
	}

	class thread_pool
	{
		public:
			explicit thread_pool(size_t thread_num);
			template<typename T>
			void push(T &&tsk);
			~thread_pool();
		private:
			std::vector<std::thread>_threads;
			std::queue<task*> _task_queue;
			std::mutex _mutex;
			std::condition_variable _cv;
			std::atomic<bool>_stop;
	};

	template<typename T>
	void thread_pool::push(T &&tsk)
	{
		task* task_ptr=new T(std::forward<T>(tsk));
		std::unique_lock<std::mutex>lock(_mutex);
		_task_queue.push(task_ptr);
		lock.unlock();
		_cv.notify_one();
	}
};