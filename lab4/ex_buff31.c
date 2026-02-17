#include <stdio.h>
#include <pthread.h>
#include <stdint.h>

#define CAPACITY 16

static pthread_t p1;
static pthread_t p2;
static pthread_t p3;
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

void* producer1(void* p) {
	
	circular_buf* b = (circular_buf*) p;
	char name = '1';
	for (int i = 0; i < 1000; ++i) {

		enqueue(b, name, i);
		printf("Producer1 enqueuing: %c, %i\n", name, i);

	}
	return p;

}

void* producer2(void* p) {
	
	circular_buf* b = (circular_buf*) p;
	char name = '2';
	for (int i = 0; i < 1000; ++i) {

		enqueue(b, name, i);
		printf("Producer2 enqueuing: %c, %i\n", name, i);

	}
	return p;

}

void* producer3(void* p) {
	
	circular_buf* b = (circular_buf*) p;
	char name = '3';
	for (int i = 0; i < 1000; ++i) {

		enqueue(b, name, i);
		printf("Producer3 enqueuing: %c, %i\n", name, i);

	}
	return p;

}

void* consumer(void* p) {

	circular_buf* b = (circular_buf*) p;
	char name;
	int value;
	int last_value1 = 0;
	int last_value2 = 0;
	int last_value3 = 0;
	int entry_count1 = 0;
	int entry_count2 = 0;
	int entry_count3 = 0;
	int error_count = 0;
	for (int i = 0; i < 3000; ++i) {

		dequeue(b, &name, &value);
		printf("Consumer dequeueing: %c, %i\n", name, value);
		if (name == '1') {
			if (value < last_value1) {
				++error_count;
				printf("Index ordering violated by Producer1\n");
			}
			last_value1 = value;
			++entry_count1;
		}
		else if (name == '2') {
			if (value < last_value2) {
				++error_count;
				printf("Index ordering violated by Producer2\n");
			}
			last_value2 = value;
			++entry_count2;
		}
		else if (name == '3') {
			if (value < last_value3) {
				++error_count;
				printf("Index ordering volated by Producer3\n");
			}
			last_value3 = value;
			++entry_count3;
		}
	}
	printf("Error count: %i\n", error_count);
	printf("Entry count 1: %i, Entry count 2: %i, Entry count 3: %i\n", entry_count1, entry_count2, entry_count3);
	return p;

}


int main() {

	setvbuf(stdout, NULL, _IONBF, 0);
	printf("Beginning program\n");

	elem storage[CAPACITY];
	circular_buf b;
	circular_buf_init(&b, storage);

	pthread_create(&p1, NULL, &producer1, &b);
	pthread_create(&p2, NULL, &producer2, &b);
	pthread_create(&p3, NULL, &producer3, &b);
	pthread_create(&c1, NULL, &consumer, &b);

	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(p3, NULL);
	pthread_join(c1, NULL);

	return 0;
}












