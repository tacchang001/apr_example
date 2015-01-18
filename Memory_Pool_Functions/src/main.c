#include <stdio.h>
#include <apr_pools.h>

/**
 * http://kurukurupapa.hatenablog.com/entries/2008/01/14
 */
int main(int argc, const char* const *argv) {
    apr_status_t status = APR_SUCCESS;
    apr_pool_t *pool = NULL;
    apr_pool_t *pool2 = NULL;
    void *ptr = NULL;

    // APRを初期化する。
    apr_initialize();

    // 親プールを作成する。
    status = apr_pool_create(&pool, NULL);
    if (status == APR_SUCCESS) {

        // メモリを確保する。
        ptr = apr_pcalloc(pool, 10);

        // 子プールを作成する。
        status = apr_pool_create(&pool2, pool);
        if (status == APR_SUCCESS) {

            // メモリを確保する。
            ptr = apr_pcalloc(pool2, 10);
            if (ptr != NULL) {
                // 書き込み
                strcpy((char*) ptr, "Hello");

                printf("書き込み結果 [%s]\n", ptr);
            }

            // メモリを解放する。
            apr_pool_clear(pool2);
        } else {
            printf("子プールを作成できませんでした。\n");
        }

        // プールを破棄する。
        apr_pool_destroy(pool);
    } else {
        printf("親プールを作成できませんでした。\n");
    }

    // APRを終了する。
    apr_terminate();

    return 0;
}
