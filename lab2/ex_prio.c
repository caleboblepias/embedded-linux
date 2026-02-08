#include <stdio.h>
#include <pthread.h>
#include <string.h>

static pthread_t main_id;
static pthread_t thread1_id;
static pthread_t thread2_id;
static pthread_t thread3_id;

static void* body(void* arg) {

	char* string = *(char**) arg;

	for (int i = 0; i < 10; ++i) {

		for (int z = 0; z < i; ++z) {

			int counter = 0;
			for (int k = 0; k < 1000000; ++k) { ++counter; }
			printf("%s ", string);		
		}
		printf("\n");
	}
	return (void*) 1;


}



int main(int argc, char *argv[]) {
	
	// disable stdout buffering
	setvbuf(stdout, NULL, _IONBF, 0);

	char* string1;
	char* string2;
	char* string3;
	struct sched_param mainparam;
	struct sched_param param1;
	struct sched_param param2;
	struct sched_param param3;
	int err1;
	int err2;
	int err3;
	void* returnvalue;
	
	pthread_attr_t attr1;
	pthread_attr_t attr2;
	pthread_attr_t attr3;
	
	// handle main settings
	main_id = pthread_self();
	mainparam.sched_priority = 99;
	pthread_setschedparam(main_id, SCHED_FIFO, &mainparam);

	// initialize attributes
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);
	pthread_attr_init(&attr3);

	// remove scheduler type inheritance
	pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setinheritsched(&attr3, PTHREAD_EXPLICIT_SCHED);

	// set scheduler policy
	pthread_attr_setschedpolicy(&attr1, SCHED_FIFO);
	pthread_attr_setschedpolicy(&attr2, SCHED_FIFO);
	pthread_attr_setschedpolicy(&attr3, SCHED_FIFO);

	// set priority
	param1.sched_priority = 1;
	param2.sched_priority = 10;
	param3.sched_priority = 10;
	
	pthread_attr_setschedparam(&attr1, &param1);
	pthread_attr_setschedparam(&attr2, &param2);
	pthread_attr_setschedparam(&attr3, &param3);

	// thread creation
	string1 = "A";
	string2 = "B";
	string3 = "C";

	pthread_create(&thread1_id, &attr1, body, (void*) &string1);
	pthread_create(&thread2_id, &attr2, body, (void*) &string2);
	pthread_create(&thread3_id, &attr3, body, (void*) &string3);

	// destroy attributes
	pthread_attr_destroy(&attr1);
	pthread_attr_destroy(&attr2);
	pthread_attr_destroy(&attr3);

	// join main with each thread
	pthread_join(thread1_id, &returnvalue);
	pthread_join(thread2_id, &returnvalue);
	pthread_join(thread3_id, &returnvalue);

	return 1;
}






