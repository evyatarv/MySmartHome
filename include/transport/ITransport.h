#ifndef I_TRAMSPORT_H
#define I_TRAMSPORT_H

#define SH_NO_TIMEOUT	(-1)

typedef enum _SH_TRANSPORT_TYPE 
		{ 
			HTTP, 
			MAX_TRANSPORT_TYPE 
		} SH_TRANSPORT_TYPE;

typedef long sh_timeout;

typedef int(*sh_send_command)(const char* addr, size_t addr_len, const char* command, size_t command_len, const void* command_data, size_t command_data_len, const void* auth, size_t auth_len, sh_timeout timeout);

typedef int(*sh_recv_data)(const char* addr, size_t addr_len, const char* command, size_t command_len, const void* command_data, size_t command_data_len, const void* auth, size_t auth_len, sh_timeout timeout);

#endif // I_TRAMSPORT_H
