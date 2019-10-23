/*
 * XXX This sample code was once meant to show how to use the basic Libevent
 * interfaces, but it never worked on non-Unix platforms, and some of the
 * interfaces have changed since it was first written.  It should probably
 * be removed or replaced with something better.
 *
 * Compile with:
 * cc -I/usr/local/include -o time-test time-test.c -L/usr/local/lib -levent
 */

#include <sys/types.h>
#include <event2/event-config.h>
#include <sys/stat.h>
#include <time.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>

#include <iostream>
#include "base/proto_time.h"

using namespace std;
using namespace base;
uint32_t last_time=0;

int event_is_persistent;

static void
timeout_cb(evutil_socket_t fd, short event, void *arg)
{
	uint32_t newtime;
	struct event *timeout = static_cast<struct event*>(arg);
	uint32_t elapsed;

	newtime=base::TimeMillis();
	elapsed=newtime-last_time;
    std::cout<<"elapsed: "<<elapsed<<std::endl;
	last_time = newtime;

	if (! event_is_persistent) {
		struct timeval tv;
		evutil_timerclear(&tv);
		tv.tv_sec = 2;
		event_add(timeout, &tv);
	}
}

int
main(int argc, char **argv)
{
	struct event timeout;
	struct timeval tv;
	struct event_base *event_base;
	int flags;
	if (argc == 2 && !strcmp(argv[1], "-p")) {
		event_is_persistent = 1;
		flags = EV_PERSIST;
	} else {
		event_is_persistent = 0;
		flags = 0;
	}

	/* Initalize the event library */
	event_base = event_base_new();

	/* Initalize one event */
	event_assign(&timeout, event_base, -1, flags, timeout_cb, (void*) &timeout);

	evutil_timerclear(&tv);
	tv.tv_sec = 2;
	event_add(&timeout, &tv);
    last_time=base::TimeMillis();
	event_base_dispatch(event_base);

	return (0);
}

