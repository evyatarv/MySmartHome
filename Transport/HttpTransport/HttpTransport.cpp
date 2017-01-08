#include <stdio.h>
#include <string.h>
#include <curl\curl.h>
#include <transport\HttpTransport.h>
#include <SmartHome_type_defs.h>
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
	
	// copy password inro realm buffer
	if (strncpy_s(realm + index, realm_max_len - index, password, password_len))
		return -SH_EGENERIC;
	index += password_len;

	return index + 1; // include '/0'
}

SH_STATUS http_send(const char* addr, size_t addr_len, const char* command, size_t command_len, const void* auth)
{
	CURL *curl;
	CURLcode res;
	SH_STATUS status = SH_SUCCESS;
	char auth_realm[SH_MAX_RELM_SIZE] = { 0 };
	char url[SH_MAX_URL_LENGTH] = { 0 };
	httpBasicAuth* basic_auth = (httpBasicAuth*)auth; 

	// get curl handle
	curl = curl_easy_init();

	if (curl)
	{
		do
		{
			// check pointer
			if (!addr || !command || !auth)
			{
				status = -SH_EINVALID_PARAM;
				break;
			}

			// check sizes 
			if (basic_auth->user_len > SH_MAX_USER_LENGTH || basic_auth->password_len > SH_MAX_PASSWORD_LENGTH ||
				basic_auth->user_len == 0 || basic_auth->password_len == 0 || 
				addr_len == 0 || command_len == 0)
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

			// set URL
			curl_easy_setopt(curl, CURLOPT_URL, url);

			// tell libcurl to follow redirection
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

			// set request with basic auth
			curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);

			//  set auth userpwd
			curl_easy_setopt(curl, CURLOPT_USERPWD, auth_realm);

			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);

			/* Check for errors */
			if (res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() res = %d failed: %s\n",
					res,
					curl_easy_strerror(res));
		} while (false);

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	else
	{
		fprintf(stderr, "curl_easy_init() failed\n");
		status = -SH_EGENERIC;
	}

	return status;
}