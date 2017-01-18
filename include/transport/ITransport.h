#ifndef I_TRAMSPORT_H
#define I_TRAMSPORT_H

typedef int(*sh_send_command)(const char* addr, size_t addr_len, const char* command, size_t command_len, const void* data, size_t data_len);

typedef int(sh_send_configuration)(const char* addr, size_t addr_len, const void* config, size_t config_len);


typedef int(*sh_recv_data)(const char* addr, size_t addr_len, const char* command, size_t command_len, const void* data);

#endif // I_TRAMSPORT_H
