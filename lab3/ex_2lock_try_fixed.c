#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

static pthread_mutex_t mutex1;
static pthread_mutex_t mutex2;

static pthread_t t1;
static pthread_t t2;
static pthread_t main_id;

static void computeTimeout(struct timespec* ts, long ms) {
	clock_gettime(CLOCK_REALTIME, ts);
	ts->tv_sec += ms / 1000;
	ts->tv_nsec += (ms % 1000) * 1000000;
	if (ts->tv_nsec >= 1000000000) {
		ts->tv_sec++;
		ts->tv_nsec -= 1000000000;
	}
}

static void* bodyA(void* p) {
	
	/*
	printf("Entered bodyA\n");
	pthread_mutex_lock(&mutex1);
	printf("Entered mutex1 in bodyA\n");
	int counter = 0;
	for (int i = 0; i < 1000000; ++i) { ++counter; }
	pthread_mutex_lock(&mutex2);
	printf("Entered mutex2 in bodyA\n");
	counter = 0;
	for (int i = 0; i < 1000000; ++i) { ++counter; }
	pthread_mutex_unlock(&mutex2);
	printf("Exited mutex2 in bodyA\n");
	counter = 0;
	for (int i = 0; i < 1000000; ++i) { ++counter; }
	pthread_mutex_unlock(&mutex1);
	printf("Exited mutex1 in bodyA\n");
	counter = 0; 
	for (int i = 0; i < 1000000; ++i) { ++counter; }

	return p;
	*/
	printf("Entered bodyA\n");
	while (1) {
		pthread_mutex_lock(&mutex1);
		printf("Entered mutex1 in bodyA\n");
		int counter = 0;
		for (int i = 0; i < 1000000; ++i) { ++counter; }
		struct timespec ts;
		computeTimeout(&ts, 1000);
		if (pthread_mutex_timedlock(&mutex2, &ts) == 0) {
			printf("Entered mutex2 in bodyA\n");
			counter = 0;
			for (int i = 0; i < 1000000; ++i) { ++counter; }
			pthread_mutex_unlock(&mutex2);
			printf("Exited mutex2 in bodyA\n");
			counter = 0;
			for (int i = 0; i < 1000000; ++i) { ++counter; }
			pthread_mutex_unlock(&mutex1);
			printf("Exited mutex1 in bodyA\nbodyA success\n");
			break;
		}
		printf("mutex2 lock timeout in bodyA\n");
		pthread_mutex_unlock(&mutex1);
		printf("Exited mutex1 in bodyA\nRestarting bodyA\n");
		counter = 0; 
		for (int i = 0; i < 1000000; ++i) { ++counter; }
	}
	return p;
}

static void* bodyB(void* p) {
	printf("Entered bodyB\n");
	while (1) {
		pthread_mutex_lock(&mutex1);
		printf("Entered mutex1 in bodyB\n");
		int counter = 0;
		for (int i = 0; i < 1000000; ++i) { ++counter; }
		struct timespec ts;
		computeTimeout(&ts, 1000);
		if (pthread_mutex_timedlock(&mutex2, &ts) == 0) {
			printf("Entered mutex2 in bodyB\n");
			counter = 0;
			for (int i = 0; i < 1000000; ++i) { ++counter; }
			pthread_mutex_unlock(&mutex2);
			printf("Exited mutex2 in bodyB\n");
			counter = 0;
			for (int i = 0; i < 1000000; ++i) { ++counter; }
			pthread_mutex_unlock(&mutex1);
			printf("Exited mutex1 in bodyB\nbodyB success\n");
			break;
		}
		printf("mutex2 lock timeout in bodyB\n");
		pthread_mutex_unlock(&mutex1);
		printf("Exited mutex1 in bodyB\nRestarting bodyB\n");
		counter = 0; 
		for (int i = 0; i < 1000000; ++i) { ++counter; }
	}
	return p;

}

int main(int argc, char* argv[]) {

	// disable stdout buffering
	setvbuf(stdout, NULL, _IONBF, 0);
	printf("Beginning program\n");

	int err1;
	int err2;

	// mutex creation
	
	pthread_mutexattr_t mutexattr1;
	pthread_mutexattr_t mutexattr2;

	err1 = pthread_mutexattr_init(&mutexattr1);
	err2 = pthread_mutexattr_init(&mutexattr2);
	if (err1 || err2) {
		printf("Error initializing mutex attributes");
	}

	err1 = pthread_mutex_init(&mutex1, &mutexattr1);
	err2 = pthread_mutex_init(&mutex2, &mutexattr2);
	if (err1 || err2) {
		printf("Error initializing mutexes");
	}

	pthread_mutexattr_destroy(&mutexattr1);
	pthread_mutexattr_destroy(&mutexattr2);

	// thread creation
	
	struct sched_param param1;
	struct sched_param param2;
	struct sched_param mainparam;

	char* string1;
	char* string2;

	void* returnvalue;

	pthread_attr_t attr1;
	pthread_attr_t attr2;

	// handle main settings
	
	main_id = pthread_self();
	mainparam.sched_priority = 99;
	pthread_setschedparam(main_id, SCHED_RR, &mainparam);

	// handle t1, t2
	
	err1 = pthread_attr_init(&attr1);
	err2 = pthread_attr_init(&attr2);
	if (err1 || err2) {
		printf("Error initializing thread attributes");
	}

	err1 = pthread_attr_setinheritsched(&attr1, PTHREAD_EXPLICIT_SCHED);
	err2 = pthread_attr_setinheritsched(&attr2, PTHREAD_EXPLICIT_SCHED);
	if (err1 || err2) {
		printf("Error setting inheritance of scheduling");
	}

	err1 = pthread_attr_setschedpolicy(&attr1, SCHED_RR);
	err2 = pthread_attr_setschedpolicy(&attr2, SCHED_RR);
	if (err1 || err2) {
		printf("Error initializing thread attributes");
	}

	param1.sched_priority = 1;
	param2.sched_priority = 1;
				
	err1 = pthread_attr_setschedparam(&attr1, &param1);
	err2 = pthread_attr_setschedparam(&attr2, &param2);
	if (err1 || err2) {
		printf("Error setting schedparams");
	}

	string1 = "A";
	string2 = "B";

	err1 = pthread_create(&t1, &attr1, bodyA, (void*) &string1);
	err2 = pthread_create(&t2, &attr2, bodyB, (void*) &string2);

	if (err1 || err2) {
		printf("Error creating threads: %i\n", err1);
	}

	pthread_attr_destroy(&attr1);
	pthread_attr_destroy(&attr2);

	pthread_join(t1, &returnvalue);
	pthread_join(t2, &returnvalue);
	

	return 1;






























}
