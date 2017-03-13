#ifndef SWITCH_H
#define SWITCH_H

#include <string>
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
	SH_Switch(uint8_t num_of_relayes, const char* address, size_t address_len, const void* transport_data, size_t transport_data_len);

	// Inherited via ISwitch
	virtual SH_STATUS on(SH_Context* context, SH_RELAY_INDEX relay_index = SH_FIRST_RELAY) override;
	virtual SH_STATUS off(SH_Context* context, SH_RELAY_INDEX relay_index = SH_FIRST_RELAY) override;
	virtual SH_STATUS alternate(SH_Context * context, SH_RELAY_INDEX relay_a = SH_FIRST_RELAY, SH_RELAY_INDEX relay_b = SH_SECOND_RELAY) override;

private:
	const char* _address;
	size_t _address_len;
	const void* _transport_data;
	size_t _transport_data_len;
	uint32_t _state;
	uint8_t _num_of_relays;

	std::string build_switch_command(int relay_index, std::string cmd);
};

#endif
