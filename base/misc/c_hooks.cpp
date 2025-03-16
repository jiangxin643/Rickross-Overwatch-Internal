#include "../main_win.h"
#include "c_hooks.h"

c_hooks hooks_instance;
c_hooks* g_hooks = &hooks_instance;

bool c_hooks::create_hook(uint32_t hook_crc, uintptr_t orig_address, uintptr_t hook_address, uint32_t hook_length) {
	if (_hook_count == MAX_HOOKS - 1) {
		asm("int $3;");
		return false;
	}
	
	hook_data* hook = &(_hooks[_hook_count++ % MAX_HOOKS]);
	hook->crc = hook_crc;
	
	{
		hook->orig_address = orig_address;
		
		memcpy(hook->orig_code, (void*)orig_address, CODE_SIZE);
	}
	
	{
		uint8_t temp[HOOK_SIZE];
		memset(temp, 0, sizeof(temp));
		
#ifdef __x86_64__
		*(uint8_t*)(temp) = 0xff;
		*(uint8_t*)(temp + 0x1) = 0x25;
		*(uint32_t*)(temp + 0x2) = 0x0;
		*(uint64_t*)(temp + 0x6) = (uint64_t)(hook_address);
#else
		*(uint8_t*)(temp) = 0xe9;
		*(uint32_t*)(temp + 0x1) = (uint32_t)(hook_address) - (uint32_t)(orig_address + 0x5);
#endif
		
		memcpy(hook->hook_code, (void*)temp, HOOK_SIZE);
	}

	{
		uint32_t trp_address = (uint32_t)(hook->trp_code);
		hook->trp_address = trp_address;
		
		uint8_t temp[CODE_SIZE];
		memcpy(temp, hook->orig_code, CODE_SIZE);
		
		uint8_t trp_length = hook_length;
		
#ifdef __x86_64__
		if (temp[0] == 0xe9) {
			// not implemented
		}
		
		uint64_t target_offset = (uint64_t)(orig_address + trp_length);
		*(uint8_t*)(temp + trp_length) = 0xff;
		*(uint8_t*)(temp + trp_length + 0x1) = 0x25;
		*(uint32_t*)(temp + trp_length + 0x2) = 0x0;
		*(uint64_t*)(temp + trp_length + 0x6) = target_offset;
#else
		if (temp[0] == 0xe9) {
			uint32_t jmp_offset = *(uint32_t*)(temp + 0x1);
			uint32_t jmp_eip = orig_address + 0x5;
			uint32_t jmp_target = jmp_eip + jmp_offset;
			
			uint32_t new_offset = jmp_target - (trp_address + 0x5);
			*(uint32_t*)(temp + 0x1) = new_offset;
			
			trp_length = 0x5;
		}
		
		uint32_t target_offset = (uint32_t)((orig_address + trp_length) - (trp_address + trp_length + JMP_SIZE));
		*(uint8_t*)(temp + trp_length) = 0xe9;
		*(uint32_t*)(temp + trp_length + 0x1) = target_offset;
#endif
		
		memset(temp + trp_length + JMP_SIZE, 0xCC, CODE_SIZE - trp_length - JMP_SIZE);
		memcpy(hook->trp_code, temp, CODE_SIZE);
	}
	
	return true;
}

void c_hooks::attach() {
	DWORD old_protect;
	
	for (int i = 0; i < _hook_count; ++i) {
		hook_data* hook = &(_hooks[i]);
		
		I(VirtualProtect)(hook->trp_code, CODE_SIZE, PAGE_EXECUTE_READWRITE, &old_protect);
		
		I(VirtualProtect)(hook->orig_address, HOOK_SIZE, PAGE_EXECUTE_READWRITE, &old_protect);
		
		memcpy(hook->orig_address, hook->hook_code, HOOK_SIZE);
		
		//I(VirtualProtect)(hook->orig_address, HOOK_SIZE_32, PAGE_EXECUTE_READ, &old_protect);
	}
}

void c_hooks::detach() {
	for (int i = 0; i < _hook_count; ++i) {
		hook_data* hook = &(_hooks[i]);
		
		memcpy(hook->orig_address, hook->orig_code, HOOK_SIZE);
	}
}

void* c_hooks::find_trp(uint32_t crc) {
	for (int i = 0; i < _hook_count; ++i) {
		hook_data* hook = &(_hooks[i]);
		if (hook->crc == crc)
			return (void*)(hook->trp_address);
	}
	
	asm("int $3;");
	
	return nullptr;
}