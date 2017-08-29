#ifndef WIN

#include "thread-pool.h"



/**
  * user interface. creat thread pool.
  * para:
  * 	num: min thread number to be created in the pool
  * return:
  * 	thread pool struct instance be created successfully
  */
tp_thread_pool *creat_thread_pool(int min_num, int max_num)
{
	tp_thread_pool *pthis;
	pthis = (tp_thread_pool*)malloc(sizeof(tp_thread_pool));	
	//pthis = new tp_thread_pool;

	memset(pthis, 0, sizeof(tp_thread_pool));

	//init member function ponter
	pthis->init = tp_init;
	pthis->close = tp_close;
	pthis->process_job = tp_process_job;
	pthis->get_thread_by_id = tp_get_thread_by_id;
	pthis->add_thread = tp_add_thread;
	pthis->delete_thread = tp_delete_thread;
	pthis->get_tp_status = tp_get_tp_status;

	//init member var
	pthis->min_th_num = min_num;
	pthis->cur_th_num = pthis->min_th_num;
	pthis->max_th_num = max_num;
	pthread_mutex_init(&pthis->tp_lock, NULL);

	//malloc mem for num thread info struct
	if(NULL != pthis->thread_info)
	{
		free(pthis->thread_info);
	}
	pthis->thread_info = (tp_thread_info*)malloc(sizeof(tp_thread_info)*pthis->max_th_num);
	memset(pthis->thread_info,0,sizeof(tp_thread_info)*pthis->max_th_num);
	for(int i=0;i<pthis->max_th_num;i++)
	{
		pthis->thread_info[i].is_busy = 0;


	}

	return pthis;
}


/**
  * member function reality. thread pool init function.
  * para:
  * 	pthis: thread pool struct instance ponter
  * return:
  * 	true: successful; false: failed
  */
TPBOOL tp_init(tp_thread_pool *pthis)
{
	int i;
	int err;

	//creat work thread and init work thread info
	for(i=0;i<pthis->min_th_num;i++)
	{
		pthread_cond_init(&pthis->thread_info[i].thread_cond, NULL);
		pthread_mutex_init(&pthis->thread_info[i].thread_lock, NULL);

		err = pthread_create(&pthis->thread_info[i].thread_id, NULL, tp_work_thread, pthis);
		if(0 != err)
		{
#ifdef THREADPOOL_PRINTF
			printf("File:%s:%d\nFunction:%s\n%s\n",__FILE__, __LINE__, __FUNCTION__,  "tp_init: creat work thread failed\n");
#endif
			return FALSE;
		}
#ifdef THREADPOOL_PRINTF
		printf("File:%s:%d\nFunction:%s\n%s\nID:%d\n",__FILE__, __LINE__, __FUNCTION__,"tp_init: creat work thread \n", pthis->thread_info[i].thread_id);
#endif
	}

	//creat manage thread
	err = pthread_create(&pthis->manage_thread_id, NULL, tp_manage_thread, pthis);
	if(0 != err)
	{
#ifdef THREADPOOL_PRINTF
		printf("File:%s:%d\nFunction:%s\n%s\n",__FILE__, __LINE__, __FUNCTION__, "tp_init: creat manage thread failed\n");
#endif
		return FALSE;
	}
#ifdef THREADPOOL_PRINTF
	printf("File:%s:%d\nFunction:%s\n%s\nID:%d\n",__FILE__, __LINE__, __FUNCTION__,"tp_init: creat manage thread %d\n", pthis->manage_thread_id);
#endif
	sleep(1);
	return TRUE;
}

/**
  * member function reality. thread pool entirely close function.
  * para:
  * 	pthis: thread pool struct instance ponter
  * return:
  */
void tp_close(tp_thread_pool *pthis)
{
	int i;

	//close work thread
	for(i=0;i<pthis->cur_th_num;i++)
	{
		kill(pthis->thread_info[i].thread_id, SIGKILL);
		pthread_mutex_destroy(&pthis->thread_info[i].thread_lock);
		pthread_cond_destroy(&pthis->thread_info[i].thread_cond);
#ifdef THREADPOOL_PRINTF
		printf("File:%s:%d\nFunction:%s\n%s\nID:%d\n",__FILE__, __LINE__, __FUNCTION__,"tp_close: kill work thread\n", pthis->thread_info[i].thread_id);
#endif
	}

	//close manage thread
	kill(pthis->manage_thread_id, SIGKILL);
	pthread_mutex_destroy(&pthis->tp_lock);
	
#ifdef THREADPOOL_PRINTF
	printf("File:%s:%d\nFunction:%s\n%s\nID:%d\n",__FILE__, __LINE__, __FUNCTION__, "tp_close: kill manage thread\n", pthis->manage_thread_id);
#endif

	//free thread struct
	free(pthis->thread_info);
}

