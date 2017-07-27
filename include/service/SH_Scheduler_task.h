#ifndef SH_SCHEDULER_H
#define SH_SCHEDULER_H

#include <SmartHome_common_defs.h>
#include <SmartHome_err_defs.h>
#include <string>

class SH_Scheduler_task
{
public:

	SH_STATUS add_schedule(std::string date, std::string date_format, std::string time, std::string time_format);

	void setCyclic(bool is_cyclic);
};

#endif