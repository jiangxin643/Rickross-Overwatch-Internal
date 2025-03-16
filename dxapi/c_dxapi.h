#pragma once

class c_dxapi {
public:
	bool init();
	void unload();
	bool is_present_address(uintptr_t frame_address, uintptr_t return_address);
	void frame_think();
	
	bool _init;
	bool _failed;
	
	bool _unload;
	bool _attempt_unload;
	
	uintptr_t _d3d_base_address;
	
	int _time_limit;
};

extern c_dxapi* g_dxapi;

//#define GLOBAL_TIME_LIMIT 7200*1000
#define GLOBAL_TIME_LIMIT 2147483647