/**
  * member function reality. main interface opened. 
  * after getting own worker and job, user may use the function to process the task.
  * para:
  * 	pthis: thread pool struct instance ponter
  *	worker: user task reality.
  *	job: user task para
  * return:
  */
void tp_process_job(tp_thread_pool *pthis, tp_work *worker, void  *pThreadParam)
{
	int i;
	int tmpid;

	//fill pthis->thread_info's relative work key
	for(i=0;i<pthis->cur_th_num;i++)
	{
		pthread_mutex_lock(&pthis->thread_info[i].thread_lock);
		if(!pthis->thread_info[i].is_busy)
		{
#ifdef THREADPOOL_PRINTF
			printf("File:%s:%d\nFun:%s\ntp_process_job: %d thread idle, thread id is %d\n",__FILE__, __LINE__, __FUNCTION__, i, pthis->thread_info[i].thread_id);
#endif
			//thread state be set busy before work
			pthis->thread_info[i].is_busy = TRUE;
			pthread_mutex_unlock(&pthis->thread_info[i].thread_lock);

			pthis->thread_info[i].th_work = worker;                       
			pthis->thread_info[i].m_pThreadParam = pThreadParam;

#ifdef THREADPOOL_PRINTF
			printf("File:%s:%d\nFun:%s\ntp_process_job: informing idle working thread %d, thread id is %d\n",__FILE__, __LINE__, __FUNCTION__, i, pthis->thread_info[i].thread_id);
#endif

			pthread_cond_signal(&pthis->thread_info[i].thread_cond);

			return;
		}
		else 
		{
			pthread_mutex_unlock(&pthis->thread_info[i].thread_lock);		
		}
	}//end of for

	//if all current thread are busy, new thread is created here
	pthread_mutex_lock(&pthis->tp_lock);
	if( pthis->add_thread(pthis) )
	{
		i = pthis->cur_th_num - 1;
		tmpid = pthis->thread_info[i].thread_id;
		pthis->thread_info[i].th_work = worker;
		pthis->thread_info[i].m_pThreadParam = pThreadParam;
	}
	else
	{
		 pthread_mutex_unlock(&pthis->tp_lock);
#ifdef THREADPOOL_PRINTF
		  printf("File:%s:%d\nFun:%s\nThread-Pool is Full,Create Thread Failed!!!!!!!!!!!!!!!!\n",__FILE__, __LINE__, __FUNCTION__);
#endif
		 return ;
	}
	pthread_mutex_unlock(&pthis->tp_lock);

	//send cond to work thread
#ifdef THREADPOOL_PRINTF
	printf("File:%s:%d\nFun:%s\ntp_process_job: informing idle working thread %d, thread id is %d\n",__FILE__, __LINE__, __FUNCTION__, i, pthis->thread_info[i].thread_id);
#endif
	pthread_cond_signal(&pthis->thread_info[i].thread_cond);
	return;	
}

/**
  * member function reality. get real thread by thread id num.
  * para:
  * 	pthis: thread pool struct instance ponter
  *	id: thread id num
  * return:
  * 	seq num in thread info struct array
  */
int tp_get_thread_by_id(tp_thread_pool *pthis, pthread_t id)
{
	int i;        
	for(i=0;i<pthis->cur_th_num;i++)
	{                
		if(id == pthis->thread_info[i].thread_id)
		{                       
			return i;
		}
	}

	return -1;
}

/**
  * member function reality. add new thread into the pool.
  * para:
  * 	pthis: thread pool struct instance ponter
  * return:
  * 	true: successful; false: failed
  */
TPBOOL tp_add_thread(tp_thread_pool *pthis)
{
	int err;
	tp_thread_info *new_thread;

	if( pthis->max_th_num <= pthis->cur_th_num )
	{
		return FALSE;
	}

	//malloc new thread info struct
	new_thread = &pthis->thread_info[pthis->cur_th_num];

	//init new thread's cond & mutex
	pthread_cond_init(&new_thread->thread_cond, NULL);
	pthread_mutex_init(&new_thread->thread_lock, NULL);

	//init status is busy
	new_thread->is_busy = TRUE;

	//add current thread number in the pool.
	pthis->cur_th_num++;

	err = pthread_create(&new_thread->thread_id, NULL, tp_work_thread, pthis);
	if(0 != err)
	{
		free(new_thread);
                new_thread = NULL;
		return FALSE;
	}
#ifdef THREADPOOL_PRINTF
	printf("File:%s:%d\nFun:%s\ntp_add_thread: creat work thread %d\n", __FILE__, __LINE__, __FUNCTION__,pthis->thread_info[pthis->cur_th_num-1].thread_id);
#endif

	return TRUE;
}

