#include "base.h"

c_ow ow_instance;
c_ow* g_ow = &ow_instance;

bool c_ow::init() {
	if (_init)
		return !_failed;
	
	_init = true;
	_failed = true;
	
	{
		CLIENT_ID client_id;
		if (!this->get_main_thread(&client_id)) {
			g_console->error("no main thread", 4.0f);
			return false;
		}
		
		_thread_id = client_id.UniqueThread;
	}
	
	_base_address = (uint64_t)(I(GetModuleHandleA)(0));
	
	//0xBED539
	_user_cmd_func_address = _base_address + 0xC0CB09; // it just same 3bComponent + 0x1034 
	
	_key_func_address = _base_address + 0x177FCE7;
	_ray_func_address = _base_address + 0xBB1800;
	
	_fnsilentaim_func_address = _base_address + 0x687390;
	
	_test_skill_struct = _base_address + 0x1BA8520;
	
	//7D1D75
	_test_break = _base_address + 0x9F6A9F        ; // fnsilentaim
	//_test_break = _base_address + 0xA1348F; // skill dump .text:0000000000A1348F                 mov     rcx, [rax+1F0h]
	_test_break2 = _base_address + 0x1B0EBB3  ; //0xBB2E59       ;
	
	{
		for (int i = 0; i < 128; ++i) {
			uint64_t id = g_pool_ids[i];
			if (id == 0)
				break;
			
			_pool_id_valid[id & 0xFFFF] = true;
		}
	}
	
	{
		HANDLE t = I(OpenThread)(THREAD_ALL_ACCESS, FALSE, _thread_id);
		if (t != NULL) {
			THREAD_BASIC_INFORMATION info;
			sys_NtQueryInformationThread(t, ThreadBasicInformation, &info, sizeof(THREAD_BASIC_INFORMATION), NULL);
			
			_stack_base = *(uint64_t*)(info.TebBaseAddress + 0x10);
			//char err[128];
			//I(wsprintfA)(err, "test %p", info.TebBaseAddress);
			//g_console->error(err, 10.0f);
			
	 		CONTEXT context = {0};
			context.ContextFlags = CONTEXT_ALL;
			
			I(GetThreadContext)(t, &context);
			
			context.Dr0 = (DWORD64)_user_cmd_func_address;
			context.Dr1 = (DWORD64)_key_func_address;
			//context.Dr2 = (DWORD64)_fnsilentaim_func_address;
		//	context.Dr2 = (DWORD64)_test_break;
		//	context.Dr3 = (DWORD64)_test_break;
			//context.Dr7 = 1<<0 | 1<<2 | 1<<4 | 1<<6;
			context.Dr7 = 1<<0 | 1<<2;
			context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
			
			I(SetThreadContext)(t, &context);
			
			I(CloseHandle)(t);
		}
	}
	
	_failed = false;
	
	return true;
}

void c_ow::unload() {
	if (_init == false || _failed  == true)
		return;
	
	if (g_dxapi->_unload)
		return;
	
	{
		HANDLE t = I(OpenThread)(THREAD_ALL_ACCESS, FALSE, _thread_id);
		if (t != NULL) {
			CONTEXT context = {0};
			context.ContextFlags = CONTEXT_ALL;
			
			I(GetThreadContext)(t, &context);
			
			context.Dr0 = 0;
			context.Dr1 = 0;
			context.Dr2 = 0;
			context.Dr3 = 0;
			context.Dr7 = 0;
			context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
			
			I(SetThreadContext)(t, &context);
			
			I(CloseHandle)(t);
		}
	}
}

void* c_ow::alloc(int size) {
	return (void*)(I(VirtualAlloc)(NULL, (SIZE_T)size, MEM_COMMIT, PAGE_READWRITE));
}

void c_ow::free(void* ptr) {
	I(VirtualFree)(ptr, 0, MEM_RELEASE);
}

bool c_ow::read(void* dest, uint64_t source, uint32_t size) {
	SIZE_T bytes_read = 0;
	NTSTATUS status = sys_NtReadVirtualMemory((HANDLE)-1, (PVOID)source, (PVOID)dest, (ULONG)size, (PULONG)&bytes_read);
	if (status != STATUS_SUCCESS) {
		//char err[128];
		//I(wsprintfA)(err, "c_ow::read failed %x", status);
	//	g_console->error(err, 4.0f);
		
		return false;
	}
	
	return (uint32_t)bytes_read == size;
}

