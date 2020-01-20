#include <stdlib.h>
#include <stdio.h>
#include "queue.h"


void queue_create(queue *q, int size) {
	q->front = 0;
	q->back = 0;
	q->size = size;
	q->buffer = malloc(size * sizeof(queueType));
}

void queue_destroy(queue *q) {
	printf("DESTROY\n");
	free(q->buffer);
}

int queue_empty(queue *q) {
	return ( q->front == q->back );
}

int queue_full(queue *q) {
	int neo_back = (q->back+1) % q->size;
	if (neo_back == q->front )
		return 1;
	else
		return 0;
}

int queue_push(queue *q, queueType element) {
	if ( queue_full(q) == 0 ){
		q->buffer[q->back] = element;
		q->back = ( q->back + 1 ) % q->size;
		return 1;
	}
	return 0;
}

int queue_pop(queue *q, queueType* element) {
	if ( queue_empty(q) == 0 ){
		*element = q->buffer[q->front];
		q->front = ( q->front + 1 ) % q->size;
		return 1;
	}
	return 0;
}
