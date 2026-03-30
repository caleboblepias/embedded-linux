//  Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main (void)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);	/* Create socket */
    int rc = zmq_bind (responder, "tcp://*:5555");		/* Bind socket on all interfaces to port 5555 */
    assert (rc == 0);
    int id;
    char msg[10 + sizeof(int)];
    while (1) {											/* Loop forever */
        char buffer [10];
        zmq_recv (responder, buffer, 10, 0);
	sscanf (buffer, "%d", &id);
        printf ("Received ID: %d Hello\n", id);
        sleep (1);          //  Do some 'work'
	sprintf (msg, "ID: %d World", id);
        zmq_send (responder, msg, 5, 0);			/* Send 5 byte response */
    }
    return 0;
}
