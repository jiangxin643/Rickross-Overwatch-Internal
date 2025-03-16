#include "base.h"

bool s_ow_init = false;

NtOpenThread_fn sys_NtOpenThread;
NtQueryInformationThread_fn sys_NtQueryInformationThread;
NtQuerySystemInformation_fn sys_NtQuerySystemInformation;
NtReadVirtualMemory_fn sys_NtReadVirtualMemory;
NtWriteVirtualMemory_fn sys_NtWriteVirtualMemory;
NtProtectVirtualMemory_fn sys_NtProtectVirtualMemory;
NtQueryVirtualMemory_fn sys_NtQueryVirtualMemory;
NtContinue_fn sys_NtContinue;

CONTEXT s_backup_context;

char s_reg_content[128];
char s_stack[512];

void __fastcall LdrParentRtlInitializeNtUserPfn_hook(EXCEPTION_RECORD* ExceptionRecord, CONTEXT* ContextFrame) {
	if (ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP) {
		if (ExceptionRecord->ExceptionAddress == g_ow->_user_cmd_func_address) {
			// rbx = input component
			// R8 = sequence number
			// R10 = server cmd
			// input + b30 = server cmd
			// r8 = sequence number
			
			uint64_t rbx = (uint64_t)(ContextFrame->Rbx);
			uint64_t r8 = (uint64_t)(ContextFrame->R8);
			uint64_t r10 = (uint64_t)(ContextFrame->R10);
			
			g_ow->_game_tickcount = (int)r8;
			g_ow->_singleton_input.component = rbx;
			g_ow->_input_cmd = (input_cmd*)(rbx + 0x1020);
			g_ow->_server_cmd = (server_cmd*)(r10);
			
			g_ow->_thread_lock = true;
			g_ow->frame_think();
			g_ow->_thread_lock = false;
		}
		
		// this 
		if (ExceptionRecord->ExceptionAddress == g_ow->_key_func_address) {
			g_ow->_test_key1 = *(uint64_t*)((uint64_t)(ContextFrame->Rsp) + 0x38 + 0x8);
			g_ow->_test_key2 = *(uint64_t*)((uint64_t)(ContextFrame->Rsp) + 0x38 + 0x10);
		} // oh ok but its different each computer :(
		
		/*if (ExceptionRecord->ExceptionAddress == g_ow->_test_break) {
			char err[128];
			I(wsprintfA)(err, "test %p", ContextFrame->Rax);
			g_console->print(err, 1.0f);
		}*/
		
		if (ExceptionRecord->ExceptionAddress == g_ow->_test_break) {
			//memcpy(s_stack, ContextFrame->Rsp - 0x100, 512);
			//char err[128];
		//	I(wsprintfA)(err, "test %p %p %p", s_stack, g_ow->_test_key1, g_ow->_test_key2);
		//	g_console->print(err, 1.0f);
		}
		
		/*if (ExceptionRecord->ExceptionAddress == g_ow->_test_break) {
			if (g_state->_rayray) {
			ray_result_t* ray_out = (ray_result_t*)(test_ray);
			char err[128];
			I(wsprintfA)(err, "[b4] %x %x %p %p", ray_out->hit_flags, ray_out->hitbox, ray_out, ContextFrame->Rcx);
	//		g_console->print(err, 1.0f);
		}
		}
		
		if (ExceptionRecord->ExceptionAddress == g_ow->_test_break2) {
			if (g_state->_rayray) {
			ray_result_t* ray_out = (ray_result_t*)(test_ray);
			char err[128];
			//uint64_t beast = *(uint64_t*)(ContextFrame->Rbp + 0xa30 - 0xa78);//*(uint16_t*)(beast + 0x38)
			float x = *(float*)(&(ContextFrame->Xmm2));
			float y = *(float*)(&(ContextFrame->Xmm2) + 4);
			float z = *(float*)(&(ContextFrame->Xmm2) + 8);
			g_esp->_rayray = Vector(x, y, z);
			I(wsprintfA)(err, "[after] %i %i %i", (int)(x * 1000.0f), (int)(y * 1000.0f), (int)(z * 1000.0f));//(int)(*(float*)(&(ContextFrame->Xmm3) + 8) * 1000.0f));
			g_console->print(err, 1.0f);
		}
		}*/
		
		if (ExceptionRecord->ExceptionAddress == g_ow->_test_break) {
			//if (((uint16_t*)(ContextFrame->Rdx))[0] == 0xa) {
			char err[128];
			I(wsprintfA)(err, "test2 0x%x 0x%x", ((uint16_t*)(ContextFrame->Rdx))[0], ((uint16_t*)(ContextFrame->Rdx))[1]);
		//	g_console->print(err, 2.0f);
		//}
		}
		
		ContextFrame->EFlags |= (1<<16);
		sys_NtContinue(ContextFrame, FALSE);
	}
}
// v2 = (uint64_t)(VirtualAlloc(Virtual, 0x400i64, 0x3000i64, 0x40i64))
// memcpy FnGetKey -> v2  // spoof
// spoof = v2;
// if then  :) 
/*
	uint64_t Core::get_component(uint64_t parent, uint8_t compid)
	{
		__try
		{
			unsigned __int64 v1 = parent;
			unsigned __int64 v2 = (uint64_t)1 << (uint64_t)(compid & 0x3F);
			unsigned __int64 v3 = v2 - 1;
			unsigned __int64 v4 = compid & 0x3F;
			unsigned __int64 v5 = compid / 0x3F;
			unsigned __int64 v6 = RPM<uint64_t>(v1 + 8 * (uint32_t)v5 + 0x88);
			__int64 v7 = (v2 & RPM<uint64_t>(v1 + 8 * (uint32_t)v5 + 0x88)) >> v4;
			unsigned __int64 v8 = (v3 & v6) - (((v3 & v6) >> 1) & 0x5555555555555555);
			unsigned __int64* v9 = (uint64_t*)(RPM<uint64_t>(v1 + 0x58) + 8 * (RPM<uint8_t>((uint32_t)v5 + v1 + 0xA8) + ((0x101010101010101 * (((v8 & 0x3333333333333333) + ((v8 >> 2) & 0x3333333333333333) + (((v8 & 0x3333333333333333) + ((v8 >> 2) & 0x3333333333333333)) >> 4)) & 0xF0F0F0F0F0F0F0F)) >> 0x38)));
			uint64_t Key1 = 0x7317465ADE98BA5Ai64;
			uint64_t Key2 = 0x57E12E367577F70Bi64;
			if (spoof!= 0x0)
			{
				reinterpret_cast<void(__fastcall*)(uint64_t*, uint64_t*)>(spoof)(&Key2, &Key1);
				uint64_t D1 = (unsigned int)*v9 | (((0x7577F70B - (unsigned int)*v9) ^ (*v9 >> 32)) << 32);
				uint64_t D2 = Key2 ^ ((unsigned int)D1
					| (((unsigned int)D1 ^ (unsigned int)~(*(unsigned __int64*)(Base + 0x2CE6700 + (Key1 >> 52)) >> 32) ^ (D1 >> 32)) << 32));
				uint64_t D3 = (unsigned int)D2 | (((unsigned int)D2 ^ (unsigned int)*(__int64*)(Base + 0x2CE6700 + (Key1 & 0xFFF)) ^ (D2 >> 32)) << 32);
				uint64_t D4 = -(int)v7 & ((unsigned int)D3 | (((unsigned int)D3 ^ 0x57E12E36 ^ (D3 >> 32)) << 32));
				return D4; // 
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}
		return 0;
	}
	
*/
/*static void decryptWithKey(uint64_t* encrypted_buf, size_t size, uint64_t* xor_key, uint64_t* decrypted_buf, uint64_t first_key)
	{
		uint64_t decrypt_key = first_key ? first_key : encrypted_buf[size] + xor_key[min(encrypted_buf[size] & 7, 7)];

		for (int i = 0; i < size; i++)
		{
			decrypted_buf[i] = encrypted_buf[i] ^ decrypt_key;
			decrypt_key += xor_key[min(decrypt_key & 7, 7)];
		}
	}
	
static uint64_t decrypt_table()
	{
		__try
		{
			uint64_t v1 = 0xA6B09E15154CBFE7;
			uint64_t v2 = 0xB3C17964B275A8AA;
			reinterpret_cast<void(__fastcall*)(uint64_t*, uint64_t*)>(spoof)(&v2, &v1);

			uint64_t qword_2D55D00 = *(uint64_t*)(Memory::GameBase + 0x2D55D00);
			D1 = (unsigned int)qword_2D55D00 | ((((unsigned int)*(_QWORD *)((char *)&_ImageBase + (v1 >> 52) + 0x2CE6700)
                                      - (unsigned int)qword_2D55D00) ^ HIDWORD(qword_2D55D00)) << 32);
  			D2 = v2 ^ ((unsigned int)D1 | (((unsigned int)~(_DWORD)qword_2D55D00 ^ __ROR4__(
                                                                           HIDWORD(*(_QWORD *)((char *)&_ImageBase
                                                                                             + (v1 & 0xFFF)
                                                                                             + 47081216)),
                                                                           11) ^ HIDWORD(D1)) << 32));
  			D3 = (unsigned int)D2 | ((~(_DWORD)D2 ^ 0x15564EB5u ^ HIDWORD(D2)) << 32);
  			D4 = (unsigned int)D2 | (((unsigned int)~(_DWORD)D3 ^ __ROR4__(
                                                          HIDWORD(*(_QWORD *)((char *)&_ImageBase
                                                                            + (v1 >> 52)
                                                                            + 0x2CE6700)),
                                                          11) ^ HIDWORD(D3)) << 32);
			uint64_t* encrypted_buf = (uint64_t*)D4;

			uint64_t xor_key[8] = {
							0xB28E031BC60C7E27ui64, //r10
							0x2DB76DE61704DE38i64, //r11
							0x4FE0232DC1436BCFi64, //rsi
							0x4A2F4481DDAA3744i64, //r12
							0x515890008ADEBC66i64, //r13
							0x1D17FE4AD0C03CBCi64, //r15
							0xC2400F6B11EF091Fi64, //r14
							0x32DF2F78EC14AB5Ei64   //r9
			};

			


			uint64_t decrypted_buf[20] = { 0, };
			size_t size = sizeof(decrypted_buf) / sizeof(uint64_t);
			// GetGManagerXOR = FnGet_GMXor
			// GetGManagerIndex = FnGet_GManagerID
			
			decryptWithKey(encrypted_buf, size, xor_key, decrypted_buf, GetGManagerXOR(encrypted_buf[20]));
			uint64_t result = decrypted_buf[GetGManagerIndex(decrypted_buf) + 4];;
			return result;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {}

		return NULL;
	}
	
	static uint64_t GetGM() // 
	{
		__try
		{
		uint64_t v1 = __ROR8__(qword_2CE7636, 0xC);
		uint64_t result = *(uint64_t*)(decrypt_table() + 0x30);
		if ((2 * v1 - 0xD2CAD390BD63C27i64) != result)
			return result; // works 
		else return 0;
		}
		__except (1) {}
		return NULL;
	}*/
