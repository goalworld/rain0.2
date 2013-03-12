/*
 * rain_event.h
 *
 *  Created on: 2013-3-11
 *      Author: wd
 */
#ifndef _RAIN_EVENT_H_
#define _RAIN_EVENT_H_
#include <wod_net.h>
enum rain_event_type
{
	RAIN_EVENT_READ = 0X01,
	RAIN_EVENT_WRITE = 0X02,
	RAIN_EVENT_CLOSE = 0X04
};

typedef struct rain_event{
	int index;
	union{
		void *data;
		int handle;
	}ev_data;
	int events;
}rain_event_t;
typedef struct {
	int (*init)();
	int (*add)(rain_event_t * ev,wod_socket_t handle,int events);
	int (*del)(rain_event_t * ev,wod_socket_t handle,int events);
	int (*process)(int ms_timeout);
	void (*destroy)();
}rain_event_action_t;
rain_event_action_t  rain_event_action;
#define rain_event_add rain_event_action.add
#define rain_event_del rain_event_action.del
#define rain_event_process rain_event_action.process
void rain_event_init();
void rain_event_read(rain_event_t * ev);
void rain_event_write(rain_event_t * ev);

#endif

