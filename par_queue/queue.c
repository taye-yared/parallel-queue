#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/**
 *  Given data, place it on the queue.  Can be called by multiple threads.
 */
void queue_push(queue_t *queue, void *data) { 
	queue_node_t * curr = malloc(sizeof(queue_node_t));
	curr->data = data;
	curr->next = NULL;

	pthread_mutex_lock(&(queue->m));
	while(queue->size >= queue->maxSize){
		pthread_cond_wait(&(queue->cv), &(queue->m));
	}

	if(queue->head == NULL){
		queue->head = curr;
		queue->tail = curr;
		pthread_mutex_unlock(&(queue->m));
		return;
	}
	queue_node_t * temp = queue->tail;
	queue->tail = curr;
	temp->next = queue->tail;
	pthread_mutex_unlock(&(queue->m));

}

/**
 *  Retrieve the data from the front of the queue.  Can be called by multiple
 * threads.
 *  Blocks the queue is empty
 */
void *queue_pull(queue_t *queue) {
	pthread_mutex_lock(&(queue->m));
	while(queue->size <= 0){
		pthread_cond_wait(&(queue->cv), &(queue->m));
	}

	if(!queue->head){		// if queue is empty
		pthread_mutex_unlock(&(queue->m));
		return NULL;
	}
	if(!queue->head->next){ 	// if head is only thing in queue
		queue_node_t * temp = queue->head;
		queue->head = NULL;
		queue->tail = NULL;
		pthread_mutex_unlock(&(queue->m));
		return temp;
	}
	queue_node_t * temp = queue->head;
	queue->head = queue->head->next;
	temp->next = NULL;
	pthread_mutex_unlock(&(queue->m));
	return temp;
}

/**
 *  Initializes the queue
 */
void queue_init(queue_t *queue, int maxSize) { 
	queue->head = NULL;
	queue->tail = NULL;
	queue->maxSize = maxSize;
	queue->size = 0;
	pthread_cond_init(&(queue->cv), NULL);
	pthread_mutex_init(&(queue->m), NULL);
}

/**
 *  Destroys the queue, freeing any remaining nodes in it.
 */
void queue_destroy(queue_t *queue) { 
	while(queue->head){	
		free(queue->head);
		queue->head = queue->head->next;
	}
	pthread_cond_destroy(&(queue->cv));
	pthread_mutex_destroy(&(queue->m));

}