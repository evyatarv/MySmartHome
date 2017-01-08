#ifndef HTTP_TRAMSPORT_H
#define HTTP_TRAMSPORT_H

#include <stdint.h>
#include <SmartHome_err_defs.h>
#include <SmartHome_type_defs.h>

#define SH_MAX_USER_LENGTH		(20)
#define SH_MAX_PASSWORD_LENGTH	(20)
#define SH_MAX_URL_LENGTH		(200)

/* Sends get http request with basic HTTP1.1 auth
 * NOTE: autentication is not optional 
 *  
 * addr			- destination address string null terminated
 * addr_len		- string size without counting '/0'
 * command		- command to send string null terminated
 * command_len	- string size without counting '/0'
 * auth			- http basic auth struct
 *				  NOTE: all string sizes are without counting '/0'
 */
SH_C_EXTERN SH_STATUS http_send
(
	const char* addr, 
	size_t addr_len, 
	const char* command, 
	size_t command_len, 
	const void* auth
);

#endif // HTTP_TRAMSPORT_H