static char test_buf[0x1000];
static char test_buf2[0x1000];

void c_ow::think() {
	if (g_dxapi->_unload)
		return;
	
	this->check_cache();
	
	/*if (_test_key1 == 0 || _test_key2 == 0) {
		uint64_t fuck = _base_address + 0xD7C83F;
		uint64_t fuck2 = _base_address + 0xD7FA6B;
		//uint64_t fuck = _base_address + 0xDBD10F;
		//uint64_t fuck2 = _base_address + 0xDC08CB;
		bool stop = false;
		
		for (int i = 0; i < 0xFA000; i += 0x1000) {
			uint64_t test = _stack_base + i;
			memcpy(test_buf, test, 0x1000);
			for (int n = 0; n < 0x1000; ++n) {
				if (n > 0x30 && *(uint8_t*)(test_buf + n - 0x10) == 0xC && *(uint64_t*)(test_buf + n) == fuck && *(uint64_t*)(test_buf + n - 0x30) == fuck2) {
					memcpy(test_buf2, test, 0x1000);
					char err[128];
					I(wsprintfA)(err, "test %x %p", n, test_buf2);
					g_console->error(err, 4.0f);
					int64_t key1 = n > 0x28 ? *(uint64_t*)(test_buf + n - 0x20) : 0;
					uint64_t key2 = n > 0x20 ? *(uint64_t*)(test_buf + n - 0x18) : 0;
					stop = true;
					
					if (_test_key1 == 0 && key1 > 0xFFFFFFFF)
						_test_key1 = key1;
					
					if (_test_key2 == 0 && key1 > 0xFFFFFFFF)
						_test_key2 = key2;
					
					break;
				}
			}
			
			if (stop)
				break;
		}
		
		
	}*/
	
	if (_state_updated)
		g_esp->paint();
}

