/*
 * An example of how to use signals to implement a timer facility using
 * POSIX threads.
 *
 * Rationale: when using normal Unix signals (SIGALRM), the signal handler executes
 * in a signal handler context.  In a signal handler context, only async-signal-safe
 * functions may be called.  Few POSIX functions are async-signal-safe.
 *
 * Instead of handling the timer activity in the signal handler function,
 * we create a separate thread to perform the timer activity.
 * This timer thread receives a signal from a semaphore, which is signaled ("posted")
 * every time a timer signal arrives.
 *
 * You'll have to redefine "timer_func" below and "set_periodic_timer."
 *
 * Read the man pages of the functions used, such as signal(2), signal(7),
 * alarm(2).
 *
 * Written by Godmar Back <gback@cs.vt.edu>, February 2006.
 */

/* required header files */
#include <pthread.h>
#include <semaphore.h>  /* semaphore operations */
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>

/* */
static void timer_func();
static void timer2_func();
static void wait_next_activation(void);

/* */
static sem_t timer_sem;		/* */
static unsigned int timer_stopped;	/*  */
static pthread_t timer_thread;	/* */

static unsigned int timer2_stopped;
static pthread_t timer2_thread;
static struct timespec r;
static int period;

void timespec_add_us(struct timespec* r, uint64_t us) {

	r->tv_nsec += us * 1000;
	while (r->tv_nsec >= 1000000000L) {
		r->tv_nsec -= 1000000000L;
		r->tv_sec++;
	}

}

static void wait_next_activation(void) {

	clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &r, NULL);
	timespec_add_us(&r, period);

}

static void* timer2thread(void*) {
	
	struct timespec time;

	while (!timer2_stopped) {
		
		clock_gettime(CLOCK_REALTIME, &time);
		printf("Time (nanosleep): %ld.%09ld\n", time.tv_sec, time.tv_nsec);
		timer2_func();
		wait_next_activation();

	}
	return 0;

}

int start_periodic_timer(uint64_t offs, int t) {

	clock_gettime(CLOCK_REALTIME, &r);
	timespec_add_us(&r, offs);
	period = t;
	pthread_create(&timer2_thread, (pthread_attr_t*)0, timer2thread, (void*)0);
	return 0;

}

/* */
static void
timersignalhandler(int sig)
{
	/*  */
	sem_post(&timer_sem);	/*  */
}

/* */
static void *
timerthread(void *_)
{
	struct timespec time;
	while (!timer_stopped) {
		int rc = sem_wait(&timer_sem);		/* */
		if (rc == -1 && errno == EINTR)     /* */
		    continue;
		if (rc == -1) {                     /* */
		    printf("timerthread failed on sem_wait\n");
		    exit(-1);
		}
		clock_gettime(CLOCK_REALTIME, &time);
		printf("Time (signal): %ld.%09ld\n", time.tv_sec, time.tv_nsec);
		timer_func();	/* */
	}
	return 0;
}

/*  */
void
init_timer(void)
{
	/* */
	sem_init(&timer_sem, /**/ 0, /**/0); /*  */
	pthread_create(&timer_thread, (pthread_attr_t*)0, timerthread, (void*)0); /*  */
	signal(SIGALRM, timersignalhandler); /* */
}

/* */
void
shutdown_timer()
{
	timer_stopped = 1;
	sem_post(&timer_sem); /* */
	pthread_join(timer_thread, 0); /*  */
}

void
shutdown_timer2()
{
	timer2_stopped = 1;
	pthread_join(timer2_thread, 0); /*  */
}

/*  */
void
set_periodic_timer(long delay)
{
	struct itimerval tval = { /* */
		/*  */ .it_interval = { .tv_sec = 0, .tv_usec = delay },
		/* */       .it_value = { .tv_sec = 0, .tv_usec = delay }
	};

	setitimer(ITIMER_REAL, &tval, (struct itimerval*)0); /*  */
}

/* end provided code part */
/**/

static sem_t demo_over;	/* */
static sem_t demo2_over;

/* */
static void
timer_func()
{
	/* */
	static int i = 0;

	printf ("Timer (signal) called %d!\n", i);
	if (++i == 100) {
		shutdown_timer();
		sem_post(&demo_over);	/*  */
	}
}

static void
timer2_func()
{
	/* */
	static int i = 0;

	printf ("Timer (nanosleep) called %d!\n", i);
	if (++i == 100) {
		shutdown_timer2();
		sem_post(&demo2_over);	/*  */
	}
}

int
main()
{
	sem_init(&demo_over, /**/ 0, /**/0);
	sem_init(&demo2_over, 0, 0);
	init_timer();
	set_periodic_timer(/**/200000);
	start_periodic_timer(200000, 200000);
	/* */
	while (sem_wait(&demo_over) == -1 && sem_wait(&demo_over) == -1 && errno == EINTR)
	    continue;
	return 0;
}
