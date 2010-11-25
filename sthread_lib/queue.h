/*
 * queue.h -- definition and declarations of sthread queue
 */
#include <stdlib.h>
#include <stdio.h>
#include <sthread.h>
#include <sthread_user.h>


/* queue_element_t */
typedef struct queue_element {
    struct _sthread *thread;
    struct queue_element *next;
} queue_element_t;

/* queue_t */
typedef struct {
  queue_element_t *first;
  queue_element_t *last;
} queue_t;

/* create_queue - allocates memory for queue_t and initiates the structure */
queue_t* create_queue();

/* delete_queue - frees all memory used by queue and its elements */
void delete_queue(queue_t *queue);

/* queue_is_empty - returns 1 if queue empty, else returns 0 */
int queue_is_empty(queue_t *queue);

/* queue_first - returns a pointer to the queue first element */
struct _sthread* queue_first(queue_t *queue);

/* queue_insert - inserts a new element on the end of the queue */
void queue_insert(queue_t *queue, struct _sthread *thread);

/* queue_remove - removes the first element of the queue */
struct _sthread* queue_remove(queue_t *queue);

/* queue_rotate - replaces active thread for the first in the queue, the old
 *                thread is inserted in the queue end */  
struct _sthread* queue_rotate(queue_t *queue, struct _sthread* old_thr);
