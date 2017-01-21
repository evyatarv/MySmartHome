#ifndef ALTERNATE_SWITCH_H
#define ALTERNATE_SWITCH_H

#include <devices\switch\SH_Switch.h>

/**
 * This class represent alternate switch, any press on aternate will
 * switch the state.
 * 
 * NOTE: This class cant reflect the real switch state. 
 */
class SH_AlternateSwitch : ISH_Switch
{
public:

	SH_AlternateSwitch(const char* a_address, size_t a_address_len, const char* b_address, size_t b_address_len,
		const void* trasport_data);

	SH_AlternateSwitch(SH_Switch* a, SH_Switch* b);

	// Inherited via ISH_Switch
	virtual SH_STATUS on(SH_Context* context) override;
	virtual SH_STATUS off(SH_Context* context) override;
	virtual SH_STATUS alternate(SH_Context* context) override;

	void initAltSwitch(SH_Switch* on, SH_Switch* off);

private:
	SH_Switch* _a;
	SH_Switch* _b;
	SH_Switch* _current_on;
	SH_Switch* _current_off;

	void initAltSwitch();
};

#endif