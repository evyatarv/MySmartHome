#ifndef I_SWITCH_H
#define I_SWITCH_H

#include <SH_Context.h>
#include <SmartHome_type_defs.h>
#include <SmartHome_err_defs.h>

/**
 * Inteface representing switch
 */
class ISH_Switch
{
public:
	enum SH_SWITCH_STATE { ON, OFF };

	virtual void initSwitch(SH_SWITCH_STATE state) = 0;
	virtual SH_STATUS on(SH_Context* context) = 0;
	virtual SH_STATUS off(SH_Context* context) = 0;
};

#endif