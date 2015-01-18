#include <stdlib.h>
#include <apr.h>
#include <apr_general.h>
#include <apr_pools.h>
#include <apr_strings.h>
#include <apr_file_io.h>

static apr_file_t *apr_stdout, *apr_stdin, *apr_stderr;

int main(int argc, const char* const *argv) {
    apr_pool_t *p;
    char *str;

    apr_app_initialize(&argc, &argv, NULL);
    apr_pool_create(&p, NULL);
    apr_file_open_stdout(&apr_stdout, p);
    apr_file_open_stdin(&apr_stdin, p);
    apr_file_open_stderr(&apr_stderr, p);

    str = apr_pstrcat(p, "Hello", ", ", "World", NULL);
    apr_file_printf(apr_stdout, "%s\n", str);
    return EXIT_SUCCESS;
}
