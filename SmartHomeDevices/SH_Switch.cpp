#include <devices\switch\SH_Switch.h>

#define CMD_ON ("on")
#define CMD_OFF ("off")

SH_Switch::SH_Switch()
{
	_address = nullptr;
	_address_len = 0;
	_transport_data = nullptr;
}

SH_Switch::SH_Switch(const char* address, size_t address_len, const void* transport_data)
{
	_address = address;
	_address_len = address_len;
	_transport_data = transport_data;
}

SH_STATUS SH_Switch::on(SH_Context* context)
{
	return context->send(_address, _address_len, CMD_ON, sizeof(CMD_ON), _transport_data);
}

SH_STATUS SH_Switch::off(SH_Context* context)
{
	return context->send(_address, _address_len, CMD_OFF, sizeof(CMD_OFF), _transport_data);
}
