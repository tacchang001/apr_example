#include <stdlib.h>
#include <stdio.h>
#include <apr_general.h>
#include <unistd.h>
#include <apr_thread_proc.h>
#include <apr_thread_mutex.h>
#include <apr_thread_rwlock.h>

// Define APR thread pool
apr_pool_t *pool;

// Define server
////MyServer *server;

// Define threads
apr_thread_t *a_thread, *b_thread;
apr_status_t status;

static void * APR_THREAD_FUNC func_a(apr_thread_t * thread,
        void *data) {
    // do func_a stuff here
    while (1) {
        fprintf(stdout, "1");
        fflush(stdout);
        sleep(1);
    }
    return NULL;
}

static void * APR_THREAD_FUNC func_b(apr_thread_t * thread,
        void *data) {
    // do func_b stuff here
    while (1) {
        fprintf(stdout, "2");
        fflush(stdout);
        sleep(5);
    }
    return NULL;
}

// Cleanup before exit
void cleanup(int s) {
    printf("Caught signal %d\n", s);

    // Destroy thread pool
    apr_pool_destroy(pool);

    //apr_thread_exit(a_thread, APR_SUCCESS);
    //apr_thread_exit(b_thread, APR_SUCCESS);

    //apr_terminate();

    // Stop server and cleanup
////    server->stopServer();
////    delete server;

    exit(EXIT_SUCCESS);
}

/**
 * http://stackoverflow.com/questions/14192219/apr-threads-and-signal-handling
 */
int main(void) {
    // Signal handling
    signal(SIGINT, cleanup);

    // Create server
////    server = MyServerFactory
////    ::getServerImpl();
////
////    bool success = server->startServer();

// Initialize APR
    if (apr_initialize() != APR_SUCCESS) {
        printf("Could not initialize\n");
        return EXIT_FAILURE;
    }

// Create thread pool
    if (apr_pool_create(&pool, NULL) != APR_SUCCESS) {
        printf("Could not allocate pool\n");
        return EXIT_FAILURE;
    }

// Create a_thread thread
    if (apr_thread_create(&a_thread, NULL, func_a, NULL, pool) != APR_SUCCESS) {
        printf("Could not create a_thread\n");
        return EXIT_FAILURE;
    }

//Create b_thread thread
    if (apr_thread_create(&b_thread, NULL, func_b, NULL, pool) != APR_SUCCESS) {
        printf("Could not create b_thread\n");
        return EXIT_FAILURE;
    }

    // Join APR threads
    apr_thread_join(&status, a_thread);
    apr_thread_join(&status, b_thread);

    return EXIT_SUCCESS;
}
