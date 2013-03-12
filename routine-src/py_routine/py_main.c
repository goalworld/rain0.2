/*
 * py_main.c
 *
 *  Created on: 2013-3-3
 *      Author: wd
 */
#include <rain.h>
#include <stddef.h>
#include <python/Python.h>
typedef struct py_routine
{

}py_routine_t;
void *
py_new(struct rain_ctx*ctx,void *args)
{
	void * env = malloc(sizeof(py_routine_t));
	Py_Initialize();

	Py_Finalize();
	return NULL;
}

void
py_delete(void *env)
{
	free(env);
}
