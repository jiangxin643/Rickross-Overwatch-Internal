#ifdef __x86_64__

#define MAX_HOOKS 16

/*
	we need the code size to match 4 64 bit jmps (14 bytes in size),
	to account for 3 32 bit jmps (5 bytes in size) and an additional jmp back after those 3 jmps
	which is 56, but we'll just use 64
*/
#define HOOK_SIZE 14
#define CODE_SIZE 48
#define JMP_SIZE 14

#else

#define MAX_HOOKS 32

#define HOOK_SIZE 5
#define CODE_SIZE 16
#define JMP_SIZE 5

#endif

struct hook_data {
	uint32_t crc;
	
	uintptr_t orig_address;
	uintptr_t trp_address;
	
	uint8_t orig_code[CODE_SIZE];
	uint8_t hook_code[HOOK_SIZE];
	uint8_t trp_code[CODE_SIZE];
};

class c_hooks {
public:
	bool create_hook(uint32_t crc, uintptr_t orig_address, uintptr_t hook_address, uint32_t hook_length);
	void attach();
	void detach();
	void* find_trp(uint32_t crc);
	
	hook_data _hooks[MAX_HOOKS];
	int _hook_count;
	
	bool _unloaded;
};

extern c_hooks* g_hooks;