/**
  * member function reality. delete idle thread in the pool.
  * only delete last idle thread in the pool.
  * para:
  * 	pthis: thread pool struct instance ponter
  * return:
  * 	true: successful; false: failed
  */
TPBOOL tp_delete_thread(tp_thread_pool *pthis)
{
	//current thread num can't < min thread num
	if(pthis->cur_th_num <= pthis->min_th_num) return FALSE;

	//if last thread is busy, do nothing
	if(pthis->thread_info[pthis->cur_th_num-1].is_busy) return FALSE;

	//kill the idle thread and free info struct
	kill(pthis->thread_info[pthis->cur_th_num-1].thread_id, SIGKILL);
	pthread_mutex_destroy(&pthis->thread_info[pthis->cur_th_num-1].thread_lock);
	pthread_cond_destroy(&pthis->thread_info[pthis->cur_th_num-1].thread_cond);

	//after deleting idle thread, current thread num -1
	pthis->cur_th_num--;

	return TRUE;
}

/**
  * member function reality. get current thread pool status:idle, normal, busy, .etc.
  * para:
  * 	pthis: thread pool struct instance ponter
  * return:
  * 	0: idle; 1: normal or busy(don't process)
  */
int  tp_get_tp_status(tp_thread_pool *pthis)
{
	float busy_num = 0.0;
	int i;

	//get busy thread number
	for(i=0;i<pthis->cur_th_num;i++)
	{
		if(pthis->thread_info[i].is_busy)
		{
			busy_num++;
		}
	}

	//0.2? or other num?
	if(busy_num/(pthis->cur_th_num) < BUSY_THRESHOLD)
	{
		return 0;//idle status
	}
	else
	{
		return 1;//busy or normal status	
	}
}

/**
  * internal interface. real work thread.
  * para:
  * 	pthread: thread pool struct ponter
  * return:
  */
void *tp_work_thread(void *pthread)
{
	pthread_t curid;//current thread id
	int nseq = -1;//current thread seq in the pthis->thread_info array
	tp_thread_pool *pthis = (tp_thread_pool*)pthread;//main thread pool struct instance

	//get current thread id
	curid = pthread_self();

	//get current thread's seq in the thread info struct array.
	nseq = pthis->get_thread_by_id(pthis, curid);
	if(nseq < 0)
	{

		return NULL;
	}
#ifdef THREADPOOL_PRINTF
	printf("File:%s:%d\nFun:%s\nentering working thread %d, thread id is %d\n", __FILE__, __LINE__, __FUNCTION__,nseq, curid);
#endif

	//wait cond for processing real job.
	while( TRUE )
	{
		pthread_mutex_lock(&pthis->thread_info[nseq].thread_lock);             
		pthread_cond_wait(&pthis->thread_info[nseq].thread_cond, &pthis->thread_info[nseq].thread_lock);
		pthread_mutex_unlock(&pthis->thread_info[nseq].thread_lock);		

#ifdef THREADPOOL_PRINTF
		printf("File:%s:%d\nFun:%s\n%d thread do work!\n",__FILE__, __LINE__, __FUNCTION__,  pthread_self());
#endif

		tp_work *work = pthis->thread_info[nseq].th_work;
		void  *job = pthis->thread_info[nseq].m_pThreadParam;

		//process                
		work->process_job( (void*)job);

		//thread state be set idle after work
		pthread_mutex_lock(&pthis->thread_info[nseq].thread_lock);		
		pthis->thread_info[nseq].is_busy = FALSE;
		pthread_mutex_unlock(&pthis->thread_info[nseq].thread_lock);

		char szContent[255] = { 0 };
#ifdef THREADPOOL_PRINTF
		printf("File:%s:%d\nFun:%s\n%d do work over\n", __FILE__, __LINE__, __FUNCTION__, szContent,pthread_self());
#endif
	}	
}

/**
  * internal interface. manage thread pool to delete idle thread.
  * para:
  * 	pthread: thread pool struct ponter
  * return:
  */
void *tp_manage_thread(void *pthread)
{
	tp_thread_pool *pthis = (tp_thread_pool*)pthread;//main thread pool struct instance

	//1?
	sleep(MANAGE_INTERVAL);

	do{

		if( pthis->get_tp_status(pthis) == 0 )
		{
			do{

				if( !pthis->delete_thread(pthis) )
				{
					break;
				}
			}while(TRUE);
		}//end for if

		//1?
		sleep(MANAGE_INTERVAL);
	}while(TRUE);
}



#endif
