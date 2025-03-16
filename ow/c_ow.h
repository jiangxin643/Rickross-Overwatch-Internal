#pragma once

struct cmp_singleton {
	uint64_t vtable;
	uint64_t component;
	uint64_t parent;
};

#define OW_TICK_INTERVAL 0.016f

#define MAX_ENTITY_PARENTS 512
#define MAX_PLAYER_PARENTS 512

class c_ow {
public:
	NTSTATUS NtOpenThread(PHANDLE ThreadHandle, ACCESS_MASK AccessMask, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
	NTSTATUS NtClose(HANDLE Handle);
	NTSTATUS NtQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
	NTSTATUS NtQueryVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
	NTSTATUS NtReadVirtualMemory(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);
	
	bool init();
	void unload();
	
	void* alloc(int size);
	void free(void* ptr);
	bool read(void* dest, uint64_t source, uint32_t size);
	
	void think();
	
	void frame_think();
	
	bool update_state();
	void add_objects();
	
	void check_cache();
	
	bool is_valid_component(uint64_t component, uint8_t id);
	bool is_valid_parent(uint64_t parent);
	bool is_valid_page(uint64_t address);
	
	bool get_main_thread(CLIENT_ID* client_id);
	
	uint64_t get_decrypted_parent(uint64_t parent);
	uint64_t lookup_entry(uint64_t parent, uint64_t id);
	uint64_t decrypt_entry(uint64_t* entry);
	uint64_t lookup_component(uint64_t parent, uint64_t id);
	
	bool _init;
	bool _failed;
	
	HANDLE _thread_id;
	
	uint64_t _base_address;
	uint64_t _stack_base;
	
	uint64_t _user_cmd_func_address;
	
	uint64_t _key_func_address;
	uint64_t _ray_func_address;
	
	uint64_t _fnsilentaim_func_address;
	
	uint64_t _test_break;
	uint64_t _test_break2;
	
	uint64_t _test_skill_struct;
	
	uint64_t _test_key1;
	uint64_t _test_key2;
	
	game_manager_t* _gm;
	bool _thread_lock;
	
	bool _unload_user_cmd;
	
	cmp_singleton _singleton_input;
	
	bool _max_hit;
	bool _state_updated;
	bool _reset;
	
	int _game_tickcount;
	
	LARGE_INTEGER _process_time;
	uint32_t _sample_time;
	
	input_cmd* _input_cmd;
	server_cmd* _server_cmd;
	pseudo_cmd _cmd;
	uint32_t _last_buttons;
	
	uint32_t _move_seed;
	
	int _test_error;
	
	int _test_index;
	int _test_max;
	int _test_result;
	
	uint64_t _test_address;
	
	bool _pool_id_valid[65535];
	
	int _black;
};

extern c_ow* g_ow;