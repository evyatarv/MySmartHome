#include "SH_Scheduler_time.h"
#include <sstream>
#include <ctime>
#include <iomanip>

static const std::string TIMING_FORMAT= "%d-%m-%YT%H:%M:%SZ";

SH_Scheduler_time::SH_Scheduler_time(const std::string timing)
{

	static const std::string dateTimeFormat{ TIMING_FORMAT };
																	 
	std::istringstream ss{ timing };
	
	std::tm dt;
	
	ss >> std::get_time(&dt, dateTimeFormat.c_str());
	
	_timing = std::mktime(&dt);
	
	if (_timing < 0)
		throw new sh_wrong_time_format_exception();
	
}

SH_Scheduler_time::SH_Scheduler_time(time_t timing)
{
	_timing = timing;
}
