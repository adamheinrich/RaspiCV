#ifndef CV_QUEUE_H
#define CV_QUEUE_H

#include <pthread.h>
#include <list>

using namespace std;

template <typename T>
class cv_queue
{
	/* Inspired by http://vichargrave.com/multithreaded-work-queue-in-c */
public:
	cv_queue()
	{
		pthread_mutex_init(&m_mutex, NULL);
		pthread_cond_init(&m_condv, NULL);
	}

	~cv_queue()
	{
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_condv);
	}

	void add(T item)
	{
		pthread_mutex_lock(&m_mutex);
		m_queue.push_back(item);
		pthread_cond_signal(&m_condv);
		pthread_mutex_unlock(&m_mutex);
	}

	T remove()
	{
		pthread_mutex_lock(&m_mutex);
		while (m_queue.size() == 0)
			pthread_cond_wait(&m_condv, &m_mutex);
		T item = m_queue.front();
		m_queue.pop_front();
		pthread_mutex_unlock(&m_mutex);
		return item;
	}

	int size()
	{
		pthread_mutex_lock(&m_mutex);
		int size = m_queue.size();
		pthread_mutex_unlock(&m_mutex);
		return size;	
	}

private:
	list<T> m_queue;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_condv;
};

#endif
