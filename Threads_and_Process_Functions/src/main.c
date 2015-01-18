#include <stdlib.h>
#include <stdio.h>
#include <apr_general.h>
#include <apr_thread_proc.h>
#include <apr_thread_mutex.h>
#include <apr_thread_rwlock.h>

/**
 * http://sylvainmarechal.chez-alice.fr/prog/apache/apr/test_thread.c
 */

//  threads (function)
static void * APR_THREAD_FUNC thread_function(apr_thread_t * thread, void *data) {
    const char * p = (const char *) data;
    printf("Thread arg. is '%s'\n", p);
    return NULL;
}

//  threads (main)
void test_thread(apr_pool_t *pool) {
    apr_thread_t *thread;
    apr_status_t status;

    if (apr_thread_create(&thread, NULL, thread_function, (void *) "coucou",
                          pool) != APR_SUCCESS) {
        printf("Could not create the thread\n");
        exit(-1);
    }

    apr_thread_join(&status, thread);
}

#define THREADS_NUMBER  10
typedef struct thread_params {
    apr_pool_t * pool;
    apr_thread_mutex_t * mutex;
    apr_threadkey_t * threadkey;
} thread_params;

static int thread_global = 0;

//  threads with key and locks (function)
/*
static apr_status_t threadkey_data_cleanup(void * data) {
    printf("threadkey_data_cleanup called (data=%p)\n", data);
    return APR_SUCCESS;
}
*/

//  threads with key and locks (function)
static void * APR_THREAD_FUNC thread_function_ex(apr_thread_t * thread,
                                                 void *data) {
    thread_params * ptp = (thread_params *) data;
    apr_thread_mutex_t * mutex = ptp->mutex;
    apr_threadkey_t * threadkey = ptp->threadkey;
    void * key;
    int i;
    static int thread_number;
    int local_thread_number;

    apr_thread_mutex_lock(mutex);
    local_thread_number = ++thread_number;
    apr_thread_mutex_unlock(mutex);

    /*apr_threadkey_data_set( (void *)local_thread_number,
     "this thread", threadkey_data_cleanup, threadkey );*/
    void *new_mem = &local_thread_number;
    apr_threadkey_private_set(&new_mem, threadkey);
    for (i = 0; i < 100000; i++) {
        apr_thread_mutex_lock(mutex);
        thread_global++;
        apr_thread_mutex_unlock(mutex);
    }
    /*apr_threadkey_data_get( (void *)&key, "this thread",  threadkey );*/
    apr_threadkey_private_get((void *) &key, threadkey);
    printf("End of thread (localthread=%d,key=%p)\n", local_thread_number, key);
    return NULL;
}

//  threads with key and lock (key dest.)
void threadkey_function_destructor(void * data) {
    printf("threadkey_function_destructor called (data=%p)\n", data);
}

//  threads with key and lock (main)
void test_thread_ex(apr_pool_t *pool) {
    apr_thread_t * athreads[THREADS_NUMBER];
    apr_status_t status;
    thread_params tp;
    int i;

    /* Init mutex */
    if (apr_thread_mutex_create(&tp.mutex,
    APR_THREAD_MUTEX_DEFAULT,
                                pool) != APR_SUCCESS) {
        printf("apr_lock_create() failed ...\n");
        exit(1);
    }

    /* Init tls key */
    if (apr_threadkey_private_create(&tp.threadkey,
                                     threadkey_function_destructor,
                                     pool) != APR_SUCCESS) {
        printf("apr_threadkey_private_create() failed ...\n");
        exit(1);
    }

    tp.pool = pool;
    for (i = 0; i < THREADS_NUMBER; i++) {
        if (apr_thread_create(&athreads[i], NULL, thread_function_ex,
                              (void *) &tp, pool) != APR_SUCCESS) {
            printf("Could not create the thread '%d'\n", i);
            exit(-1);
        }
    }

    for (i = 0; i < THREADS_NUMBER; i++) {
        apr_thread_join(&status, athreads[i]);
    }
    printf("All threads are exited and thread_global='%d'\n", thread_global);
    apr_threadkey_private_delete(tp.threadkey);
    apr_thread_mutex_destroy(tp.mutex);
}

static int global_rdlock_counter = 0;

// thread_writer()
static void * APR_THREAD_FUNC thread_writer(apr_thread_t * thread, void *data) {
    apr_thread_rwlock_t * rwlock = (apr_thread_rwlock_t *) data;
    int i;
    for (i = 0; i < 1000000; i++) {
        apr_thread_rwlock_wrlock(rwlock);
        global_rdlock_counter++;
        apr_thread_rwlock_unlock(rwlock);
    }
    return NULL;
}

//  thread_reader()
static void * APR_THREAD_FUNC thread_reader(apr_thread_t * thread, void *data) {
    apr_thread_rwlock_t * rwlock = (apr_thread_rwlock_t *) data;
    int i;
    for (i = 0; i < 1000000; i++) {
        int a;
        apr_thread_rwlock_rdlock(rwlock);
        a = global_rdlock_counter;
        apr_thread_rwlock_unlock(rwlock);
        a = a + 1;
    }
    return NULL;
}

#define READER_THREADS  10
#define WRITER_THREADS  2
//  test_thread_rwlock()
void test_thread_rwlock(apr_pool_t *pool) {
    apr_thread_t *athreads[WRITER_THREADS + READER_THREADS];
    apr_status_t status;
    apr_thread_rwlock_t * rwlock;
    int i;

    /* create the rd lock */
    status = apr_thread_rwlock_create(&rwlock, pool);
    if (status != APR_SUCCESS) {
        printf("apr_thread_rwlock_create() failed %d\n", status);
        exit(1);
    }

    /* create some writer threads */
    for (i = 0; i < WRITER_THREADS; i++) {
        if (apr_thread_create(&athreads[i], NULL, thread_writer,
                              (void *) rwlock, pool) != APR_SUCCESS) {
            printf("Could not create the thread\n");
            exit(-1);
        }
    }

    /* create some reader threads */
    for (i = WRITER_THREADS; i < READER_THREADS + WRITER_THREADS; i++) {
        if (apr_thread_create(&athreads[i], NULL, thread_reader,
                              (void *) rwlock, pool) != APR_SUCCESS) {
            printf("Could not create the thread (i=%d)\n", i);
            exit(-1);
        }
    }

    /* wait end */
    for (i = 0; i < READER_THREADS + WRITER_THREADS; i++) {
        apr_thread_join(&status, athreads[i]);
    }

    apr_thread_rwlock_destroy(rwlock);

    /* */
    printf("global_rdlock_counter=%d\n", global_rdlock_counter);
}

/**
 *
 */
int main(int argc, char * argv[]) {
    apr_pool_t *pool;
    if (apr_initialize() != APR_SUCCESS) {
        printf("Could not initialize\n");
        exit(-1);
    }
    if (apr_pool_create(&pool, NULL) != APR_SUCCESS) {
        printf("Could not allocate pool\n");
        exit(-1);
    }
    test_thread(pool);
    test_thread_ex(pool);
    test_thread_rwlock(pool);
    apr_pool_destroy(pool);
    return 0;
}
