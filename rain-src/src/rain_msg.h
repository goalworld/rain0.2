/*
 * rain_msg.h
 *
 *  Created on: 2013-1-16
 *      Author: wd
 */

#ifndef RAIN_MSG_H_
#define RAIN_MSG_H_
#include "rain_type.h"
enum{
	RAIN_MSG_REQ,
	RAIN_MSG_RSP,
	RAIN_MSG_TIMER,
	RAIN_MSG_NEXTTICK,
	RAIN_MSG_EXIT,
	RAIN_MSG_TCP
};
struct rain_ctx_message{
	rain_routine_t src;
	union{
		void *msg;
		void *tick_data;
		void *time_data;
		rain_tcp_t *tcp_data;
	}u_data;
	union{
		int sz;
		int fd;
		int timerid;
		int exitcode;
		int tcpstate;
	}u_sz;
	int type;
	rain_session_t session;
};

#endif /* RAIN_MSG_H_ */
