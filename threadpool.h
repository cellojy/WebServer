#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<list>
#include<cstdio>
#include<exception>
#include<pthread.h>
#include"locker.h"

template<typename T>
class threadpool
{
public:
    threadpool(int thread_number=8,int max_requesets=10000);
    ~threadpool();
    bool append(T* request);
private:
    static void* worker(void* arg);
    void run();
private:
    int m_thread_number; // num of thread
    int m_max_requesets; //num of max_requesets
    pthread_t *m_threads; //thread queue
    std::list<T*>m_workerqueue; 
    locker m_queuelocker;
    sem m_queuestat;
    bool m_stop;
};

template<typename T>
threadpool<T>::threadpool(int thread_number,int max_requesets):
    m_thread_number(thread_number),m_max_requesets(max_requesets),m_threads(NULL),m_stop(false)
{  
    m_threads=new pthread_t[thread_number];
    if(!m_threads) throw std::exception();
    for(int i=0;i<thread_number;++i)
    {
        if(pthread_create(m_threads+i,NULL,worker,this)!=0)
        {
            delete []m_threads;
            throw std::exception();
        }
        if(pthread_detach(m_threads[i]))
        {
            delete []m_threads;
            throw std::exception();
        }
    }
}

template<typename T>
threadpool<T>::~threadpool()
{
    delete []m_threads;
    m_stop=true;
}

template<typename T>
bool threadpool<T>::append(T* request)
{
    m_queuelocker.lock();
    if(m_workerqueue.size() > m_max_requesets)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workerqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
void* threadpool<T>::worker(void *arg)
{
    threadpool* pool=(threadpool*) arg;
    pool->run();
    return pool;
}

template<typename T>
void threadpool<T>::run()
{
    while(!m_stop)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if(m_workerqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T* request=m_workerqueue.front();
        m_workerqueue.pop_front();
        m_queuelocker.unlock();
        if(!request)
        {
            continue;
        }
        request->process();
    }
}
#endif
