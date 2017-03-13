#include <stdio.h>
#include <string.h>
#include <curl\curl.h>
#include <transport\HttpTransport.h>
#include <SmartHome_common_defs.h>
#include <SmartHome_err_defs.h>

#define SH_MAX_RELM_SIZE	(SH_MAX_USER_LENGTH + SH_MAX_PASSWORD_LENGTH + 1)

int build_url(const char* addr, size_t addr_len, const char* command, size_t command_len,
								char* url, size_t url_max_len)
{
	size_t index = 0;

	// check that we have enough space in the our realm buffer 
	if (addr_len + command_len > SH_MAX_URL_LENGTH || 
		url_max_len > SH_MAX_URL_LENGTH)
	{
		return -SH_EINVALID_PARAM;
	}

	// copy user into realm buffer 
	if (strncpy_s(url, url_max_len - index, addr, addr_len))
		return -SH_EGENERIC;
	index += addr_len;

	// addind '/'
	if (strncpy_s(url + index, url_max_len - index, "/", 1))
		return -SH_EGENERIC;
	index += 1;

	// copy password inro realm buffer
	if (strncpy_s(url + index, url_max_len - index, command, command_len))
		return -SH_EGENERIC;
	index += command_len;

	return index + 1; // include '/0'

	return 0;
}
int build_auth_relm(const char* user, size_t user_len, const char* password, 
								size_t password_len, char* realm, size_t realm_max_len)
{
	size_t index = 0;

	// check that we have enough space in the our realm buffer 
	if (user_len + password_len > realm_max_len)
	{
		return -SH_EINVALID_PARAM;
	}
	
	// copy user into realm buffer 
	if (strncpy_s(realm, realm_max_len - index, user, user_len))
		return -SH_EGENERIC; 
	index += user_len;

	// addind ':'
	if(strncpy_s(realm + index, realm_max_len - index, ":", 1))
		return -SH_EGENERIC;
	index += 1;
	
	// copy password into realm buffer
	if (strncpy_s(realm + index, realm_max_len - index, password, password_len))
		return -SH_EGENERIC;
	index += password_len;

	return index + 1; // include '/0'
}

bool validate_params(const char* addr, size_t addr_len, const char* op, size_t op_len, 
						const void* command_data, size_t command_data_len, const void* auth, size_t auth_len)
{
	SH_STATUS status = SH_SUCCESS;
	httpBasicAuth* basic_auth = (httpBasicAuth*)auth;

	do
	{
		// check mendatory params
		if (!addr || !op || !auth)
		{
			status = -SH_EINVALID_PARAM;
			break;
		}

		// chek auth type
		if (auth_len != sizeof(httpBasicAuth))
		{
			status = -SH_EINVALID_PARAM;
			break;
		}

		// check sizes 
		if (basic_auth->user_len > SH_MAX_USER_LENGTH || basic_auth->password_len > SH_MAX_PASSWORD_LENGTH ||
			basic_auth->user_len == 0 || basic_auth->password_len == 0 ||
			(command_data == nullptr && command_data_len != 0) || (command_data_len == 0 && command_data != nullptr) ||
			addr_len == 0 || op_len == 0)
		{
			status = -SH_EINVALID_PARAM;
			break;
		}
	} while (false);

	return (status == SH_SUCCESS);
}

SH_STATUS http_preform
(
	const char* url, 
	const void* data, 
	const void* auth, 
	sh_timeout timeout
)
{
	CURL *curl;
	CURLcode res;
	SH_STATUS status = SH_SUCCESS;

	/* In windows, this will init the winsock stuff */
	curl_global_init(CURL_GLOBAL_ALL);

	// get curl handle
	curl = curl_easy_init();

	if (curl)
	{
		/* set URL */
		curl_easy_setopt(curl, CURLOPT_URL, url);

		/* set POST request */
		//curl_easy_setopt(curl, CURLOPT_POST, true);

		/* specify the POST data */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

		/* set request with basic auth */
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);

		/* complete connection within timeout seconds (default is 0 = ~300sec) */
		if (timeout > SH_NO_TIMEOUT)
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

		/*  set auth userpwd */
		curl_easy_setopt(curl, CURLOPT_USERPWD, auth);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

			status = SH_COMM_FAIL;
		}
	}

	/* always cleanup */
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return status;
}

SH_STATUS http_send_command(const char* addr, size_t addr_len, const char* command, size_t command_len, 
									const void* command_data, size_t command_data_len, const void* auth, size_t auth_len, sh_timeout timeout)
{
	SH_STATUS status = SH_SUCCESS;
	char auth_realm[SH_MAX_RELM_SIZE] = { 0 };
	char url[SH_MAX_URL_LENGTH] = { 0 };
	httpBasicAuth* basic_auth = (httpBasicAuth*)auth;

	do
	{
		// check params
		if (!validate_params(addr, addr_len, command, command_len, command_data, command_data_len, auth, auth_len))
		{
			status = -SH_EINVALID_PARAM;
			break;
		}

		// build auth relm
		if (build_auth_relm(
			basic_auth->user, basic_auth->user_len,
			basic_auth->password, basic_auth->password_len,
			auth_realm, SH_MAX_RELM_SIZE) <= 0)
		{
			status = -SH_EGENERIC;
			break;
		}

		// build url
		if (build_url(
			addr, addr_len,
			command, command_len,
			url, SH_MAX_URL_LENGTH) <= 0)
		{
			status = -SH_EGENERIC;
			break;
		}

		status = http_preform(url, command_data, auth_realm, timeout);

	} while (false);

	return status;
}