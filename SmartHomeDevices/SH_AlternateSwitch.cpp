#include <devices\switch\SH_AlternateSwitch.h>
#include <SH_exception.h>


SH_AlternateSwitch::SH_AlternateSwitch(const char * a_address, size_t a_address_len, const char * b_address, size_t b_address_len,
	const void * trasport_data)
{
	throw sh_not_implemented_exception();
}

SH_AlternateSwitch::SH_AlternateSwitch(SH_Switch * a, SH_Switch* b)
{
	if (!a || !b)
		throw sh_invalid_params();

	_a = a; 
	_b = b;

	initilize_alt_switch();
}

void SH_AlternateSwitch::initilize_alt_switch(SH_Switch * on, SH_Switch * off)
{
	if (!on ||  !off || on == off || 
		(on != _a && on != _b) || (off != _a && off != _b))
		throw sh_invalid_params();

	_current_on = on; 
	_current_off = off;
}

void SH_AlternateSwitch::initilize_alt_switch()
{
	_current_on = _a;
	_current_off = _b;
}

SH_STATUS SH_AlternateSwitch::Switch(sh_context* context)
{
	SH_Switch* tmp;
	SH_STATUS sh_status = SH_EGENERIC;

	do
	{
		// alternate switches
		sh_status = _current_on->off(context);
		if (sh_status != SH_SUCCESS)
			break;

		sh_status = _current_on->on(context);
		if (sh_status != SH_SUCCESS)
			break;

		// swap pointers
		tmp = _current_on;
		_current_on = _current_off;
		_current_off = tmp;
		tmp = nullptr;
	} 
	while (false);

	if (sh_status != SH_SUCCESS)
		initilize_alt_switch();

	return sh_status;
}