void c_ow::frame_think() {
	server_cmd* ucmd = _server_cmd;
	
	if (g_dxapi->_attempt_unload) {
		_unload_user_cmd = true;
		return;
	}
	
	if (g_dxapi->_time_limit > GLOBAL_TIME_LIMIT)
		return;
	
	if (_max_hit) {
		g_console->error("max components hit", -1.0f);
		return;
	}
	
	//this->check_cache();
	
	float curtime = (float)(_game_tickcount) * OW_TICK_INTERVAL;
	
	g_state->_curtime = curtime;
	
	_state_updated = this->update_state();
	_cmd.bits = 0;
	
	_reset = false;
	
	if (!_state_updated)
		return;
	
	if (local->object.health < 1.0f)
		return;
	
	input_cmd* in = _input_cmd;
	Vector forward = Vector(in->forward_x, in->forward_y, in->forward_z);
	
	Vector test_ang = Vector(SHORT2ANGLE(ucmd->pitch_quake), SHORT2ANGLE(ucmd->yaw_quake), 0.0f);
	forward = test_ang.forward().normal();
	
	g_esp->_viewforward = forward;
	
	_cmd.forward = forward;
	_cmd.silent = forward;
	_cmd.buttons = ucmd->buttons;
	_cmd.forwardmove = ucmd->forwardmove;
	_cmd.sidemove = ucmd->sidemove;
	
	g_anim->_tick_force_ms = 0;
	
	bool bot = false;
	if (local->object.hero == HERO_LUCIO)
		bot = true;
	if (local->object.hero == HERO_BRIG)
		bot = true;
	if (local->object.hero == HERO_REINHARDT)
		bot = true;
	if (local->object.hero == HERO_ANA)
		bot = true;
	if (local->object.hero == HERO_HAMMOND)
		bot = true;
	
	if (g_cvars_misc->heal_bot.value == 0)
		bot = false;
//	if (bot)
//		_cmd.buttons |= IN_JUMP;
	
	g_anim->think();
	
	g_hero->think();
	
	g_state->update_aim_rays();
	g_state->update_trig_rays();
	
	g_aim->think();
	
	g_anim->history_aim();
	
	g_aim->force_tick();
	
	g_hero->trig_think();
	g_trig->think();
	
	g_hero->fire_time_think();
	
	g_hero->auto_melee();
	
	if (local->fire_ready) {
		if (local->object.hero == HERO_HANZO && local->ability_limit != 0x5a1) {
			if (!(_cmd.buttons & IN_ATTACK) && _last_buttons & IN_ATTACK)
				local->firing = true;
		} else {
			if (_cmd.buttons & IN_ATTACK)
				local->firing = true;
		}
	}
	
	g_anim->force_tick();
	
	bool avoid_force = false;
	
	if (local->object.hero == HERO_ECHO && _cmd.buttons & IN_ABILITY3)
		avoid_force = true;
	
	server_cmd* scmd = _server_cmd;
	//server_cmd* scmd2 = (server_cmd*)((uint64_t)(_server_cmd) + (0x30 * 3));
	
	Vector angles = Vector(_cmd.silent.pitch(), _cmd.silent.yaw(), 0.0f);
	
	bool firing = false;
	
	if (local->firing)
		firing = true;
	
	if (_cmd.buttons & IN_ABILITY1
		|| _cmd.buttons & IN_ABILITY2
		|| _cmd.buttons & IN_ABILITY3
		|| _cmd.buttons & IN_MELEE
		|| _cmd.buttons & IN_JUMP)
		firing = true;
	
	if (g_cvars_misc->anti_aim.value != 0 && !firing) {
		angles.x = 89.0f;
		
		switch (g_cvars_misc->anti_aim.value) {
			case 1: {
				angles.y += 180.0f;
				break;
			}
			case 2: {
				angles.y += _game_tickcount % 2 ? (135.0f) : (-135.0f);
				break;
			}
			case 3: {
				angles.y += _game_tickcount % 2 ? (90.0f) : (-90.0f);
				break;
			}
			case 4: {
				angles.y += (float)(_game_tickcount % 18) * 20.0f;
				break;
			}
		}
		
		NormalizeAngle(&(angles.y));
		
		_cmd.silent = angles.forward().normal();
	}
	
	g_hero->zarya_super_jump();
	
	if (_cmd.bits & UCMD_FORWARD) {
		in->forward_x = _cmd.forward.x;
		in->forward_y = _cmd.forward.y;
		in->forward_z = _cmd.forward.z;
	}
	
	scmd->buttons = _cmd.buttons;
//	scmd->sidemove = 0x7f;
	
	float move_yaw = forward.yaw();
	
	if (bot && !(scmd->buttons & IN_USE)) {
		if (g_hero->_beasd) {
			move_yaw = g_hero->_beasd_dir.yaw();
			
			if (g_hero->_beasd_dist > 1.0f)
				scmd->forwardmove = 0x7f;
			
			if (g_hero->_beasd_dist < 2.0f) {
				if ((_game_tickcount % (62/2)) > (62/4))
					scmd->sidemove = 0x7f;
				else if ((_game_tickcount % (62/2)) > 0)
					scmd->sidemove = 0x81;
			} else if (g_hero->_beasd_dist > 20.0f) {
				if ((_game_tickcount % (62*6)) > (62*3))
					scmd->sidemove = 0x7f;
				else if ((_game_tickcount % (62*6)) > 0)
					scmd->sidemove = 0x81;
			}
			
			if (local->object.ult_charge > 0.99f) {
				if ((_game_tickcount % (2)) == 0)
					scmd->buttons |= IN_ABILITY3;
			}
		} else {
			if ((_game_tickcount % (62*2)) == 0)
				_move_seed = FNV1A_RT((char*)&(g_ow->_game_tickcount), 4);
			
			scmd->forwardmove = 0x7f;
			
			move_yaw = (float)(_move_seed % 8) * 45.0f;
		}
		
		if (local->object.hero == HERO_ANA) {
			//if ((_game_tickcount % (62*2)) < 31) {
			if ((_game_tickcount % (62*1)) == 0 && local->object.total_hp < 175.0f) {
				scmd->buttons |= IN_ABILITY2;
				_cmd.silent = Vector(89.0f, 0.0f, 0.0f).forward();
				angles.x = 89.0f;
			}
			
			if ((_game_tickcount % (62*1)) == 0)
				scmd->buttons |= IN_ABILITY1;
		} else {
			if ((_game_tickcount % (62*1)) == 0)
				scmd->buttons |= IN_ABILITY2;
		}
		
		if (local->object.hero == HERO_HAMMOND) {
			if ((_game_tickcount % (62*1)) == 0)
				scmd->buttons |= IN_ABILITY2;
			
			if ((_game_tickcount % (62*1)) == 0)
				scmd->buttons |= IN_DUCK;
			
			if ((_game_tickcount % (62*1)) == 0)
				scmd->buttons |= IN_ATTACK2;
		}
		
		if (local->object.hero == HERO_BRIG || local->object.hero == HERO_REINHARDT) {
			scmd->buttons |= IN_ATTACK;
			
			//if ((_game_tickcount % (62*1)) == 0)
				//scmd->buttons |= IN_ABILITY1;
		}
		
		if (local->object.hero == HERO_LUCIO && local->cross_fade == 0.0f) {
			if ((_game_tickcount % (62*1)) == 0)
				scmd->buttons |= IN_ABILITY1;
		}
	}
	
	if (forward != _cmd.silent) {
		Vector silent_angles = Vector(_cmd.silent.pitch(), _cmd.silent.yaw(), 0.0f);
		
		scmd->pitch_quake = ANGLE2SHORT(silent_angles.x);
		scmd->yaw_quake = ANGLE2SHORT(silent_angles.y);
	}
	
	if (move_yaw != angles.y) {
		float f = 0.0f;
		float s = 0.0f;
		
		if (scmd->forwardmove == 0x7F)
			f = 1.0f;
		else if (scmd->forwardmove == 0x81)
			f = -1.0f;
		
		if (scmd->sidemove == 0x7F)
			s = -1.0f;
		else if (scmd->sidemove == 0x81)
			s = 1.0f;
		
		Vector move_dir = Vector(f, 0.0f, s);
		
		if (move_dir.x != 0.0f || move_dir.z != 0.0f) {
			float angle = move_dir.yaw();
			
			float view_yaw = move_yaw;
			if (view_yaw < 0.0f)
				view_yaw += 360.0f;
			
			float yaw = angles.y;
			if (yaw < 0.0f)
				yaw += 360.0f;
			
			angle += yaw - view_yaw;
			if (angle < 0.0f)
				angle += 360.0f;
			
			angle = fmodf(angle, 360.0f);
			
			Vector new_move = Vector(0.0f, angle, 0.0f).forward().normal() * 100.0f;
			
			scmd->forwardmove = MOVE2BYTE(new_move.x);
			scmd->sidemove = MOVE2BYTE(-new_move.z);
		}
	}
	
	_last_buttons = scmd->buttons;
}

