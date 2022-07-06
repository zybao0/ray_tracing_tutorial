#include<ctime>
#include<random>
#include<cstdlib>

#include<tools.h>

vec::real rnd::rand()
{
	static std::random_device device;
	static std::mt19937 generator(device());
	static std::uniform_real_distribution<vec::real>random;
	return random(generator);
}

namespace my_thread_pool
{
	bool single_task::empty()const
	{
		return _empty;
	}
	std::function<void(void)> single_task::get()
	{
		_empty=1;
		return _func;
	}

	multi_tasks::multi_tasks(){}
	multi_tasks::multi_tasks(multi_tasks &&tsk):_tasks(std::move(tsk._tasks)){}
	bool multi_tasks::empty()const
	{
		return _tasks.empty();
	}
	std::function<void(void)> multi_tasks::get()
	{
		std::function<void(void)> func=std::move(_tasks.front());
		_tasks.pop();
		return func;
	}

	thread_pool::thread_pool(size_t thread_num=0):_stop(0)
	{
		for(int i=0;i<thread_num;i++)_threads.emplace_back([this]
		{
			while(1)
			{
				std::unique_lock<std::mutex>lock(_mutex);
				_cv.wait(lock,[this]{return _stop||!_task_queue.empty();});
				if(_task_queue.empty())break;
				task* task_ptr=_task_queue.front();
				std::function<void(void)> func=std::move(task_ptr->get());
				if(task_ptr->empty())
				{
					delete task_ptr;
					_task_queue.pop();
				}
				lock.unlock();
				func();
			}
		});
	}
	thread_pool::~thread_pool()
	{
		_stop.store(1,std::memory_order_relaxed);
		_cv.notify_all();
		for(size_t i=0;i<_threads.size();i++)if(_threads[i].joinable())_threads[i].join();
	}
}