bool OW_Init_Hooks() { 
	g_console->print("gamefunctions initialized", 5.0f);
	
	sys_NtOpenThread = NT_IMPORT(NtOpenThread);
	sys_NtQueryInformationThread = NT_IMPORT(NtQueryInformationThread);
	sys_NtQuerySystemInformation = NT_IMPORT(NtQuerySystemInformation);
	sys_NtReadVirtualMemory = NT_IMPORT(NtReadVirtualMemory);
	sys_NtWriteVirtualMemory = NT_IMPORT(NtWriteVirtualMemory);
	sys_NtProtectVirtualMemory = NT_IMPORT(NtProtectVirtualMemory);
	sys_NtQueryVirtualMemory = NT_IMPORT(NtQueryVirtualMemory);
	sys_NtContinue = NT_IMPORT(NtContinue);
	
	uint64_t KiUserApcDispatcher = (uint64_t)(find_import(HASH("KiUserApcDispatcher")));
	
	// KiUserExceptionDispatcher
	uint16_t jz = *(uint16_t*)(KiUserApcDispatcher + 0x23);
	if ((jz & 0xFF) != 0x74)
		return false;
	
	uint64_t mov = KiUserApcDispatcher + 0x23 + 0x2 + (uint64_t)(jz >> 8) + 0x6;
	
	for (int i = 0; i < 16; ++i) {
		if (*(uint16_t*)mov == 0x8b48)
			break;
		
		mov += 1;
	}
	
	if (*(uint16_t*)mov != 0x8b48)
		return false;
	
	uint32_t rel32 = *(uint32_t*)(mov + 0x3);
	uint64_t LdrParentRtlInitializeNtUserPfn = (mov + 0x7 + (uint64_t)rel32) + 0x8;
	
	if (true) {
		DWORD old_protect;
		I(VirtualProtect)(LdrParentRtlInitializeNtUserPfn, 8, PAGE_READWRITE, &old_protect);
		*(uint64_t*)(LdrParentRtlInitializeNtUserPfn) = (uint64_t)LdrParentRtlInitializeNtUserPfn_hook;
		
		g_console->print("prime loaded", 5.0f);
	}
	
	//g_hooks->attach();
	
	return true;
}

