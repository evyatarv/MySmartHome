#include <devices\switch\SH_Switch.h>

#define CMD_ON ("on")
#define CMD_OFF ("off")

SH_Switch::SH_Switch()
{
	_address = nullptr;
	_address_len = 0;
	_transport_data = nullptr;
	_transport_data_len = 0;
	_state = SH_SWITCH_STATE::OFF;
}

SH_Switch::SH_Switch(const char* address, size_t address_len, const void* transport_data, size_t transport_data_len)
{
	_address = address;
	_address_len = address_len;
	_transport_data = transport_data;
	_transport_data_len = transport_data_len;
}

SH_STATUS SH_Switch::on(SH_Context* context)
{
	return context->send(_address, _address_len, CMD_ON, sizeof(CMD_ON), _transport_data, _transport_data_len, context->timeout);
}

SH_STATUS SH_Switch::off(SH_Context* context)
{
	return context->send(_address, _address_len, CMD_OFF, sizeof(CMD_OFF), _transport_data, _transport_data_len, context->timeout);
}

void SH_Switch::initSwitch(SH_SWITCH_STATE state)
{
	_state = state;
}

SH_STATUS SH_Switch::alternate(SH_Context * context)
{
	if (_state == SH_SWITCH_STATE::OFF)
		return on(context);
	return off(context);
}
