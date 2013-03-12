/*
 * rain_tcp.h
 *
 *  Created on: 2013-3-11
 *      Author: wd
 */

#ifndef RAIN_TCP_H_
#define RAIN_TCP_H_
#include "rain.h"
#include <wod_net.h>
#include <wod_cyclebuffer.h>
#include "rain_event.h"
struct rain_tcp{
	wod_socket_t sock;
	rain_ctx_t * ctx;
	wod_cycle_buffer_t rdbuf;
	int err;
	int sz;
	rain_event_t rev;
	rain_event_t wev;
	int ref;
	unsigned blisten:1;
};
void rain_tcp_init();
int rain_tcp_real_read(rain_tcp_t * tcp);

rain_tcp_t * rain_tcp_get_by_handle(int handle);
void rain_tcp_unref(rain_tcp_t *tcp);
#endif /* RAIN_TCP_H_ */
