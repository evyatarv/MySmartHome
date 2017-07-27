#ifndef SH_THREADS_H
#define SH_THREADS_H

#include <thread>
#include <time.h>
#include <SmartHome_err_defs.h>
#include <SmartHome_common_defs.h>

typedef int(*sh_scheduled_task)(void*);

class SH_Thread
{
public: 

	SH_Thread(sh_scheduled_task task);

	void start(void* task_args);

private:

	std::thread* _worker;
	sh_scheduled_task _task;
};

#endif

