
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

extern int (*glibc_pthread_mutex_lock)(pthread_mutex_t *m);
extern int (*glibc_pthread_mutex_unlock)(pthread_mutex_t *m);

extern "C"
{

typedef struct
{
    pid_t tid;
    pthread_t t;
} thread_to_tid_t;

thread_to_tid_t thread_to_tid[512] = {{0,0}};

void my_settid_to_thread(pthread_t t, pid_t tid)
{
    glibc_pthread_mutex_lock(&mutex);
    for(int i=0;i<512;i++)
    {
        if(thread_to_tid[i].tid == 0 && thread_to_tid[i].t == 0)
        {
            thread_to_tid[i].tid = tid;
            thread_to_tid[i].t = t;
        }
    }
    glibc_pthread_mutex_unlock(&mutex);
}

pid_t my_gettid_from_thread(pthread_t t)
{
    glibc_pthread_mutex_lock(&mutex);
    pid_t tid = 0;
    for(int i=0;i<512;i++)
    {
        if(thread_to_tid[i].t == t)
        {
            tid = thread_to_tid[i].tid;
            break;
        }
    }
    glibc_pthread_mutex_unlock(&mutex);
    return tid;
}

void my_remtid(pthread_t t)
{
    glibc_pthread_mutex_lock(&mutex);
    for(int i=0;i<512;i++)
    {
        if(thread_to_tid[i].t == t)
        {
            thread_to_tid[i].tid = 0;
            thread_to_tid[i].t = 0;
        }
    }
    glibc_pthread_mutex_unlock(&mutex);
}

}

