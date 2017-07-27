#ifndef SH_SCHEDULER_TIME_H
#define SH_SCHEDULER_TIME_H

#include <SmartHome_common_defs.h>
#include <SmartHome_err_defs.h>
#include <SH_exception.h>
#include <string>
#include <time.h>

class SH_Scheduler_time
{
public:

	SH_Scheduler_time(const std::string timing);

	SH_Scheduler_time(time_t timing);

private: 
	time_t _timing;
};

#endif