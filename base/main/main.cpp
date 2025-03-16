#include "../../base.h"

__attribute((dllexport)) uintptr_t Special_Func_ptr = 0x88881414;

typedef ULONG (__stdcall* Special_Func_fn)();
__attribute((dllexport)) Special_Func_fn* Special_Func = 0x13376969;

__attribute((dllexport)) uint32_t Modified_Time = 0x99998888;

struct KUSER_SHARED_DATA {
	ULONG                         TickCountLowDeprecated;
	ULONG                         TickCountMultiplier;
	ULONG                  InterruptTime[3];
	ULONG                  SystemTime[3];
	ULONG                  TimeZoneBias[3];
	USHORT                        ImageNumberLow;
	USHORT                        ImageNumberHigh;
	WCHAR                         NtSystemRoot[260];
	ULONG                         MaxStackTraceDepth;
	ULONG                         CryptoExponent;
	ULONG                         TimeZoneId;
	ULONG                         LargePageMinimum;
	ULONG                         AitSamplingValue;
	ULONG                         AppCompatFlag;
	ULONGLONG                     RNGSeedVersion;
	ULONG                         GlobalValidationRunlevel;
	LONG                          TimeZoneBiasStamp;
	ULONG                         NtBuildNumber;
	ULONG               NtProductType;
	BOOLEAN                       ProductTypeIsValid;
	BOOLEAN                       Reserved0[1];
	USHORT                        NativeProcessorArchitecture;
	ULONG                         NtMajorVersion;
	ULONG                         NtMinorVersion;
};

#define MM_SHARED_USER_DATA_VA 0x7FFE0000
#define USER_SHARED_DATA ((KUSER_SHARED_DATA*)MM_SHARED_USER_DATA_VA)

EXPORT ULONG __stdcall Special_Func_hook() 
{
	uintptr_t frame_address = (uintptr_t)(__builtin_frame_address(0));
	uintptr_t return_address = (uintptr_t)(__builtin_return_address(0));
	
	if (g_dxapi->init()) 
	{
		char teststr[128];
		//	I(wsprintfA)(teststr, "swapchain2 %p %p %p", 0, return_address, frame_address);
		//	I(OutputDebugStringA)(teststr);
		if (g_dxapi->is_present_address(frame_address, return_address)) 
		{
			void* r12 = 0;
			void* r13 = 0;
			asm("movq %%r12, %0;"
				:"=r"(r12));
			asm("movq %%r13, %0;"
				:"=r"(r13));
			
			ULONG nBuildNumber = USER_SHARED_DATA->NtBuildNumber;
			
			IDXGISwapChain* swap = 0;
			if (nBuildNumber == 22471 || nBuildNumber == 22000 || nBuildNumber == 21996)
				swap = (IDXGISwapChain*)(*(uint64_t*)(frame_address + 0x30));
			else if (r13 != 0)
				swap = (IDXGISwapChain*)r13;
			else if (r12 != 0)
				swap = (IDXGISwapChain*)r12;
			
			//I(wsprintfA)(teststr, "swapchain3 %p %p %p", swap, return_address, frame_address);
			//I(wsprintfA)(teststr, "USER_SHARED_DATA %x %x %x", nBuildNumber, nMajorVersion, nMinorVersion);
			//I(OutputDebugStringA)(teststr);
			
			if (!g_dx11->_swapchain)
				g_dx11->_swapchain = swap;
			
			if (g_dx11->_swapchain)
				g_dxapi->frame_think();
			
			//IDirect3DDevice9* device = *(IDirect3DDevice9**)(frame_address + 0x10);
			//g_dx9->_device = device;
		
			//if (g_dx9->_device)
			//	g_dxapi->frame_think();
		}
	}
	
	if (g_dxapi->_unload)
		*(uintptr_t*)(Special_Func_ptr) = Special_Func;
	
	ULONG result = ((Special_Func_fn)(Special_Func))();
	return result;
}

bool read_memory(void* dest, uint64_t source, uint32_t data_size) 
{
	memcpy(dest, source, data_size);	
	return true;
}

static int _error_data = 0;
const bool _qword = true;

