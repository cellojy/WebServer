#ifndef LOCKER_H
#define LOCKER_H
#include<exception>
#include<pthread.h>
#include<semaphore.h>
class sem
{
public:
    sem()
    {
        int res=sem_init(&m_sem,0,0);
        if(res!=0) throw std::exception();
    }
    ~sem()
    {
        sem_destroy(&m_sem);
    }
    bool wait()
    {
        return sem_wait(&m_sem);
    }
    bool post()
    {
        return sem_post(&m_sem);
    }

private:
    sem_t m_sem;
};

class locker
{
public:
    locker()
    {
        int res=pthread_mutex_init(&m_mutex,NULL);
        if(res!=0) throw std::exception();
    }
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex);
    }
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;

private:

};

class cond
{
public:
    cond()
    {
        if(pthread_mutex_init(&m_mutex,NULL)!=0) throw std::exception();
        if(pthread_cond_init(&m_cond,NULL)!=0)
        {
            pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    ~cond()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }
    bool wait()
    {
        int res=0;
        pthread_mutex_lock(&m_mutex);
        res=pthread_cond_wait(&m_cond,&m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return res;
    }
    bool signal()
    {
        return pthread_cond_signal(&m_cond)==0;
    }
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;

};
#endif
