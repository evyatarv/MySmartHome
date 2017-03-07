#ifndef SMART_HOME_TYPE_DEFS_H
#define SMART_HOME_TYPE_DEFS_H

#include <stdint.h>

#define	SH_C_EXTERN extern "C" __declspec(dllexport)

typedef int SH_STATUS;

// ***************** HTTP TYPES *****************

/* representing http basic auth
*
* user			- user string inclued null terminated
* user_len		- string size without counting '/0'
* password		- password string inclued null terminated
* password_len	- string size without counting '/0'
*
*/
typedef struct _httpBasicAuth
{
	char* user;
	size_t user_len;
	char* password;
	size_t password_len;
}httpBasicAuth;

// ***************** HTTP TYPES - ENDS *****************

typedef enum _sh_relay_index
{
	SH_FIRST_RELAY = 1,
	SH_SECOND_RELAY,
	SH_THIRD_RELAY,
	SH_FORTH_RELAY,
	SH_FIFTH_RELAY,
	SH_MAX_RELAY
}SH_RELAY_INDEX;
#endif // SMART_HOME_TYPE_DEFS_H
