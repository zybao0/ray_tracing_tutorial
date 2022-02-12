//define vec(type_name) cnum(const_number) rnd(random number)
#pragma once

#include<queue>
#include<tuple>
#include<mutex>
#include<cstdio>
#include<thread>
#include<vector>
#include<atomic> 
#include<cstddef>
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
	template<size_t N>
	struct tuple_expand
	{
		template<typename Function,typename Tuple, typename... Argus>
		static auto expand(Function&& func,Tuple&& tup,Argus&& ...args)->decltype(tuple_expand<N-1>::expand(std::forward<Function>(func),std::forward<Tuple>(tup),std::get<N-1>(std::forward<Tuple>(tup)),std::forward<Argus>(args)...))
		{
			return tuple_expand<N-1>::expand(std::forward<Function>(func),std::forward<Tuple>(tup),std::get<N-1>(std::forward<Tuple>(tup)),std::forward<Argus>(args)...);
		}
	};
	template<>
	struct tuple_expand<0>
	{
		template<typename Function,typename Tuple, typename... Argus>
		static auto expand(Function&& func,Tuple&& tup,Argus&& ...args)->decltype(func(std::forward<Argus>(args)...))
		{
			return func(std::forward<Argus>(args)...);
		}
	};
	template<typename... Args>
	class thread_pool
	{
		public:
			typedef void(*Func_ptr)(Args ...);
			thread_pool(size_t n,const Func_ptr f);
			~thread_pool();
			thread_pool(const thread_pool&)=delete;
			thread_pool &operator=(const thread_pool&)=delete;
			void push_task(Args... args);
			bool queue_empty();
		private:
			void work();
		public:
			std::condition_variable queue_empty_msg;
		private:
			Func_ptr func;
			size_t thread_num;
			std::atomic<size_t>task_left;
			std::atomic<bool>stop_running;
			std::vector<std::thread>threads;
			std::queue<std::tuple<Args...>>task_queue;
			std::mutex queue_lock;
			std::condition_variable new_task_msg;
	};
	template<typename... Args>
	thread_pool<Args ...>::thread_pool(size_t n,const Func_ptr f):thread_num(n),func(f),task_left(0),stop_running(0)
	{
		for(int i=0;i<thread_num;i++)threads.emplace_back(&thread_pool<Args ...>::work,this);
	}
	template<typename... Args>
	thread_pool<Args ...>::~thread_pool()
	{
		stop_running.store(1,std::memory_order_relaxed);
		new_task_msg.notify_all();
		for(int i=0;i<thread_num;i++)if(threads[i].joinable())threads[i].join();
	}
	template<typename... Args>
	void thread_pool<Args ...>::push_task(Args... args)
	{
		task_left++;
		std::lock_guard<std::mutex> lk(queue_lock);
		task_queue.emplace(args...);
		new_task_msg.notify_one();
	}
	template<typename... Args>
	void thread_pool<Args ...>::work()
	{
		while(1)
		{
			std::unique_lock<std::mutex> lk(queue_lock);
			new_task_msg.wait(lk,[this]{return stop_running||!task_queue.empty();});
			if(stop_running)break;
			std::tuple<Args...>t(std::move(task_queue.front()));
			task_queue.pop();
			lk.unlock();
			tuple_expand<std::tuple_size<std::tuple<Args...> >::value>::expand(func,t);
			task_left--;
			if(queue_empty())queue_empty_msg.notify_all();
		}
	}
	template<typename... Args>
	bool thread_pool<Args ...>::queue_empty()
	{
		return task_left.load(std::memory_order_relaxed)==0;
	}
};