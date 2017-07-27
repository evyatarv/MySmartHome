#include <SH_Threads.h>
#include <mutex>
#include <condition_variable>

SH_Thread::SH_Thread(sh_scheduled_task task)
{
	_worker = nullptr;
}

void SH_Thread::start(void* task_args)
{
	_worker = new std::thread(_task, task_args);
}