bool c_ow::update_state() {
	ow_object* obj = &(local->object);
	memset(local, 0, sizeof(ow_local));
	
	g_state->_object_count = 0;
	memset(g_state->_objects, 0, sizeof(g_state->_objects));
	
	cmp_input* input = (cmp_input*)(_singleton_input.component);
	
	obj->player_parent = this->get_decrypted_parent(input->_parent);
	
	if (obj->player_parent == 0 || !this->is_valid_page(obj->player_parent))
		return false;
	
	_gm = (game_manager_t*)(*(uint64_t*)(obj->player_parent + 0x28) ^ 0x9D1F293863EE4394);
	if (!this->is_valid_page(_gm))
		return false;
	
	{
		cmp_link* link = (cmp_link*)(this->lookup_component(obj->player_parent, CMPID_LINK));
		if (link)
			obj->link_id = link->get_link_id();
	}
	/*cmp_health* hp = (cmp_health*)(g_local->lookup_component(CMPID_HEALTH));
	if (hp != 0)
		obj->entity_parent = this->get_decrypted_parent(hp->_parent);
	*/
	this->add_objects();
	
	if (obj->entity_parent == 0 || !this->is_valid_page(obj->entity_parent))
		return false;
	
	if (!g_local->update())
		return false;
	
	g_state->update_objects();
	
	return true;
}

