#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

#define CAPACITY 16

static pthread_t p1;
static pthread_t c1;

typedef struct {

	char c;
	int i;

} elem;

typedef struct {

	elem* buf;
	size_t tail;
	size_t head;
	size_t counter;
	pthread_mutex_t mutex;
	pthread_cond_t cond;

} circular_buf;

void circular_buf_init(circular_buf* b, elem* storage) {

	b->buf = storage;
	b->tail = 0;
	b->head = 0;
	b->counter = 0;
	pthread_mutex_init(&(b->mutex), NULL);
	pthread_cond_init(&(b->cond), NULL);

}

void enqueue(circular_buf* b, char name, int value) {

	pthread_mutex_lock(&(b->mutex));
	while (b->counter == CAPACITY) {
	
		pthread_cond_wait(&(b->cond), &(b->mutex));

	}
	(b->buf + b->head)->c = name;
	(b->buf + b->head)->i = value;
	b->head = (b->head + 1) % CAPACITY;
	++(b->counter);
	pthread_cond_broadcast(&(b->cond));
	pthread_mutex_unlock(&(b->mutex));

}
	
void dequeue(circular_buf* b, char* name, int* value) {

	pthread_mutex_lock(&(b->mutex));
	while (b->counter == 0) {

		pthread_cond_wait(&(b->cond), &(b->mutex));

	}
	*name = (b->buf + b->tail)->c;
	*value = (b->buf + b->tail)->i;
	b->tail = (b->tail + 1) % CAPACITY;
	--(b->counter);
	pthread_cond_broadcast(&(b->cond));
	pthread_mutex_unlock(&(b->mutex));

}

void* producer(void* p) {
	
	circular_buf* b = (circular_buf*) p;
	char name = 'P';
	for (int i = 0; i < 1000; ++i) {

		enqueue(b, name, i);
		printf("Producer enqueuing: %c, %i\n", name, i);

	}
	return p;

}

void* consumer(void* p) {

	circular_buf* b = (circular_buf*) p;
	char name;
	int value;
	for (int i = 0; i < 1000; ++i) {

		dequeue(b, &name, &value);
		printf("Consumer dequeueing: %c, %i\n", name, value);

	}
	return p;

}


int main() {

	setvbuf(stdout, NULL, _IONBF, 0);
	printf("Beginning program\n");

	elem storage[CAPACITY];
	circular_buf b;
	circular_buf_init(&b, storage);

	pthread_create(&p1, NULL, &producer, &b);
	pthread_create(&c1, NULL, &consumer, &b);

	pthread_join(p1, NULL);
	pthread_join(c1, NULL);

}












