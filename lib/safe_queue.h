#ifndef __SAFE_QUEUE
#define __SAFE_QUEUE


#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
 

template <typename T>
class safe_queue
{
public:
	safe_queue()
	{
		setmax(10000);
	}

	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		auto item = queue_.front();
		queue_.pop();
		cond_.notify_one();
		return item;
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		item = queue_.front();
		queue_.pop();
		cond_.notify_one();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (size() >= max_size_)
		{
			cond_.wait(mlock);
		}
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
	}

	void push(T&& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (size() >= max_size_)
		{
			cond_.wait(mlock);
		}
		queue_.push(std::move(item));
		mlock.unlock();
		cond_.notify_one();
	}

	size_t size()
	{
		return queue_.size();
	}

	void setmax(size_t max_size)
	{
		max_size_ = max_size;
	}

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
	size_t max_size_;
};


#endif	// __SAFE_QUEUE
