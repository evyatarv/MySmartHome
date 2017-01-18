#ifndef SH_CONTEXT_H
#define SH_CONTEXT_H

#include<transport\ITransport.h>

class SH_Context
{
public:
	sh_send_command send;
	sh_recv_data recv;
};

#endif