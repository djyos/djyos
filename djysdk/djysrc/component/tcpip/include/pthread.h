/*
 * pthread.h
 *
 *  Created on: 2015Äê4ÔÂ25ÈÕ
 *      Author: zhangqf
 */

#ifndef __PTHREAD_H
#define __PTHREAD_H

#include <stdint.h>
#include <stddef.h>
#include <os.h>
#include <sys/time.h>

//pthread.h
typedef unsigned int  pthread_t;
typedef unsigned int pthread_attr_t;

#define pthread_mutex_t(mutex)  struct MutexLCB mutex
bool_t pthread_mutex_lock(struct MutexLCB *mutex);
bool_t pthread_mutex_unlock(struct MutexLCB *mutex);
bool_t pthread_mutex_init(struct MutexLCB *mutex, const char *name);
bool_t pthread_mutex_destroy(struct MutexLCB *mutex);

#define pthread_cond_t(cond)   int cond
bool_t  pthread_cond_signal(int *cond);
bool_t  pthread_cond_wait(int *cond, struct MutexLCB *mutex);
bool_t  pthread_cond_init(int *cond,char *name);
bool_t  pthread_cond_destroy(int *cond);

int pthread_create(pthread_t  *threadId, const pthread_attr_t *attr,\
       ptu32_t (*taskroutine)(void ),void *arg);



#endif /* __PTHREAD_H */
