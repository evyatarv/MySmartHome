#include <devices\switch\SH_Switch.h>

const std::string CMD_ON("on");
const std::string CMD_OFF("off");
const std::string CMD_RELAY("relay");

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

SH_STATUS SH_Switch::on(SH_Context* context, SH_RELAY_INDEX relay_index)
{	
	if (relay_index >= SH_MAX_RELAY || relay_index <= 0)
		throw new sh_not_support();

	SH_STATUS status = SH_EGENERIC;
	std::string cmd = build_switch_command(relay_index, CMD_ON); 

	status = context->send(_address, _address_len, cmd.c_str(), cmd.length(), nullptr, 0, _transport_data, _transport_data_len, context->timeout);
	if (status == SH_SUCCESS)
		_state = SH_SWITCH_STATE::ON;

	return status;
}

SH_STATUS SH_Switch::off(SH_Context* context, SH_RELAY_INDEX relay_index)
{
	if (relay_index >= SH_MAX_RELAY || relay_index <= 0)
		throw new sh_not_support();

	SH_STATUS status = SH_EGENERIC;
	std::string cmd = build_switch_command(relay_index, CMD_OFF);

	status = context->send(_address, _address_len, cmd.c_str(), cmd.length(), nullptr, 0, _transport_data, _transport_data_len, context->timeout);
	if (status == SH_SUCCESS)
		_state = SH_SWITCH_STATE::OFF;

	return status;

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

std::string SH_Switch::build_switch_command(int relay_index, std::string cmd)
{
	return (CMD_RELAY + "/" + std::to_string(relay_index) + "/" + cmd);
}