bool OW_Init() {
	if (s_ow_init)
		return true;
	
	g_console->print("prime injected", 5.0f);
	
	if (!OW_Init_Hooks())
		return false;
	
	s_ow_init = true;
	
	return true;
}

void OW_InitX(float a, matrix_t& m) {
	float s;
	float c;
	sincosf(a, &s, &c);
	
	m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = c;
    m[1][2] = s;
    m[1][3] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = -s;
    m[2][2] = c;
    m[2][3] = 0.0f;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

void OW_InitY(float a, matrix_t& m) {
	float s;
	float c;
	sincosf(a, &s, &c);
	
	m[0][0] = c;
	m[0][1] = 0.0f;
	m[0][2] = -s;
	m[0][3] = 0.0f;
	
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	
	m[2][0] = s;
	m[2][1] = 0.0f;
	m[2][2] = c;
	m[2][3] = 0.0f;
	
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

void OW_Transform(Vector vec, matrix_t m, Vector* out) {
	Vector Z = Vector(vec.z, vec.z, vec.z);
	Vector Y = Vector(vec.y, vec.y, vec.y);
	Vector X = Vector(vec.x, vec.x, vec.x);
	
	Vector result = Vector(Z.x * m[2][0] + m[3][0], Z.y * m[2][1] + m[3][1], Z.z * m[2][2] + m[3][2]);
	result = Vector(Y.x * m[1][0] + result.x, Y.y * m[1][1] + result.y, Y.z * m[1][2] + result.z);
	result = Vector(X.x * m[0][0] + result.x, X.y * m[0][1] + result.y, X.z * m[0][2] + result.z);
	
	*out = result;
}