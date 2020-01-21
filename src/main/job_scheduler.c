#include <stdio.h>
#include <stdlib.h>

#include "job_scheduler.h"

void queue_create(queue_t *q, int size) {
	q->front = 0;
	q->back = 0;
	q->size = size;
	q->buffer = malloc(size * sizeof(queueType *));
}

void queue_destroy(queue_t *q) {
	printf("DESTROY\n");
	free(q->buffer);
}

int queue_empty(queue_t *q) {
	return ( q->front == q->back );
}

int queue_full(queue_t *q) {
	int neo_back = (q->back+1) % q->size;
	if (neo_back == q->front )
		return 1;
	else
		return 0;
}

int queue_push(queue_t *q, queueType *element) {
	if ( queue_full(q) == 0 ){
		q->buffer[q->back] = element;
		q->back = ( q->back + 1 ) % q->size;
		return 1;
	}
	return 0;
}

queueType *queue_pop(queue_t *q) {
	if ( queue_empty(q) == 0 ){
		queueType *element = q->buffer[q->front];
		q->front = ( q->front + 1 ) % q->size;
		return element;
	}
	return NULL;
}

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
    job_scheduler_threadWork(js, NULL);
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
            job = queue_pop(&(js->q));
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
    queue_create(&(js->q), 100);

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
