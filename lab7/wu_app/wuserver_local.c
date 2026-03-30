//  Weather update client
//  Connects SUB socket to tcp://localhost:5556
//  Collects weather updates and finds avg temp in zipcode

#include "zhelpers.h"

int main (int argc, char *argv [])
{
    //  Socket to talk to wuserver_main
    printf ("Collecting updates from weather server…\n");
    void *context = zmq_ctx_new ();
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    int rc = zmq_connect (subscriber, "tcp://localhost:5556");
    assert (rc == 0);

    //  Subscribe to zipcode, default is NYC, 10001
    char *filter = (argc > 1)? argv [1]: "10001 ";
    rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, filter, strlen (filter));
    assert (rc == 0);

    // Socket to talk to wuclient_local
    void* responder = zmq_socket (context, ZMQ_REP);
    rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);

    // Poll
    int update_nbr;
    long total_temp = 0;

    while(1) {
    
	zmq_pollitem_t items [] = {
		
		{ subscriber, 0, ZMQ_POLLIN, 0 },
		{ responder, 0, ZMQ_POLLIN, 0 }

	};

	zmq_poll (items, 2, -1);

	if (items[0].revents & ZMQ_POLLIN) {
		char* string = s_recv(subscriber);
		int zipcode, temperature, relhumidity, sequence_count;
		sscanf(string, "%d %d %d %d", &zipcode, &temperature, &relhumidity, &sequence_count);
		total_temp += temperature;
		update_nbr++;
		free(string);
		printf("Received sequence_count: %d\n", sequence_count);



	}
	if (items[1].revents & ZMQ_POLLIN) {
		char msg[10 + sizeof(int)];
		sprintf(msg, "Average: %ld", total_temp / update_nbr);
		printf("Sending average\n");
		zmq_send(responder, msg, 10 + sizeof(int), 0);

	}

    }

    zmq_close (subscriber);
    zmq_ctx_destroy (context);
    return 0;
}
