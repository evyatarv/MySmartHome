#ifndef SWITCH_H
#define SWITCH_H

#include <SH_exception.h>
#include <devices\switch\ISH_Switch.h>

/**
 * This class represet one basic switch device that expose on/off
 * functionality
 */
class SH_Switch : ISH_Switch
{
public:
	SH_Switch();
	SH_Switch(const char* address, size_t address_len, const void* transport_data, size_t transport_data_len);
	
	void initSwitch(SH_SWITCH_STATE state);

	// Inherited via ISwitch
	virtual SH_STATUS on(SH_Context* context, SH_RELAY_INDEX relay_num = SH_FIRST_RELAY) override;
	virtual SH_STATUS off(SH_Context* context, SH_RELAY_INDEX relay_num = SH_FIRST_RELAY) override;
	virtual SH_STATUS alternate(SH_Context* context) override;

private:
	const char* _address;
	size_t _address_len;
	const void* _transport_data;
	size_t _transport_data_len;
	SH_SWITCH_STATE _state;
};

#endif
