#ifndef I_TRAMSPORT_H
#define I_TRAMSPORT_H

typedef int(*sh_send)(const char* addr, size_t addr_len, const char* command, size_t command_len, const void* data);

typedef int(*sh_recv)(const char* addr, size_t addr_len, const char* command, size_t command_len, const void* data);
#endif // I_TRAMSPORT_H
