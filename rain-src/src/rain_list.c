/*
 * rain_list.c
 *
 *  Created on: 2013-3-11
 *      Author: wd
 */


#include "rain_list.h"
#include <stddef.h>
void
rain_list_init(rain_list_t * list)
{
	list->head = NULL;
}
void
rain_list_insert(rain_list_t * list,void *data)
{
	rain_list_node_t **pphead = &list->head;
	rain_list_node_t * p = malloc(sizeof(rain_list_node_t));
	p->data = data;
	if(!*pphead){
		p->next = (*pphead)->next;
	}
	*pphead = p;
}
int
rain_list_remove(rain_list_t * list,void *data,rian_list_data_equal eq)
{
	rain_list_node_t **pphead = &list->head;
	rain_list_node_t * cut;
	while(*pphead){
		cut = *pphead;
		if(ep(cut->data,data)){
			*pphead = cut->next;
			free(cut);
			return 0;
		}
		pphead = &cut->next;
	}while(cut);
	return -1;
}
void rain_list_destroy(rain_list_t *list)
{

}
