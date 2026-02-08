#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

static pthread_t main_id;
static pthread_t thread1_id;
static unsigned int val;

static void* body(void* arg) {
	#ifdef NOTNOW	
	unsigned int i;
	for (i = 0, val = 0; i < 1000000000; ++i) { val = +i; }
	return NULL;
	#endif
	#ifdef NOTNOW	
	char* string = *(char**)arg;

	for (int i = 0; i < 1000; ++i) {

		for (int z = 0; z < i; ++z) {

			int counter = 0;
			for (int k = 0; k < 1000000; ++k) { ++counter; }
			printf("%s ", string);

		}

		printf("\n");
	}
	
	return (void*) 1;
	#endif
	char* string = *(char**)arg;

	for (int i = 0; i < 100; ++i) {

		for (int z = 0; z < i; ++z) {

			int counter = 0;
			for (int k = 0; k < 100; ++k) { ++counter; }
			val = +i;
			printf("%d ", val);

		}

		printf("\n");
	}
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	for (int i = 0; i < 800; ++i) {

		for (int z = 0; z < i; ++z) {

			int counter = 0;
			for (int k = 0; k < 100; ++k) { ++counter; }
			val = +i;
			printf("%d ", val);

		}

		printf("\n");
	}
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	for (int i = 0; i < 100; ++i) {

		for (int z = 0; z < i; ++z) {

			int counter = 0;
			for (int k = 0; k < 100; ++k) { ++counter; }
			val = +i;
			printf("%d ", val);

		}

		printf("\n");
	}
	return (void*) 1;


}


int main(int argc, char *argv[]) {


	setvbuf(stdout, NULL, _IONBF, 0);


	char* string1;
	struct sched_param mainparam;
	struct sched_param param1;
	int err1;
	void* returnvalue;

	pthread_attr_t attr1;

	// main settings
	main_id = pthread_self();
	mainparam.sched_priority = 99;
	pthread_setschedparam(main_id, SCHED_FIFO, &mainparam);

	// thread 1 settings
	pthread_attr_init(&attr1);
	pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr1, SCHED_FIFO);
	param1.sched_priority = 1;
	pthread_attr_setschedparam(&attr1, &param1);
	string1 = "A";
	pthread_create(&thread1_id, &attr1, body, (void*) &string1);
	pthread_attr_destroy(&attr1);
	
	usleep(1000000);
	pthread_cancel(thread1_id);
	pthread_join(thread1_id, &returnvalue);
	
	printf("%d\n", val);
	return 1;


}

