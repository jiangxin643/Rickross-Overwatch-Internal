#include "base.h"

#include "../menu_test/menu.h"

c_dxapi dxapi_instance;
c_dxapi* g_dxapi = &dxapi_instance;

bool c_dxapi::init() {
	if (_init)
		return !_failed;
	
	_init = true;
	_failed = true;
	
	_d3d_base_address = I(GetModuleHandleA)("d3d11.dll");
	
	if (!OW_Init())
		return false;
	
	_failed = false;
	
	_time_limit = 0;
	
	g_tMenu->Init();
	//InitializeMenu();
	
	return true;
}

void c_dxapi::unload() {
	_attempt_unload = true;
	
	//if (g_ow->_unload_user_cmd != true)
	//	return;
	
	//g_hooks->detach();
	g_ow->unload();
	
	_unload = true;
}

bool c_dxapi::is_present_address(uintptr_t frame_address, uintptr_t return_address) {
	if (return_address < _d3d_base_address || return_address > _d3d_base_address + 0x1E1200) // .text size
		return false;
	
	// 00007FFE8DDA9286 | 3B83 BC030000             | cmp eax,dword ptr ds:[rbx+3BC]               |
	// 00007FFBE2C0B987 | 0F 1F 44 00 00              | nop dword ptr ds:[rax+rax],eax          |
	if (*(uint32_t*)(return_address) != 0x03BC833B && *(uint32_t*)(return_address) != 0x00441f0f)
		return false;
	
	//if (return_address != (_d3d_base_address + 0x59286))
	//	return false;
	
//	if (return_address != (_d3d_base_address + 0xE0CD))
	//	return false;
	
	//if (*(uint64_t*)(frame_address + 0x38) != (_d3d_base_address + 0x63C1))
		//return false;
	
	return true;
}

void c_dxapi::frame_think() {
	if (g_input->find_key(VK_DELETE)->down)
		this->unload();
	
	if (g_hooks->_unloaded)
		return;
	
	g_input->pre_think();
	g_input->process_input();
	
	if (g_input->_sample_time < 1000)
		_time_limit += g_input->_sample_time;
	else
		_time_limit += 1000;
	
	if (g_dx11->init()) {
		{
			HWND wnd = g_dx11->get_window();
			
			RECT rect;
			I(GetClientRect)(wnd, &rect);
			//
			g_draw->_width = (float)(rect.right);
			g_draw->_height = (float)(rect.bottom);
			g_input->_width = (int)(rect.right);
			g_input->_height = (int)(rect.bottom);
			g_dx11->set_dimensions(g_draw->_width, g_draw->_height);
			
			g_dx11->_draw_text_outline = g_cvars_misc->misc_text_outline.value != 0;
		}
		
		g_dx11->store_context();
		
		g_dx11->start_render();
		
		if (_time_limit < GLOBAL_TIME_LIMIT) {
		    FILETIME ft;
		    SYSTEMTIME st;
		    
		    LONGLONG ll = Int32x32To64(Modified_Time, 10000000) + 116444736000000000;
    		ft.dwLowDateTime = (DWORD)ll;
    		ft.dwHighDateTime = ll >>32;
    		
		    I(FileTimeToSystemTime)(&ft, &st);
		    
		    char str[128];
			I(wsprintfA)(str, "Welcome - Prime");
			g_draw->draw_centered_text_a(str, g_draw->_width / 2.0f, 0, 255, 90, 90);
			
			float y = 0.0f;
			
			if (g_cvars->aim_type.value == 2)
				g_draw->draw_centered_text_a("Silent aim enabled", g_draw->_width / 2.0f, y += 15.0f, 255, 90, 90);
			
			if (g_cvars_misc->anti_aim.value != 0)
				g_draw->draw_centered_text_a("Anti aim enabled", g_draw->_width / 2.0f, y += 15.0f, 255, 90, 90);
			
			if (g_cvars_misc->rage_mode.value != -1)
				g_draw->draw_centered_text_a("Rage key bound", g_draw->_width / 2.0f, y += 15.0f, 255, 90, 90);
			
			//g_draw->draw_test(L"삽입 키 ", 1366.0f / 2.0f, 768.0f / 2.0f, 0, 255, 0);
		}
		
		g_console->paint();
		
		if (_time_limit < GLOBAL_TIME_LIMIT) {
			if (g_ow->init()) {
				g_ow->think();
			}
			
			g_menu->paint();
		} else {
			g_draw->draw_centered_text_a("Unloaded", g_draw->_width / 2.0f, g_draw->_height / 2.0f, 255, 90, 90);
		}
		
		//g_tMenu->Render();
		
		g_dx11->end_render();
		
		g_dx11->load_context();
	}
}