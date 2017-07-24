#ifndef SH_CS_WRAPPER_H
#define SH_CS_WRAPPER_H


#include <devices\switch\SH_Switch.h>

#ifdef __cplusplus
extern "C" {
#endif

	SH_C_EXTERN SH_Switch* CreateSwitch (uint8_t num_of_relayes, const char* address, size_t address_len);

	SH_C_EXTERN void FreeContext(SH_Context* context);

	SH_C_EXTERN SH_Context* CreateContext (uint32_t transport_type, long timeout, char* auth_name, size_t auth_name_len, char* auth_pass, size_t auth_pass_len);
	
	SH_C_EXTERN void FreeContext(SH_Context* context);

	SH_C_EXTERN int On (SH_Context* context, SH_Switch* sh_switch,	uint32_t relat_index);

	SH_C_EXTERN int Off (SH_Context* context, SH_Switch* sh_switch, uint32_t relat_index);

	SH_C_EXTERN int Alternate (SH_Context* context, SH_Switch* sh_switch);

#ifdef __cplusplus
}
#endif

#endif // SH_CS_WRAPPER_H