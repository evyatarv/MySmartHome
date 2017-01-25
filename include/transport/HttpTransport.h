#ifndef HTTP_TRAMSPORT_H
#define HTTP_TRAMSPORT_H

#include <stdint.h>
#include <transport\ITransport.h>
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
SH_C_EXTERN SH_STATUS http_send_command
(
	const char* addr, 
	size_t addr_len, 
	const char* command, 
	size_t command_len, 
	const void* auth,
	size_t auth_len,
	sh_timeout timeout
);

SH_C_EXTERN SH_STATUS http_send_configuration
(
	const char* addr,
	size_t addr_len,
	const void* config,
	size_t config_len,
	const void* auth,
	size_t auth_len,
	sh_timeout timeout
);

#endif // HTTP_TRAMSPORT_H
