#ifndef Thread_Pool_HH
#define Thread_Pool_HH
#include "Config.h"
#ifndef WIN

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#ifndef TPBOOL
typedef int TPBOOL;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BUSY_THRESHOLD 0.5	//(busy thread)/(all thread threshold)
#define MANAGE_INTERVAL 5	//tp manage thread sleep interval

//typedef struct tp_work_desc_s tp_work_desc;
typedef struct tp_work_s tp_work;
typedef struct tp_thread_info_s tp_thread_info;
typedef struct tp_thread_pool_s tp_thread_pool;



//base thread struct
struct tp_work_s
{
	//main process function. user interface
	void (*process_job)(void * pParam);
};

//thread info
struct tp_thread_info_s
{
	pthread_t		thread_id;	//thread id num
	TPBOOL  		is_busy;	//thread status:true-busy;flase-idle
	pthread_cond_t          thread_cond;	
	pthread_mutex_t		thread_lock;
	tp_work			*th_work;
	void 		*m_pThreadParam;
};

//main thread pool struct
struct tp_thread_pool_s
{
	TPBOOL (*init)(tp_thread_pool *pthis);
	void (*close)(tp_thread_pool *pthis);
	void (*process_job)(tp_thread_pool *pthis, tp_work *worker, void  *pThreadParam);
	int  (*get_thread_by_id)(tp_thread_pool *pthis, pthread_t id);
	TPBOOL (*add_thread)(tp_thread_pool *pthis);
	TPBOOL (*delete_thread)(tp_thread_pool *pthis);
	int (*get_tp_status)(tp_thread_pool *pthis);
	
	int min_th_num;		//min thread number in the pool
	int cur_th_num;		//current thread number in the pool
	int max_th_num;         //max thread number in the pool
	pthread_mutex_t tp_lock;
	pthread_t manage_thread_id;	//manage thread id num
	tp_thread_info *thread_info;	//work thread relative thread info
};

tp_thread_pool *creat_thread_pool(int min_num, int max_num);

void *tp_work_thread(void *pthread);
void *tp_manage_thread(void *pthread);

 TPBOOL tp_init(tp_thread_pool *pthis);
 void tp_close(tp_thread_pool *pthis);
 void tp_process_job(tp_thread_pool *pthis, tp_work *worker, void  *pThreadParam );
int  tp_get_thread_by_id(tp_thread_pool *pthis, pthread_t id);
TPBOOL tp_add_thread(tp_thread_pool *pthis);
TPBOOL tp_delete_thread(tp_thread_pool *pthis);
int  tp_get_tp_status(tp_thread_pool *pthis);
tp_thread_pool *creat_thread_pool(int min_num, int max_num);
#endif


#endif
