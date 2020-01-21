#ifndef js_H_
#define js_H_
    #define NUM_OF_THREADS 8

    #include <pthread.h>
    #include <stdlib.h>

    #include "query.h"
    #include "relation.h"
    #include "queue.h"

    typedef struct job_scheduler job_scheduler;

    typedef struct query_job {
        query_t *query;
        rel_t **relations;
        job_scheduler *js;
    }query_job;

    typedef query_job queueType;

    void query_job_init(query_job *qj, query_t *query, rel_t **relations, void *js);

    int run(query_job *qj);

    typedef struct {
        int front;
        int back;
        int size;
        queueType** buffer;
    } queue_t;

    void queue_create(queue_t *q, int size);

    void queue_destroy(queue_t *q);

    int queue_empty(queue_t *q);

    int queue_full(queue_t *q);

    int queue_push(queue_t *q, queueType *element);

    queueType *queue_pop(queue_t *q);

    typedef struct job_scheduler {
        char bar;
        char done;
        size_t num_of_threads;
        pthread_t *threads;
        pthread_mutex_t queueLock;
        pthread_cond_t cond_nonempty;
        pthread_cond_t cond_empty;
        pthread_barrier_t pbar;
        queue_t q;
    } job_scheduler;

    void job_scheduler_threadWork(job_scheduler *js, void *arg);
    /**
     * Initializes the JobScheduler with the number of open threads.
     * Returns true if everything done right false else.
     */
    char job_scheduler_init(job_scheduler *js, size_t num_of_threads);

    /**
     * Free all resources that the are allocated by JobScheduler
     * Returns true if everything done right false else.
     */
    char job_scheduler_destroy(job_scheduler *js);

    /**
     * Waits until executed all jobs in the queue.
     */
    void job_scheduler_barrier(job_scheduler *js);

    /**
     * Add a job in the queue and returns a JobId
     */
    int job_scheduler_schedule(job_scheduler *js, query_job *qj);

    /**
     * Waits until all threads finish their job, and after that close all threads.
     */
    void job_scheduler_stop(job_scheduler *js);


#endif