void c_ow::add_objects() {
	uint64_t entity_admin = _gm->entity_admin;
	if (entity_admin == 0)
		return;
	
//	entity_admin = *(uint64_t*)(entity_admin + 0x18);
	map_element* hash_map = *(map_element**)(entity_admin + 0x8);
	
	uint64_t links[64];
	int link_count = 0;
	int admin_count = 0;
	
	for (int i = 0; i < 1024; ++i) {
		map_element* e = &(hash_map[i]);
		
		if (i == 1024 - 1) {
			entity_admin = *(uint64_t*)(entity_admin + 0x18);
			if (entity_admin != 0) {
				i = 0;
				hash_map = *(map_element**)(entity_admin + 0x8);
				admin_count += 1;
			}
		}
		
		if (e->hash == -1)
			continue;
		
		object_pool* pool = (object_pool*)(&(e->pool));
		uint64_t type = pool->get_type();
		if (type == 0)
			continue;
		
		if (!_pool_id_valid[type & 0xFFFF])
			continue;
		
		if (pool == local->object.player_parent)
			continue;
		
		cmp_health* hp = (cmp_health*)(g_ow->lookup_component(pool, CMPID_HEALTH));
		if (hp != 0) {
			if (pool->get_link_id() == local->object.link_id) {
			//	g_console->print("pool->get_link_id() == local->object.link_id", 1.0f);
				local->object.entity_parent = (uint64_t)pool;
			//	local->object.pool_type = pool->get_type();
				continue;
			}
			
			int index = g_state->_object_count;
			
			ow_object* obj = &(g_state->_objects[index]);
			obj->index = index;
			obj->type = OBJECT_INVALID;
			
			obj->entity_parent = (uint64_t)pool;
			obj->player_parent = 0;
			obj->link_id = pool->get_link_id();
			obj->pool_type = pool->get_type();
			
			g_state->_object_count += 1;
			continue;
		}
		
		if (link_count < 64) {
			cmp_link* link = (cmp_link*)(g_ow->lookup_component(pool, CMPID_LINK));
			if (link != 0)
				links[link_count++] = (uint64_t)link;
		}
	}
	
	for (int i = 0; i < g_state->_object_count; ++i) {
		ow_object* obj = &(g_state->_objects[i]);
		
		for (int n = 0; n < link_count; ++n) {
			cmp_link* link = (cmp_link*)links[n];
			
			if (obj->link_id == link->get_link_id()) {
				obj->player_parent = this->get_decrypted_parent(link->_parent);
				break;
			}
		}
	}
}

void c_ow::check_cache() {
	if (_thread_lock == false && _gm != 0) {
		if (!this->is_valid_page(_gm))
			_gm = 0;
		else if (*(uint64_t*)(_gm) < _base_address || *(uint64_t*)(_gm) > _base_address + 0x3500000)
			_gm = 0;
	}
	
	if (_gm == 0) {
		memset(g_local, 0, sizeof(c_local));
		memset(g_state, 0, sizeof(c_state));
		memset(g_hero, 0, sizeof(c_hero));
		memset(g_tank, 0, sizeof(c_tank));
		memset(g_offense, 0, sizeof(c_offense));
		memset(g_defense, 0, sizeof(c_defense));
		memset(g_genji, 0, sizeof(c_genji));
		memset(g_aim, 0, sizeof(c_aim));
		memset(g_trig, 0, sizeof(c_trig));
	}
}

bool c_ow::is_valid_page(uint64_t address) {
	MEMORY_BASIC_INFORMATION mbi;
	PSAPI_WORKING_SET_EX_INFORMATION wsi;
	int mbi_size = sizeof(mbi);
	int wsi_size = sizeof(wsi);
	
	SIZE_T size_out = 0;
	DWORD result = sys_NtQueryVirtualMemory((HANDLE)-1, address, MemoryBasicInformation, (PVOID)&mbi, mbi_size, &size_out);
	if (result != 0 || size_out == 0)
		return false;
	
	//if (page_size != 0 && (u32)(mbi.RegionSize) != page_size)
	//	return false;
	
	if ((uint64_t)(mbi.BaseAddress) > _base_address)
		return false;
	
	if ((mbi.State & 0x1000/*MEM_COMMIT*/) == 0 || (mbi.Protect & 0x100/*PAGE_GUARD*/) != 0 || (mbi.Protect & 0x4/*PAGE_READWRITE*/) == 0)
		return false;
	
	wsi.VirtualAddress = mbi.BaseAddress;
	
	result = sys_NtQueryVirtualMemory((HANDLE)-1, 0, MemoryWorkingSetExList, (PVOID)&wsi, wsi_size, 0);
	if (result != 0)
		return false;
	
	if (wsi.VirtualAttributes.Valid == false)
		return false;
	
	return true;
}

