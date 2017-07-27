#ifndef SH_SCHEDULER_TASK_H
#define SH_SCHEDULER_TASK_H

#include <SmartHome_common_defs.h>
#include <SmartHome_err_defs.h>
#include <service\SH_Scheduler_time.h>
#include <list>

class SH_Scheduler_task
{
public:

	SH_STATUS add_schedule(SH_Scheduler_time* time);

	void set_cyclic(bool is_cyclic);

private: 
	std::list<SH_Scheduler_time> _timing; 
};

#endif