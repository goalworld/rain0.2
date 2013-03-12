/*
 * rain_list.h
 *
 *  Created on: 2013-3-11
 *      Author: wd
 */

#ifndef RAIN_LIST_H_
#define RAIN_LIST_H_
typedef struct rain_list_node{
	void * data;
	struct rain_list_node *next;
}rain_list_node_t;
typedef struct rain_list{
	rain_list_node_t * head;
}rain_list_t;
typedef int (*rian_list_data_equal)(void *data1,void *data2);
void rain_list_init(rain_list_t * list);
void rain_list_insert(rain_list_t * list,void *data);
int rain_list_remove(rain_list_t * list,void *data,rian_list_data_equal eq);
void rain_list_destroy(rain_list_t *list);
#endif /* RAIN_LIST_H_ */