bool c_ow::get_main_thread(CLIENT_ID* client_id) {
	bool success = false;
	NTSTATUS status = 0;
	
	ULONG size = 0;
	status = sys_NtQuerySystemInformation(SystemProcessInformation, 0, 0, &size);
	if (status != STATUS_SUCCESS && status != STATUS_INFO_LENGTH_MISMATCH) {
		return false;
	}
	
	void* mem = this->alloc(size);
	if (mem == 0)
		return false;
	
	void* orig_mem = mem;
	memset(mem, 0, size);
	
	status = sys_NtQuerySystemInformation(SystemProcessInformation, mem, size, NULL);
	if (status != STATUS_SUCCESS && status != STATUS_INFO_LENGTH_MISMATCH) {
		return false;
	}
	
	void* cur_proc = mem;
	
	DWORD cur_proc_id = I(GetCurrentProcessId)();
	
	while (true) {
		SYSTEM_PROCESS_INFORMATION* process = (SYSTEM_PROCESS_INFORMATION*)cur_proc;
		if (process->NextEntryOffset == 0)
			break;
		
		DWORD test = (DWORD)(process->UniqueProcessId);
		if (test == cur_proc_id) {
			SYSTEM_THREAD_INFORMATION* thread = (SYSTEM_THREAD_INFORMATION*)((uintptr_t)(process) + sizeof(SYSTEM_PROCESS_INFORMATION));
			
			if (thread->StartAddress == 0) {
				success = true;
				client_id->UniqueProcess = thread->ClientId.UniqueProcess;
				client_id->UniqueThread = thread->ClientId.UniqueThread;
			}
			
			break;
		}
		
		cur_proc += process->NextEntryOffset;
	}
	
	this->free(orig_mem);
	
	return success;
}

uint64_t c_ow::get_decrypted_parent(uint64_t parent) {
	uint64_t v1 = *(uint64_t*)(_base_address + 0x2D1D226);
	return parent ^ (2 * __rorq(v1, 4) + 0x736BBC322F3464E1);
}

uint64_t c_ow::lookup_entry(uint64_t parent, uint64_t id) {
	uint64_t id_bit = (uint64_t)(1) << (uint64_t)(id & 0x3F);
	uint64_t id_shr6 = id >> 6;
	
	uint64_t temp = parent + (id_shr6 * 8);
	if (!this->is_valid_page(temp))
		return 0;
	
	uint64_t ptr = *(uint64_t*)(temp + 0x88);
	if (ptr == 0)
		return 0;
	
	uint64_t ptr_and = ptr & id_bit;
	if (ptr_and == 0)
		return 0;
	
	uint64_t ptr_and2 = ptr & (id_bit - 1);
	
	uint64_t crypt_v7 = ptr_and2 - ((ptr_and2 >> 1) & 0x5555555555555555);
	uint64_t crypt_v8 = ((0x101010101010101 * (((crypt_v7 & 0x3333333333333333) + ((crypt_v7 >> 2) & 0x3333333333333333) + (((crypt_v7 & 0x3333333333333333) + ((crypt_v7 >> 2) & 0x3333333333333333)) >> 4)) & 0xF0F0F0F0F0F0F0F)) >> 56);
	uint64_t index = *(uint8_t*)(parent + id_shr6 + 0xA8) + crypt_v8;
	if ((index & 0x80000000) != 0)
		return 0;
	
	uint64_t list = *(uint64_t*)(parent + 0x58);
	uint64_t* entry = (uint64_t*)(list + ((uint64_t)(index) * 8));
	
	return entry;
}

