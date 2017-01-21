#ifndef ALTERNATE_SWITCH_H
#define ALTERNATE_SWITCH_H

#include <devices\switch\SH_Switch.h>

class SH_AlternateSwitch
{
public:

	SH_AlternateSwitch(const char* a_address, size_t a_address_len, const char* b_address, size_t b_address_len,
		const void* trasport_data);

	SH_AlternateSwitch(SH_Switch* a, SH_Switch* b);

	SH_STATUS alternate(SH_Context* context);

	void initAltSwitch(SH_Switch* on, SH_Switch* off);

private:
	SH_Switch* _a;
	SH_Switch* _b;
	SH_Switch* _current_on;
	SH_Switch* _current_off;

	void initAltSwitch();


};

#endif