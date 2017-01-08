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

#endif // SMART_HOME_TYPE_DEFS_H
