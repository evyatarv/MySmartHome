
#include <internal\SH_CsWrapper.h>
#include <transport\HttpTransport.h>

SH_Switch* CreateSwitch
(
	uint8_t num_of_relayes,
	const char* address,
	size_t address_len
)
{
	char* pAddress = (char*)malloc(address_len);
	memcpy(pAddress, address, address_len);

	SH_Switch* pSwitch = new SH_Switch(num_of_relayes, pAddress, address_len);

	return pSwitch;
}

SH_Context* CreateContext (uint32_t transport_type, long timeout, char* auth_name, size_t auth_name_len, char* auth_pass, size_t auth_pass_len)
{
	char* pAuthName = (char*)malloc(auth_name_len);
	char* pAuthPass = (char*)malloc(auth_pass_len);

	// copy trasport data
	memcpy(pAuthPass, auth_pass, auth_pass_len);
	memcpy(pAuthName, auth_name, auth_name_len); 

	httpBasicAuth* switch_auth = new httpBasicAuth{ pAuthName, auth_name_len, pAuthPass, auth_pass_len };

	SH_Context* pContext = new SH_Context();

	// init transport function pointers.
	switch (transport_type)
	{
		case SH_TRANSPORT_TYPE::HTTP: 
		{
			pContext->send = http_send_command;
		}
		break;

		default:
			throw new sh_invalid_params_exception();
	}

	// set transport timeout
	pContext->timeout = timeout;

	// set transport data
	pContext->transport_data = switch_auth;
	pContext->transport_data_len = sizeof(httpBasicAuth);

	return pContext;
}

void FreeContext(SH_Context* context)
{
	if (context == nullptr)
		return;

	if (context->transport_data != nullptr)
	{
		switch (context->transport_type)
		{
		case SH_TRANSPORT_TYPE::HTTP:
		{
			free(((httpBasicAuth*)(context->transport_data))->user);
			free(((httpBasicAuth*)(context->transport_data))->password);
		}
		break;

		default:
			throw new sh_not_support_exception();
		}

		free((void*)context->transport_data);
	}

	free(context);

	context = nullptr;
}

int On
(
	SH_Context* context,
	SH_Switch* sh_switch,
	uint32_t relay_index
)
{
	return (sh_switch->on(context, (SH_RELAY_INDEX)relay_index));
}

int Off
(
	SH_Context* context,
	SH_Switch* sh_switch,
	uint32_t relay_index
)
{
	return (sh_switch->off(context, (SH_RELAY_INDEX)relay_index));
}

int Alternate
(
	SH_Context* context,
	SH_Switch* sh_switch
)
{
	return (sh_switch->alternate(context));
}