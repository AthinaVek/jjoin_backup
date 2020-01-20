#include <stdio.h>
#include <stdlib.h>

#include "job_scheduler.h"

int query_job_run(query_job *qj) {
    query_exec(qj->query/*, qk->js, qj->relations*/);
    return 0;
}

void query_job_init(query_job *qj, query_t *query, rel_t **relations, void *js) {
    qj->query = query;
    qj->relations = relations;
    qj->js = js;
}

/* Start routine */
void *jobThreadWork(void *arg) {
    job_scheduler *js = (job_scheduler *) arg;
    job_scheduler_threadWork(&js, NULL);
    return NULL;
}

/*
 * Getting job from queue and running it until:
 * 1) Queue is empty
 * 2) Barrier is called (bar)
 * 3) Work is done
 */
void job_scheduler_threadWork(job_scheduler *js, void *arg) {
    while (1) {
        if (pthread_mutex_lock(&(js->queueLock)) != 0) {
            fprintf(stderr, "%s\n", "lock");
            exit(EXIT_FAILURE);
        }

        while (queue_empty(&(js->q)) && !js->done && !js->bar) {
            pthread_cond_wait(&(js->cond_nonempty), &(js->queueLock));
        }

        query_job *job;
        if (queue_empty(&(js->q))) {
            job = NULL;
        } else {
            job = js->q.buffer[js->q.front];
            (js->q).pop();
        }
        if (queue_empty(&(js->q))) {
            // if queue has emptied, signal barrier
            pthread_cond_signal(&(js->cond_empty));
        }

        if (pthread_mutex_unlock(&(js->queueLock)) != 0) {
            fprintf(stderr, "%s\n", "unlock");
            exit(EXIT_FAILURE);
        }

        if (job != NULL) {
            query_job_run(job);
            // job->init(arg);
            // job->run();
            free(job);
        } else {
            if (js->done)
                return;
        }
        if (js->bar) {
            // wait until all have reached this point and bar is false again
            pthread_barrier_wait(&(js->pbar));
            pthread_barrier_wait(&(js->pbar));
        }
    }
}

/* Initialize JobScheduler info */
char job_scheduler_init(job_scheduler *js, size_t num_of_threads) {
    js->bar = 0;
    js->done = 0;
    js->q = malloc(sizeof(queue));
    queue_create(js->q, 100);

    pthread_mutex_init(&(js->queueLock), NULL);
    pthread_cond_init(&(js->cond_nonempty), NULL);
    pthread_cond_init(&(js->cond_empty), NULL);
    pthread_barrier_init(&(js->pbar), NULL, (unsigned) num_of_threads + 1);

    js->num_of_threads = num_of_threads;
    js->threads = malloc(sizeof(pthread_t)*num_of_threads);
    for (size_t i = 0; i < js->num_of_threads; i++) {
        pthread_create(js->threads + i, NULL, jobThreadWork, js);
    }
    return 1;
}

/* Destroy JobScheduler info */
char job_scheduler_destroy(job_scheduler *js) {
    pthread_mutex_destroy(&(js->queueLock));
    pthread_cond_destroy(&(js->cond_nonempty));
    pthread_cond_destroy(&(js->cond_empty));
    pthread_barrier_destroy(&(js->pbar));

    free(js->threads);
    return 1;
}

/* If barrier is called, wait until queue is empty and enable barrier.
 * Wake threads to finish any remaining jobs.
 * Wait until all jobs are done, then disable barrier.
 */
void job_scheduler_barrier(job_scheduler *js) {
    if (pthread_mutex_lock(&(js->queueLock)) != 0) {
        fprintf(stderr, "%s\n", "lock");
        exit(EXIT_FAILURE);
    }

    while (!queue_empty(&(js->q))) {
        pthread_cond_wait(&(js->cond_empty), &(js->queueLock));
    }
    js->bar = 1;
    if (pthread_mutex_unlock(&(js->queueLock)) != 0) {
        fprintf(stderr, "%s\n", "unlock");
        exit(EXIT_FAILURE);
    }

    pthread_cond_broadcast(&(js->cond_nonempty));
    pthread_barrier_wait(&(js->pbar));
    js->bar = 0;
    pthread_barrier_wait(&(js->pbar));     // so threadWork doesnt get stuck in wait
}

/* Appoint job in JobScheduler's queue*/
int job_scheduler_schedule(job_scheduler *js, query_job *job) {
    if (pthread_mutex_lock(&(js->queueLock)) != 0) {
        fprintf(stderr, "%s\n", "lock");
        exit(EXIT_FAILURE);
    }
    queue_push(&(js->q), job);
    pthread_cond_signal(&(js->cond_nonempty));
    if (pthread_mutex_unlock(&(js->queueLock)) != 0) {
        fprintf(stderr, "%s\n", "unlock");
        exit(EXIT_FAILURE);
    }
    return 0;
}

/* Enable done and wait until all threads finish their jobs */
void job_scheduler_stop(job_scheduler *js) {
    js->done = 1;
    pthread_cond_broadcast(&(js->cond_nonempty));
    for (size_t i = 0; i < js->num_of_threads; i++) {
        pthread_join(js->threads[i], NULL);
    }
}
