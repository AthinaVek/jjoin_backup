#ifndef _QUEUE_
#define _QUEUE_
/*dhlwseis typwn*/
    #include "job_scheduler.h"

    typedef query_job queueType;

    typedef struct {
    	int front;
    	int back;
    	int size;
    	queueType* buffer;
    } queue;

    void queue_create(queue *q, int size);

    void queue_destroy(queue *q);

    int queue_empty(queue *q);

    int queue_full(queue *q);

    int queue_push(queue *q, queueType element);

    int queue_pop(queue *q, queueType* element);

#endif
