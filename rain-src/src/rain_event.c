/*
 * rain_event.c
 *
 *  Created on: 2013-3-11
 *      Author: wd
 */

#include "rain_event.h"
#include "rain_tcp.h"
#include "rain_context.h"
#include "rain_msg.h"

void
rain_event_init()
{
	rain_event_action.init();
}

void rain_event_read(rain_event_t *ev)
{
	rain_tcp_t * p = rain_tcp_get_by_handle(ev->ev_data.handle);
	if(p){
		struct rain_ctx_message msg;
		msg.type = RAIN_MSG_TCP;
		msg.u_sz.tcpstate = p->blisten?RAIN_TCP_ACCEPT:RAIN_TCP_READ;
		msg.u_data.tcp_data = p;
		rain_ctx_push_message(p->ctx,msg);
		rain_tcp_unref(p);
	}

}
void rain_event_write(rain_event_t *ev)
{
	rain_tcp_t * p = rain_tcp_get_by_handle(ev->ev_data.handle);
	if(p){
		struct rain_ctx_message msg;
		msg.type = RAIN_MSG_TCP;
		msg.u_sz.tcpstate = RAIN_TCP_WRITE;
		msg.u_data.tcp_data = p;
		rain_ctx_push_message(p->ctx,msg);
		rain_tcp_unref(p);
	}

}
