/*
 * queue.c - implementation of queue manipulation functions
 */
#include "queue.h"

queue_element_t *empty_element_ptr;

void sthread_user_free(struct _sthread *thread);

queue_t* create_queue() {
  queue_t *queue;
  queue = (queue_t*) malloc(sizeof(queue_t));
  queue->first = NULL;
  queue->last = NULL;
  empty_element_ptr = NULL;
  return queue;
}

void delete_queue(queue_t *queue) {
  queue_element_t *pointer, *next;
  pointer = queue->first;
  while(pointer){
    next = pointer->next;
    sthread_user_free(pointer->thread);
    free(pointer);
    pointer = next;
  }
  free(queue);
}

void queue_insert(queue_t *queue, struct _sthread *thread) {

  queue_element_t *new_element;

  if(empty_element_ptr == NULL)
    new_element = (queue_element_t*) malloc(sizeof(queue_element_t));
  else {
    new_element = empty_element_ptr;
    empty_element_ptr = NULL;
  }

  new_element->thread = thread;
  new_element->next = NULL;

  if(queue->first == NULL) {
    queue->first = new_element;
    queue->last = new_element;
    return;
  }
  queue->last->next = new_element;
  queue->last = new_element;
}


int queue_is_empty(queue_t *queue){
  if (queue->first == NULL)return 1;
  else return 0;
}


struct _sthread *queue_first(queue_t *queue) {
  queue_element_t *pointer; 
  pointer = queue->first;
  return pointer->thread;
}


struct _sthread * queue_remove(queue_t *queue) {
  queue_element_t *temp;
  struct _sthread *thread;

  temp = queue->first;
  queue->first = temp->next;
  thread = temp->thread;
  temp->thread = NULL;
  temp->next = NULL;

  if(empty_element_ptr)free(temp);
  else empty_element_ptr = temp;
 
  return thread;
}
