/*
 * rain_ctx.h
 *
 *  Created on: 2012-11-9
 *      Author: goalworld
 */

#ifndef RAIN_CTX_H_
#define RAIN_CTX_H_
#include <stdbool.h>
#include "rain_type.h"
#include "rain_msg.h"
#include <wod_array.h>
#include "rain_mutex.h"
struct rain_ctx
{
	struct rain_message_queue * msgQue;
	struct wod_array arr;
	rain_mutex_t mtx;
	struct rain_moudle * mod;
	rain_routine_t rid;//const
	rain_routine_t prid;
	void * arg;
	rain_recv_msg_fn recv;
	rain_recv_msg_fn recv_rsp;
	rain_link_fn link;
	rain_timeout_fn timeoutfn;
	rain_timeout_fn nexttickfn;
	rain_tcp_fn 	tcp_fn;
	bool bmain;
	int ref;
	int bdis;
	int binit;
	bool bexit;
	int exit_code;
	rain_session_t session;
};
int rain_ctx_init(int rainid);
rain_ctx_t * rain_ctx_new(rain_routine_t prid,const char * mod,const char *args);
const char * rain_ctx_mod_name(rain_ctx_t *ctx);
rain_routine_t	 rain_ctx_get_id(rain_ctx_t *ctx);
rain_routine_t	 rain_ctx_get_pid(rain_ctx_t *ctx);
int rain_ctx_add_link(rain_ctx_t *ctx,rain_routine_t rid);
rain_session_t rain_ctx_genter_session(rain_ctx_t *ctx);
void rain_ctx_addtcp( rain_ctx_t *ctx,rain_tcp_t *tcp);
void rain_ctx_removetcp(rain_ctx_t *ctx,rain_tcp_t *tcp);
int rain_ctx_run(rain_ctx_t *ctx);
int rain_ctx_push_message(rain_ctx_t *ctx,struct rain_ctx_message msg);
void rain_ctx_ref(rain_ctx_t *ctx);
void rain_ctx_unref(rain_ctx_t *ctx);

rain_ctx_t * rain_handle_query_ctx(rain_routine_t rid,bool blog);
int rain_handle_push_message(rain_routine_t dest, struct rain_ctx_message msg);
int rain_handle_get_localid(rain_routine_t rid);
int rain_handle_kill(rain_routine_t rid,int code);

#endif /* RAIN_CTX_H_ */
