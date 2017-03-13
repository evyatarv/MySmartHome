#ifndef SMART_HOME_COMMON_DEFS_H
#define SMART_HOME_COMMON_DEFS_H

#include <stdint.h>

// ***************** COMMON DEFS *****************
#define	SH_C_EXTERN extern "C" __declspec(dllexport)
typedef int SH_STATUS;

// ***************** COMMON DEFS - ENDS *****************



// ***************** COMMON UTILS *****************
#define SH_SET_BIT(x, index)			(x |= 1 << index)
#define SH_CLEAR_BIT(x, index)			(x &= ~(1 << index))
#define SH_GET_BIT(x, index)			((x >> index) & 1)
#define SH_CHANGE_BIT(x, index, val)	(x ^= (-val ^ x) & (1 << index))

// ***************** COMMON UTILS - ENDS *****************



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



// ***************** SYSTEM DEFS *****************
#define SH_SWIRCH_ALT_SLEEP	(250)

typedef enum _sh_relay_index
{
	SH_ZERO_RELAYS = 0,
	SH_FIRST_RELAY = 1,
	SH_SECOND_RELAY,
	SH_THIRD_RELAY,
	SH_FORTH_RELAY,
	SH_FIFTH_RELAY,
	SH_MAX_RELAY
}SH_RELAY_INDEX;

// ***************** SYSTEM DEFS - ENDS *****************


#endif // SMART_HOME_COMMONE_DEFS_H
