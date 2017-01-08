#ifndef I_SWITCH_H
#define I_SWITCH_H

#include <SH_Context.h>
#include <SmartHome_type_defs.h>
#include <SmartHome_err_defs.h>

/**
 * Inteface representing one switch
 */
class ISH_Switch
{
public:

	virtual SH_STATUS on(SH_Context* context) = 0;
	virtual SH_STATUS off(SH_Context* context) = 0;
};

#endif