#include <stdio.h>
#include <stdlib.h>
#include <apr.h>
#include <apr_general.h>
#include <apr_pools.h>
#include <apr_strings.h>
#include <apr_file_io.h>
#include <apr_time.h>

static apr_pool_t *pool;
static apr_file_t *apr_stdout, *apr_stderr;

/**
 * http://d.hatena.ne.jp/dayflower/20090512/1242093785
 */
int main(int argc, const char * const *argv, const char * const *envp) {
    char *str_ctime;
    char *str;

    if (apr_app_initialize(&argc, &argv, &envp) != APR_SUCCESS) {
        fputs("apr_app_initialize() failed", stderr);
        exit(1);
    }
    atexit(apr_terminate);

    apr_pool_create(&pool, NULL);

    apr_file_open_stdout(&apr_stdout, pool);
    apr_file_open_stderr(&apr_stderr, pool);

    str_ctime = apr_palloc(pool, APR_CTIME_LEN);
    apr_ctime(str_ctime, apr_time_now());

    str = apr_pstrcat(pool, "Current time is ", str_ctime, ".\n", NULL);
    apr_file_puts(str, apr_stdout);

    return 0;
}
