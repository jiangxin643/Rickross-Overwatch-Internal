#include "../main_win.h"

static import_data s_imports[MAX_IMPORTS];
static int s_import_count = 0;

static uint32_t s_module_hashes[] = {HASH("ntdll.dll"), HASH("kernel32.dll"), HASH("user32.dll")};

// NtCurrentTeb
_TEB* get_teb() {
#ifdef __x86_64__
	_TEB* teb = (_TEB*)(__readgsqword(0x30));
#else
	_TEB* teb = (_TEB*)(__readfsdword(0x18));
#endif
	return teb;
}

bool get_module(uint32_t module_crc, uintptr_t* base_address) {
	TEB* teb = get_teb();
	PEB* peb = teb->ProcessEnvironmentBlock;
	PEB_LDR_DATA* ldr = peb->Ldr;
	
	LDR_DATA_TABLE_ENTRY* entry = (LDR_DATA_TABLE_ENTRY*)ldr->InMemoryOrderModuleList.Flink;
	LDR_DATA_TABLE_ENTRY* entry_head = entry;
	
	char c_name[260];
	wchar_t w_name[260];
	
	while (true) {
#ifdef __x86_64__
		LDR_DATA_TABLE_ENTRY* temp = (LDR_DATA_TABLE_ENTRY*)((uintptr_t)(entry) - 0x10);
#else
		LDR_DATA_TABLE_ENTRY* temp = (LDR_DATA_TABLE_ENTRY*)((uintptr_t)(entry) - 0x8);
#endif
		
		UNICODE_STRING string = *(UNICODE_STRING*)&(temp->Reserved4);
		
		if (string.Buffer != NULL) {
			int len = wide_to_multi(string.Buffer, c_name);
			
			for (int i = 0; i < len; ++i) {
				if (c_name[i] <= 'Z' && c_name[i] >= 'A')
					c_name[i] -= ('Z'-'z');
			}
			
			uint32_t crc = HASH_RT(c_name);
			if (crc == module_crc) {
				*base_address = (uintptr_t)(temp->DllBase);
				return true;
			}
		}
		
		// InLoadOrderModuleList
		entry = (LDR_DATA_TABLE_ENTRY*)(entry->Reserved1[0]);
		
		if (entry->Reserved1[0] == entry_head->Reserved1[0])
			break;
	}
	
	return false;
}

bool get_export(uint32_t export_crc, uintptr_t module_address, uintptr_t* export_address) {
	uint32_t e_lfanew = *(uint32_t*)(module_address + 0x3c);
	
	/* Signature + File Header */
	uint64_t offset_to_directories = e_lfanew + 0x4 + 0x14;
	
#ifdef __x86_64__
	offset_to_directories += sizeof(IMAGE_OPTIONAL_HEADER64);
#else
	offset_to_directories += sizeof(IMAGE_OPTIONAL_HEADER32);
#endif
	
	offset_to_directories -= (sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_NUMBEROF_DIRECTORY_ENTRIES);
	offset_to_directories += (sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_DIRECTORY_ENTRY_EXPORT);
	
	IMAGE_DATA_DIRECTORY* export_directory = (IMAGE_DATA_DIRECTORY*)(module_address + offset_to_directories);
	IMAGE_EXPORT_DIRECTORY* export_dir = (IMAGE_EXPORT_DIRECTORY*)(module_address + export_directory->VirtualAddress);
	
	int count = (int)(export_dir->NumberOfNames);
	int func_count = (int)(export_dir->NumberOfFunctions);
	
	DWORD* names = (DWORD*)(module_address + export_dir->AddressOfNames);
	WORD* ordinals = (WORD*)(module_address + export_dir->AddressOfNameOrdinals);
	DWORD* funcs = (DWORD*)(module_address + export_dir->AddressOfFunctions);
	
	for (int i = 0; i < count; ++i) {
		DWORD name_address = names[i];
		
		char* name = (char*)(module_address + name_address);
		
		if (HASH_RT(name) == export_crc) {
			WORD ordinal = ordinals[i];
			DWORD func = funcs[ordinal];
			
			*export_address = module_address + (uintptr_t)func;
			
			return true;
		}
	}
	
	return false;
}

void* find_import(uint32_t import_crc) {
	for (int i = 0; i < s_import_count; ++i) {
		import_data* imp = &(s_imports[i]);
		if (imp->crc == import_crc)
			return (void*)(imp->address);
	}
	
	if (s_import_count == MAX_IMPORTS - 1)
		return nullptr;
	
	uintptr_t base_address = 0;
	uintptr_t export_address = 0;
	
	for (int i = 0; i < sizeof(s_module_hashes) / sizeof(uint32_t); ++i) {
		uint32_t module_crc = s_module_hashes[i];
		if (get_module(module_crc, &base_address)) {
			if (get_export(import_crc, base_address, &export_address)) {
				import_data* imp = &(s_imports[s_import_count++]);
				imp->crc = import_crc;
				imp->address = export_address;
				
				return (void*)(imp->address);
			}
		}
	}
	
	return nullptr;
}