#ifndef SH_CONTEXT_H
#define SH_CONTEXT_H

#include<transport\ITransport.h>

class sh_context
{
public:
	sh_send send;
	sh_recv recv;
};

#endif