bool get_import_delayed(uint64_t module_address, uint32_t import_crc, uint64_t* import_function) 
{
	uint32_t e_lfanew = 0;
	if (!read_memory(&e_lfanew, (uint64_t)(module_address + 0x3c), sizeof(e_lfanew))) 
	{
		_error_data = __LINE__;
		return false;
	}
	
	IMAGE_DATA_DIRECTORY import_directory;
	
	// Signature + File Header
	uint64_t offset_to_directories = e_lfanew + 0x4 + 0x14;
	if (_qword)
		offset_to_directories += sizeof(IMAGE_OPTIONAL_HEADER64);
	else
		offset_to_directories += sizeof(IMAGE_OPTIONAL_HEADER32);
	
	offset_to_directories -= (sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_NUMBEROF_DIRECTORY_ENTRIES);
	offset_to_directories += (sizeof(IMAGE_DATA_DIRECTORY) * IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
	
	if (!read_memory(&import_directory, (uint64_t)(module_address + offset_to_directories), sizeof(import_directory))) {
		_error_data = __LINE__;
		return false;
	}
	
	IMAGE_DELAYLOAD_DESCRIPTOR import_desc;
	
	int size = 0;
	
	if (_qword)
		size = sizeof(IMAGE_THUNK_DATA64);
	else
		size = sizeof(IMAGE_THUNK_DATA32);
	
	char module_name[256];
	memset(module_name, 0, sizeof(module_name));
	
	char name[64];
	memset(name, 0, sizeof(name));
	
	for (int i = 0; true; ++i) {
		if (!read_memory(&import_desc, (uint64_t)(module_address + import_directory.VirtualAddress + (i * sizeof(import_desc))), sizeof(import_desc))) {
			_error_data = __LINE__;
			return false;
		}
		
		if (import_desc.DllNameRVA == 0)
			break;
		
		if (!read_memory(module_name, (uint64_t)(module_address + import_desc.DllNameRVA), 256)) {
			_error_data = __LINE__;
			return false;
		}
		
		if (_qword) {
			IMAGE_THUNK_DATA64 orig_thunk;
			
			for (int n = 0; true; ++n) {
				if (!read_memory(&orig_thunk, (uint64_t)(module_address + import_desc.ImportNameTableRVA + (n * size)), size)) {
					_error_data = __LINE__;
					return false;
				}
				
				if (orig_thunk.u1.Function == 0)
					break;
				
				if (orig_thunk.u1.AddressOfData & (1<<31))
					continue;
				
				// IMAGE_IMPORT_BY_NAME + 2
				memset(name, 0, sizeof(name));
				for (int k = 0; k < 64; ++k) {
					if (!read_memory(name + k, (uint64_t)(module_address + orig_thunk.u1.AddressOfData + 2 + k), 1))
						break;
					
					if (name[k] == '\0')
						break;
				}
				
				if (FNV1A_RT(name, strlen(name)) == import_crc) {
					uint64_t func_address = (uint64_t)(module_address + import_desc.ImportAddressTableRVA + (n * size));
					
					*import_function = func_address;
					
					/*if (!this->read_memory(import_address, func_address, sizeof(uint64_t))) {
						_error_data = __LINE__;
						return false;
					}*/
					
					return true;
				}
			}
		} else {
		/*	IMAGE_THUNK_DATA32 orig_thunk;
			
			for (int n = 0; true; ++n) {
				if (!this->read_memory(&orig_thunk, (uint64_t)(module_address + import_desc.OriginalFirstThunk + (n * size)), size)) {
					_error_data = __LINE__;
					return false;
				}
				
				if (orig_thunk.u1.Function == 0)
					break;
				
				if (orig_thunk.u1.AddressOfData & (1<<31))
					continue;
				
				// IMAGE_IMPORT_BY_NAME + 2
				memset(name, 0, sizeof(name));
				for (int k = 0; k < 128; ++k) {
					if (!this->read_memory(name + k, (uint64_t)(module_address + orig_thunk.u1.AddressOfData + 2 + k), 1))
						break;
					
					if (name[k] == '\0')
						break;
				}
				
				if (FNV1A_RT(name, strlen(name)) == import_crc) {
					uint64_t func_address = (uint64_t)(module_address + import_desc.FirstThunk + (n * size));
					
					*import_function = func_address;
					
					//if (!this->read_memory(import_address, func_address, sizeof(uint32_t))) {
					//	_error_data = __LINE__;
					//	return false;
					//}
					
					return true;
				}
			}*/
		}
	}
	
	return false;
}

typedef NTSTATUS (*NTAPI NtProtectVirtualMemory_fn)(HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
NtProtectVirtualMemory_fn sys2_NtProtectVirtualMemory;

extern "C" int entry2() {
	uint64_t d3d11_address = (uint64_t)GetModuleHandleA("d3d11.dll");
	uint64_t gdi32_address = (uint64_t)GetModuleHandleA("gdi32.dll");
	
	uint64_t entry_pointer = 0;
	get_import_delayed(d3d11_address, HASH("GdiEntry13"), &entry_pointer);
	
	uint64_t entry_original = (uint64_t)GetProcAddress(GetModuleHandleA("gdi32.dll"), "GdiEntry13");
	
	uint64_t hook_address = *(uint64_t*)(entry_pointer);
//	*(uint64_t*)(0) = 0;
	
	//sys_NtProtectVirtualMemory = (NtProtectVirtualMemory_fn)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtProtectVirtualMemory");
	
	DWORD old_prot;
	VirtualProtect((LPVOID)(entry_pointer), 8, PAGE_EXECUTE_READWRITE, &old_prot);
	//uint32_t size = 8;
//	sys_NtProtectVirtualMemory(GetCurrentProcess(), (PVOID*)hook_address, (PSIZE_T)&size, PAGE_EXECUTE_READWRITE, &old_prot);
	
	Special_Func_ptr = entry_pointer;
	Special_Func = entry_original;
	*(uint64_t*)(entry_pointer) = (uint64_t)Special_Func_hook;
	//char test[128];
//	wsprintfA(test, "test %p %p %p", entry_pointer, entry_original, hook_address);
	//MessageBoxA(0, test, "bosu", 0);
	
	return 0;
}

HANDLE global_thread  = 0;

extern "C" BOOL WINAPI entry(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
        	global_thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)entry2, 0, 0, 0);
         // Initialize once for each new process.
         // Return FALSE to fail DLL load.
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
        	TerminateThread(global_thread, 0);
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
