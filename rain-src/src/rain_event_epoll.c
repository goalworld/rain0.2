/*
 * rain_event_epoll.c
 *
 *  Created on: 2013-3-11
 *      Author: wd
 */
#include "rain_event.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <memory.h>
#include <assert.h>
#include "rain_loger.h"
#define EPOLL_EVENT_LIST 10240
static int rain_epoll_init();
static int rain_epoll_add(rain_event_t * ev,wod_socket_t fd,int events);
static int rain_epoll_del(rain_event_t * ev,wod_socket_t fd,int events);
static int rain_epoll_process(int ms_timeout);
static void rain_epoll_destroy();
rain_event_action_t rain_event_action = {
		rain_epoll_init,
		rain_epoll_add,
		rain_epoll_del,
		rain_epoll_process,
		rain_epoll_destroy
};
static int ep = -1;
static struct epoll_event  *event_list = NULL;
static int
rain_epoll_init()
{
	ep = epoll_create(1024);
	if( ep < 0){
		return -errno;
	}
	event_list = malloc(sizeof(struct epoll_event) * EPOLL_EVENT_LIST);
	assert(event_list);
	memset(event_list,0,sizeof(struct epoll_event) * EPOLL_EVENT_LIST);

	RAIN_LOG(0,"%s","epoll init");
	return 0;
}
static int
rain_epoll_add(rain_event_t * ev,wod_socket_t fd,int events)
{
	struct epoll_event ee;
	ee.data.ptr = ev;
	events |= ~ev->events;
	ee.events = 	(events & RAIN_EVENT_READ) ? EPOLLIN:0 |
			(events & RAIN_EVENT_WRITE) ? EPOLLOUT:0 ;
	int ret = -1;
	if(ee.events != 0){
		ee.events |= EPOLLET;
		ret = epoll_ctl(ep,EPOLL_CTL_ADD,fd,&ee);
		if( ret < 0 && errno == EEXIST){
			ret = epoll_ctl(ep,EPOLL_CTL_MOD,fd,&ee);
		}
	}
	if(ret >= 0){
		ev->events = events;
		return 0;
	}else{
		return -errno;
	}
}
static int
rain_epoll_del(rain_event_t * ev,wod_socket_t fd,int events)
{
	struct epoll_event ee;
	ee.data.ptr = ev;
	events = events  & ev->events;
	ev->events = events;
	ee.events = 	(events & RAIN_EVENT_READ) ? EPOLLIN:0 |
			(events & RAIN_EVENT_WRITE) ? EPOLLOUT:0 ;
	int ret = -1;
	if(ee.events != 0){
		ee.events |= EPOLLET;
		ret = epoll_ctl(ep,EPOLL_CTL_MOD,fd,&ee);
	}else{
		ret = epoll_ctl(ep,EPOLL_CTL_DEL,fd,&ee);
	}
	if(ret >= 0){
		ev->events = events;
		return 0;
	}else{
		return -errno;
	}
}
static int
rain_epoll_process(int ms_timeout)
{
		int ret;
		int i;
		while(1){
			 ret = epoll_wait(ep,event_list,EPOLL_EVENT_LIST,ms_timeout);
			 if(ret < 0 && (errno == EINTR)) continue;
			 else break;
		}
		//lock using id;
		if(ret > 0){
			for(i=0; i<ret; i++){
				rain_event_read((rain_event_t *)event_list[i].data.ptr);
			}
		}
		return 0;
}
static void
rain_epoll_destroy()
{
	if(event_list)
		free(event_list);
	if(ep != -1){
		close(ep);
	}
}
