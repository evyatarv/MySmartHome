#include <devices\switch\SH_Switch.h>
#include <Windows.h>

const std::string CMD_ON("on");
const std::string CMD_OFF("off");
const std::string CMD_RELAY("relay");

SH_Switch::SH_Switch()
{
	_address = nullptr;
	_address_len = 0;
	_state = 0;
	_num_of_relays = SH_ZERO_RELAYS;
}

SH_Switch::SH_Switch(uint8_t num_of_relays, const char* address, size_t address_len): SH_Switch()
{
	_address = address;
	_address_len = address_len;

	if (num_of_relays >= SH_MAX_RELAY)
		throw new sh_not_support();

	_num_of_relays = num_of_relays;
}

SH_STATUS SH_Switch::on(SH_Context* context, SH_RELAY_INDEX relay_index)
{	
	if (relay_index > _num_of_relays || relay_index <= SH_ZERO_RELAYS)
		return SH_EINVALID_PARAM;

	SH_STATUS status = SH_EGENERIC;
	std::string cmd = build_switch_command(relay_index, CMD_ON); 

	status = context->send(_address, _address_len, cmd.c_str(), cmd.length(), nullptr, 0, context->transport_data, context->transport_data_len, context->timeout);
	if (status == SH_SUCCESS)
		SH_SET_BIT(_state, relay_index);

	return status;
}

SH_STATUS SH_Switch::off(SH_Context* context, SH_RELAY_INDEX relay_index)
{
	if (relay_index > _num_of_relays || relay_index <= SH_ZERO_RELAYS)
		return SH_EINVALID_PARAM;

	SH_STATUS status = SH_EGENERIC;
	std::string cmd = build_switch_command(relay_index, CMD_OFF);

	status = context->send(_address, _address_len, cmd.c_str(), cmd.length(), nullptr, 0, context->transport_data, context->transport_data_len, context->timeout);
	if (status == SH_SUCCESS)
		SH_CLEAR_BIT(_state, relay_index);

	return status;

}

SH_STATUS SH_Switch::alternate(SH_Context * context, SH_RELAY_INDEX relay_a, SH_RELAY_INDEX relay_b)
{
	SH_STATUS status = SH_EGENERIC;
	SH_RELAY_INDEX relay_off = SH_ZERO_RELAYS;
	SH_RELAY_INDEX relay_on = SH_ZERO_RELAYS;

	if ((relay_a >= _num_of_relays || relay_a <= SH_ZERO_RELAYS) &&
		(relay_b >= _num_of_relays || relay_b <= SH_ZERO_RELAYS))
		return SH_EINVALID_PARAM;

	if (SH_GET_BIT(_state, relay_a) && SH_GET_BIT(_state, relay_b))
	{
		//TODO: unclear state - print log err
		return SH_EGENERIC;
	}

	// set relays flow
	if (SH_GET_BIT(_state, relay_a))
	{
		relay_off = relay_a;
		relay_on = relay_b;
	}
	else
	{
		relay_off = relay_b;
		relay_on = relay_a;
	}

	do
	{
		status = off(context, relay_off);
		if (status != SH_SUCCESS)
			break; //TODO: print err
		
		Sleep(SH_SWIRCH_ALT_SLEEP);

		status = on(context, relay_on);

	} while (false);

	return status;
}

std::string SH_Switch::build_switch_command(int relay_index, std::string cmd)
{
	return (CMD_RELAY + "/" + std::to_string(relay_index) + "/" + cmd);
}
