#ifndef SWITCH_H
#define SWITCH_H

#include <devices\switch\ISH_Switch.h>

/*
 * This class represet one basic switch device that expose on/off
 * functionality
 */
class SH_Switch : ISH_Switch
{
public:
	SH_Switch();
	SH_Switch(const char* address, size_t address_len, const void* transport_data);
	
	// Inherited via ISwitch
	virtual SH_STATUS on(SH_Context* context) override;
	virtual SH_STATUS off(SH_Context* context) override;

private:
	const char* _address;
	size_t _address_len;
	const void* _transport_data;
};

#endif
