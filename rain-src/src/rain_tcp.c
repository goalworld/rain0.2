/*
 * rain_tcp.c
 *
 *  Created on: 2013-3-11
 *      Author: wd
 */

#include "rain.h"
#include "rain_tcp.h"
#include "errno.h"
#include <stdlib.h>
#include "rain_mutex.h"
#include <assert.h>
/**
 * return>0:缓冲区空了
 * return=0:读到结尾。
 * return<0:出错了
 */
#define TCP_SZ 10240
static rain_tcp_t* tcp_map[TCP_SZ];
static rain_mutex_t tcp_mtx;
void rain_tcp_init()
{
	rain_mutex_init(&tcp_mtx);
	memset(tcp_map,0,sizeof(void *)*TCP_SZ);
}
static rain_tcp_t *
rain_get_new_tcp(int fd)
{
	rain_tcp_t * tcp;
	if(fd >= TCP_SZ ){
		return NULL;
	}
	rain_mutex_lock(&tcp_mtx);
	tcp = tcp_map[fd];
	assert(tcp == NULL);
	tcp = malloc(sizeof(rain_tcp_t));
	tcp_map[fd] = tcp;
	tcp->ref = 1;
	rain_mutex_unlock(&tcp_mtx);
	return tcp;
}
void
rain_tcp_unref(rain_tcp_t * tcp)
{
	rain_mutex_lock(&tcp_mtx);
	tcp->ref --;
	if(tcp->ref == 0){
		tcp_map[tcp->sock] = NULL;
		free(tcp);
	}
	rain_mutex_unlock(&tcp_mtx);
}
rain_tcp_t * rain_tcp_get_by_handle(int handle)
{
	rain_mutex_lock(&tcp_mtx);
	rain_tcp_t * tcp = tcp_map[handle];
	tcp->ref ++;
	rain_mutex_unlock(&tcp_mtx);
	return tcp;
}
int
rain_tcp_real_read(rain_tcp_t * tcp)
{
	struct wod_cycle_pair pair;
	int fret = 0;
	for(;;){
		int ret = wod_cycle_buffer_get_unused(&tcp->rdbuf,&pair,512);
		if(ret != 0){
			fret = -1;
			break;
		}
		int rret = 0;
		int num=0;
		struct wod_socket_buf io[2];
		io[num].b_body = pair.first.buf;
		io[num].b_sz = pair.first.sz;
		++num;
		if(pair.second.sz > 0 ){
			io[1].b_body = pair.second.buf;
			io[1].b_sz = pair.second.sz;
			++num;
		}
		rret = wod_net_readv(tcp->sock,io,num);
		if(rret < 0){
			if(errno == EAGAIN){
				fret = 1;
				break;
			}else if(errno != EINTR){
				fret = -1;
				break;
			}
		}else if(rret == 0){
			fret = 0;
			break;
		}
		wod_cycle_buffer_grow(&tcp->rdbuf,rret);
	}
	return fret < 0 ? -errno:fret;
}
int
rain_tcp_read(struct rain_ctx *ctx,	rain_tcp_t *tcp,void *buf,size_t sz/*in out*/)
{
	if(ctx != tcp->ctx){
		return -EINVAL;
	}
	if(sz == 0){
		return -EINVAL;
	}
	int ret;
	struct wod_cycle_pair pair;
	ret = wod_cycle_buffer_get_used(&tcp->rdbuf,&pair);
	if (ret != 0 || wod_cycle_pair_sz(&pair) < sz){
		ret = rain_tcp_real_read(tcp);
		if(ret <= 0){
			tcp->err = ret;
		}
		wod_cycle_buffer_get_used(&tcp->rdbuf,&pair);
	}
	ret = wod_cycle_pair_readsz(&pair,buf,sz);
	if(ret < 0){
		if(wod_cycle_pair_sz(&pair) == 0){
			return tcp->err;
		}
		return -EAGAIN;
	}else{
		wod_cycle_buffer_pop(&tcp->rdbuf,sz);
		return sz;
	}

}
int
rain_tcp_write(struct rain_ctx *ctx,rain_tcp_t *tcp,void *buf,size_t sz)
{
	if(ctx != tcp->ctx){
		return -1;
	}
	return wod_net_write(tcp->sock,buf,sz);
}
int
rain_tcp_close(struct rain_ctx *ctx,rain_tcp_t *tcp)
{
	rain_ctx_removetcp(ctx,tcp);
	int ret = wod_net_close(tcp->sock);
	wod_cycle_buffer_destroy(&tcp->rdbuf);
	rain_tcp_unref(tcp);
	return ret;
}
rain_tcp_t * rain_tcp_listen(struct rain_ctx *ctx,const char *ip,int port)
{
	wod_socket_t sock = wod_net_tcp_listen(TCP4,ip,port);
	if(sock <= 0){
		return NULL;
	}
	wod_net_noblock(sock,1);
	rain_tcp_t *tcp = rain_get_new_tcp(sock);
	assert(tcp);

	tcp->ctx = ctx;
	tcp->sock = sock;
	tcp->rev.ev_data.handle = sock;
	tcp->rev.events = 0;
	tcp->wev.ev_data.handle = sock;
	tcp->wev.events = 0;
	tcp->blisten = 1;
	tcp->err = 1;
	wod_cycle_buffer_init(&tcp->rdbuf,1024);
	rain_event_add(&tcp->rev,sock,RAIN_EVENT_READ);
	rain_ctx_addtcp(ctx);
	return tcp;
}
rain_tcp_t * rain_tcp_accept(struct rain_ctx *ctx,rain_tcp_t *listcp)
{
	wod_socket_t sock = wod_net_accept(listcp->sock);
	if(sock <= 0){
		return NULL;
	}
	wod_net_noblock(sock,1);
	rain_tcp_t *tcp = rain_get_new_tcp(sock);
	assert(tcp);
	tcp->ctx = ctx;
	tcp->sock = sock;
	tcp->rev.ev_data.handle = sock;
	tcp->rev.events = 0;
	tcp->wev.ev_data.handle = sock;
	tcp->wev.events = 0;
	tcp->blisten = 0;
	tcp->err = 1;
	wod_cycle_buffer_init(&tcp->rdbuf,1024);
	rain_event_add(&tcp->rev,sock,RAIN_EVENT_READ);
	rain_ctx_addtcp(ctx);
	return tcp;
}
rain_tcp_t * rain_tcp_connect(struct rain_ctx *ctx,const char *ip,int port)
{
	wod_socket_t sock = wod_net_tcp_connect(TCP4,ip,port);
	if(sock <= 0){
		return NULL;
	}
	wod_net_noblock(sock,1);
	rain_tcp_t *tcp = rain_get_new_tcp(sock);
	assert(tcp);
	tcp->ctx = ctx;
	tcp->sock = sock;
	tcp->rev.ev_data.handle = sock;
	tcp->rev.events = 0;
	tcp->wev.ev_data.handle = sock;
	tcp->wev.events = 0;
	tcp->blisten = 0;
	tcp->err = 1;
	wod_cycle_buffer_init(&tcp->rdbuf,1024);
	rain_event_add(&tcp->rev,sock,RAIN_EVENT_READ);
	return tcp;
}
