/*
 * rain_type.h
 *
 *  Created on: 2013-1-16
 *      Author: wd
 */

#ifndef RAIN_TYPE_H_
#define RAIN_TYPE_H_
#include <stdint.h>
typedef int32_t rain_routine_t;
typedef int32_t rain_session_t;
enum
{
	RAIN_OK,
	RAIN_ERROR
};
enum{
	RAIN_TCP_READ,
	RAIN_TCP_WRITE,
	RAIN_TCP_ACCEPT
};
//以下三个函数千万不能调用阻塞函数。
struct rain_ctx;
typedef void *(*rain_new_fn)(struct rain_ctx*ctx,const char *args);
typedef void(*rain_del_fn)(void *env,int code);


struct rain_msg
{
	void *data;
	int sz;
	short type;
};
typedef struct rain_tcp rain_tcp_t;
typedef struct rain_ctx rain_ctx_t;
typedef void(*rain_recv_msg_fn)(void *env,rain_routine_t src,struct rain_msg msg,rain_session_t session);
typedef void (*rain_link_fn)(void *env,rain_routine_t exitid,int code);
typedef void(*rain_timeout_fn)(void *env,void* user_data);
typedef void (*rain_tcp_fn)(void *env,rain_tcp_t *tcp,int event);

#endif /* RAIN_TYPE_H_ */
