/*
 * test_main.c
 *
 *  Created on: 2012-11-12
 *      Author: goalworld
 */

#include <rain.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct test_s{
	struct rain_ctx* ctx;
	long recvsize;
	int cli;
	rain_routine_t tcpsvr_id,jsv8_test_id;
	rain_tcp_t * tcp;
	int x;
}test_t;
static void _recv(void *arg,rain_routine_t src,struct rain_msg msg,rain_session_t session);
static void _recv_rsp(void *arg,rain_routine_t src,struct rain_msg msg,rain_session_t session);
static void _time_out(void *env,void *userdata);
static void _link_exit(void *env,rain_routine_t exitid,int code);
static void _tcp_event(void *env,rain_tcp_t * tcp,int ev);
void *
test_new(struct rain_ctx *ctx,char *args)
{
	test_t * tt = malloc(sizeof(test_t));
	tt->ctx = ctx;
	tt->recvsize = 0;
	tt->cli = 0;
	rain_debug(tt->ctx,"TestRunning,arguments:%s",args);
	fflush(stdout);
	RAIN_CALLBACK(ctx,_recv,_recv_rsp,_link_exit,_time_out,NULL,_tcp_event);
	rain_timeout(ctx,60*1000,NULL);
	tt->tcp = rain_tcp_listen(ctx,"0.0.0.0",8194);
	tt->x = 0;
	return tt;
}
void
test_delete(void *env,int code)
{
	if(!env){
		return;
	}
	test_t * tt = (test_t*)env;
	rain_tcp_close(tt->ctx,tt->tcp);
	free(env);
}
static void
_time_out(void *env,void *userdata)
{
	test_t * tt = (test_t*)env;
	rain_debug(tt->ctx,"_time_out");
}
static void _tcp_event(void *env,rain_tcp_t * tcp,int ev)
{
	test_t * tt = (test_t*)env;
	if(ev == RAIN_TCP_ACCEPT){
		rain_tcp_t * conn = rain_tcp_accept(tt->ctx,tcp);
	}else if(ev == RAIN_TCP_READ){
		for(;;){
			unsigned int head = 0;
			int ret = rain_tcp_read(tt->ctx,tcp,&head,4);
			if(ret == 4){
				head = htonl(head);
				printf("head %d ||||",head);
				char buf[head+1];
				ret = rain_tcp_read(tt->ctx,tcp,buf,head);
				if(ret == head){
					buf[head] = 0;
					printf("body %s %d \n",buf,tt->x++);
				}else{
					break;
				}
			}else{
				break;
			}
		}
	}
}
/*
 * char buf[head+1];
			int ret = rain_tcp_read(tt->ctx,tcp,&head,head);
			buf[head] = 0;
			rain_debug(tt->ctx,"body %s",buf);
 */
static void
_link_exit(void *env,rain_routine_t exitid,int code)
{
}
static void
_recv(void *env,rain_routine_t src,struct rain_msg msg,rain_session_t session)
{
	test_t * tt = (test_t*)env;
	tt->recvsize +=msg.sz;
	if(msg.type == 1){
		tt->cli++;
	}
	free(msg.data);
}
static void
_recv_rsp(void *arg,rain_routine_t src,struct rain_msg msg,rain_session_t session)
{
	char buf[msg.sz+1];
	memcpy(buf,msg.data,msg.sz);
	buf[msg.sz] = 0x00;
	printf("WATCHDOG-RSP:%s,%d\n",buf,msg.sz);
	fflush(stdout);
	free(msg.data);
}

