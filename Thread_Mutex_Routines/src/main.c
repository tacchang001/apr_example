#include <stdlib.h>
#include <apr_general.h>
#include <apr_file_io.h>
#include <apr_thread_mutex.h>

static apr_pool_t *pool;
static apr_file_t *apr_stdout, *apr_stderr;

int main(int argc, const char * const *argv, const char * const *envp) {
    if (apr_app_initialize(&argc, &argv, &envp) != APR_SUCCESS) {
        fputs("apr_app_initialize() failed", stderr);
        exit(1);
    }
    atexit(apr_terminate);

    apr_pool_create(&pool, NULL);

    apr_file_open_stdout(&apr_stdout, pool);
    apr_file_open_stderr(&apr_stderr, pool);

    apr_thread_mutex_t *mod_mruby_mutex;

    apr_status_t status = apr_thread_mutex_create(&mod_mruby_mutex, APR_THREAD_MUTEX_DEFAULT, pool);
    if(status != APR_SUCCESS){
        fprintf(stderr, "ERROR: Error creating thread mutex.");
        return status;
    }

    if (apr_thread_mutex_lock(mod_mruby_mutex) != APR_SUCCESS) {
        fprintf(stderr, "ERROR: mod_mruby_mutex lock failed");
        return -1;
    }

    if (apr_thread_mutex_unlock(mod_mruby_mutex) != APR_SUCCESS){
        fprintf(stderr, "ERROR: mod_mruby_mutex unlock failed");
        return -1;
    }

    return 0;
}