#define LOWORD(_dw)     ((WORD)(((DWORD_PTR)(_dw)) & 0xffff))
#define HIWORD(_dw)     ((WORD)((((DWORD_PTR)(_dw)) >> 16) & 0xffff))
#define LODWORD(_qw)    ((DWORD)(_qw))
#define HIDWORD(_qw)    ((uint64_t)(((_qw) >> 32) & 0xffffffff))

// (uint32_t)v1 == v1 & 0xFFFFFFFF
uint64_t c_ow::decrypt_entry(uint64_t* entry) {
	uint64_t key1 = g_ow->_test_key1;
	uint64_t key2 = g_ow->_test_key2;
	
	uint64_t key1_lo = g_ow->_test_key1 & 0xFFFFFFFF;
	uint64_t key2_lo = g_ow->_test_key2 & 0xFFFFFFFF;
	
	uint64_t deref = *entry;
	uint64_t deref_lo = deref & 0xFFFFFFFF;
	uint64_t deref_hi = deref >> 32;
	
	/*uint64_t temp = g_ow->_base_address + 0x2D8C240;
	uint64_t key_a = *(uint64_t*)(temp + (key1 >> 52));
	uint64_t key_a_hi = key_a >> 32;
	uint64_t key_b = *(uint64_t*)(temp + (key1 & 0xFFF));
	uint64_t key_b_lo = key_b & 0xFFFFFFFF;
	
	uint64_t v1 = deref_lo | (((0xF1F76D40 - deref_lo) ^ deref_hi) << 32); // maybe diff
	uint64_t v1_lo = v1 & 0xFFFFFFFF;
	uint64_t v1_hi = v1 >> 32;
	
	uint64_t v11 = key2_lo ^ deref_lo | (((key2_lo ^ deref_lo ^ key_b_lo ^ ((key2 ^ (deref_lo | ((deref_lo ^ (unsigned int)~key_a_hi) ^ v1_hi) << 32))) >> 32)) << 32);
	uint64_t v14 = v11 >> 32;
	
	uint64_t v13 = (key2_lo ^ deref_lo) & 0xFFFFFFFF; // same
	
	uint64_t v12 = (key2 ^ deref_lo);
	uint64_t v15 = v12 ^ 0x57E12E36;
	
	uint64_t result = v13 | ((v15 ^ v14) << 32); // same*/
	
	uint64_t temp = g_ow->_base_address + 0x2D1D000;
	
	uint64_t* v11 = entry;
	uint64_t v25 = key1;
	uint64_t v26 = key2;
	
	/*uint64_t v10 = (unsigned int)(*v9) | ((((unsigned int)(*v9) + __rold(*(uint64_t *)(temp + (v18 >> 52)), 1)) ^ HIDWORD(*v9)) << 32);
	uint64_t v11 = (unsigned int)(v19) ^ (unsigned int)(v10);
	uint64_t v12 = v11 | (((0xF1F76D40 - ((unsigned int)(v19) ^ (unsigned int)(v10))) ^ ((uint64_t)(v19 ^ ((unsigned int)(v10) | (((unsigned int)(v10) ^ ~HIDWORD(*(uint64_t*)(temp + (v18 & 0xFFF))) ^ HIDWORD(v10)) << 32))) >> 32)) << 32);
	uint64_t v14 = (unsigned int)(v12) ^ HIDWORD(*(uint64_t*)(temp + (v18 >> 52)));
	uint64_t v15 = HIDWORD(v12);
	uint64_t v16 = (uint64_t)((unsigned int)(v11) | ((v14 ^ v15) << 32));*/
	
	/*uint64_t v10 = v20 ^ ((unsigned int)*v9 | (((unsigned int)*v9 ^ (unsigned int)~HIDWORD(*(uint64_t*)(temp + (v19 >> 52))) ^ (((unsigned int)*v9 | (((uint64_t)(0x3DC2DBC6 - (unsigned int)*v9) ^ HIDWORD(*v9)) << 32)) >> 32)) << 32));
	uint64_t v11 = (unsigned int)v20 ^ (unsigned int)*v9;
	uint64_t v12 = v11 | (((unsigned int)(v10 + __rold(*(uint64_t*)(temp + (v19 & 0xFFF)), 1)) ^ HIDWORD(v10)) << 32);
	uint64_t v13 = (unsigned int)v11;
	uint64_t v14 = 0xFC9B7DD2 - (unsigned int)v11;
	uint64_t v16 = HIDWORD(v12);
	uint64_t v17 = (uint64_t)(v13 | ((v14 ^ v16) << 32));
	*/
	
	/*uint64_t v10 = (unsigned int)*v9;
	uint64_t v11 = v10 | (((0xBEEBB2E - (unsigned int)*v9) ^ HIDWORD(*v9)) << 32);
	uint64_t v12 = v20 ^ ((unsigned int)v10 | (((unsigned int)(2 * __rord(HIDWORD(*(uint64_t*)(temp + (v19 >> 52))), 3) - v11) ^ HIDWORD(v11)) << 32));
	uint64_t v13 = (unsigned int)v20 ^ (unsigned int)*v9;
	uint64_t v14 = 0xAB95D972 - (unsigned int)v13;
	uint64_t v15 = 0;
	uint64_t v16 = (uint64_t)(v13 | ((((unsigned int)*(uint64_t*)(temp + (v19 & 0xFFF)) - (unsigned int)v12) ^ HIDWORD(v12)) << 32)) >> 32;
	uint64_t v17 = (uint64_t)((unsigned int)v13 | ((v14 ^ v16) << 32));
	*/
	
	/*uint64_t v10 = (unsigned int)*v9;
	uint64_t v11 = v10 | (((0xF6E2FA3E - (unsigned int)*v9) ^ HIDWORD(*v9)) << 32);
	uint64_t v12 = v21 ^ ((unsigned int)v10 | (((unsigned int)(v11
	                                      + __rold(
	                                          HIDWORD(*(uint64_t*)(temp + (v20 >> 52))),
	                                          1)) ^ HIDWORD(v11)) << 32));
	uint64_t v13 = (unsigned int)v21 ^ (unsigned int)*v9;
	uint64_t v14 = v13 | (((unsigned int)~(uint32_t)v12 ^ __rord(*(uint64_t*)(temp + (v20 & 0xFFF)), 11) ^ HIDWORD(v12)) << 32);
	uint64_t v15 = (unsigned int)(0x4B59AC11 - v14);
	uint64_t v16 = 0;
	uint64_t v17 = HIDWORD(v14);
	uint64_t v18 = (uint64_t*)((unsigned int)v13 | ((v15 ^ v17) << 32));
	*/
	
	uint64_t v12 = (unsigned int)*v11;
	uint64_t v13 = (unsigned int)v26 ^ (unsigned int)v12 | (((2 * __rord(*(uint64_t*)(temp + (v25 & 0xFFF)), 3)
                                                 - ((unsigned int)v26 ^ (unsigned int)v12)) ^ ((v26 ^ (v12 | (((unsigned int)(HIDWORD(*(uint64_t *)(temp + (v25 >> 52))) - *v11) ^ ((v12 | (((unsigned __int64)(0xBF382D50 - (unsigned int)*v11) ^ HIDWORD(*v11)) << 32)) >> 32)) << 32))) >> 32)) << 32);
	uint64_t v14 = v13;
	uint64_t v15 = (unsigned int)v13;
	uint64_t v16 = HIDWORD(v13);
	uint64_t v17 = v14 ^ 0x3B3C7414u;
	uint64_t v18 = 0;
	uint64_t v19 = (uint64_t*)(v15 | ((v17 ^ v16) << 32));
	
	uint64_t result = v19;
	
	return result;
}

uint64_t c_ow::lookup_component(uint64_t parent, uint64_t id) {
	if (_test_key1 == 0 || _test_key2 == 0)
		return 0;
	
	uint64_t* entry = lookup_entry(parent, id);
	if (entry == 0)
		return 0;
	
	uint64_t decrypted = decrypt_entry(entry);
	if (decrypted == 0)
		return 0;
	
	uint64_t id_bit = (uint64_t)(1) << (uint64_t)(id & 0x3F);
	uint64_t id_shr6 = id >> 6;
	
	uint64_t temp = parent + (id_shr6 * 8);
	uint64_t ptr = *(uint64_t*)(temp + 0x88);
	
	uint64_t entry_key = (id_bit & ptr) >> (id & 0x3F);
	
	uint64_t result = (~entry_key) & (decrypted);
	return result;
}