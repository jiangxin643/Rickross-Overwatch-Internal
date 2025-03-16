#include "../main_win.h"
#include "c_input.h"

c_input input_instance;
c_input* g_input = &input_instance;

#define MOUSE_FRAMETIME 16

void c_input::pre_think() {
	_cmd_index += 1;
	
	user_cmd* cmd = &_cmds[_cmd_index % MAX_USER_CMDS];
	memset(cmd, 0, sizeof(user_cmd));
	cmd->index = _cmd_index;
	
	// set these manually
	_center_x = _width / 2;
	_center_y = _height / 2;
	
	LARGE_INTEGER test;
	I(QueryPerformanceCounter)(&test);
	
	LARGE_INTEGER freq;
	I(QueryPerformanceFrequency)(&freq);
	
	_sample_time = (uint32_t)(((test.QuadPart - _process_time.QuadPart)) / (freq.QuadPart / 1000));
	
	I(QueryPerformanceCounter)(&_process_time);
}

void c_input::post_think() {
	user_cmd* cmd = &_cmds[_cmd_index % MAX_USER_CMDS];
	user_cmd* prev_cmd = nullptr;
	if (_cmd_index > 0)
		prev_cmd = &_cmds[(_cmd_index - 1) % MAX_USER_CMDS];
	
	if (prev_cmd == nullptr)
		return;
	
	INPUT inputs[MAX_INPUTS+1];
	int input_count = 0;
	
	memset(inputs, 0, sizeof(inputs));
	
	if (cmd->mouse_dx != 0 || cmd->mouse_dy != 0) {
		if (true) {
			uint32_t divisor = 1;
			if (_sample_time > 0 && _sample_time < MOUSE_FRAMETIME)
				divisor = MOUSE_FRAMETIME / _sample_time;
			
			INPUT* ip = &inputs[input_count++];
			ip->type = INPUT_MOUSE;
			ip->mi.dwFlags = MOUSEEVENTF_MOVE;
			
			ip->mi.dx = (LONG)(cmd->mouse_dx) / (LONG)(divisor);
			ip->mi.dy = (LONG)(cmd->mouse_dy) / (LONG)(divisor);
		} else {
			//base->USER32_SetCursorPos(_mouse_state.input_x + cmd->mouse_dx, _mouse_state.input_y + cmd->mouse_dy);
		}
	}
	
	if (cmd->mouse_keys != prev_cmd->mouse_keys) {
		for (int i = 0; i < MAX_MOUSE_KEYS; ++i) {
			int mode = 0;
			if ((cmd->mouse_keys & mouse_flags[i]) && !(prev_cmd->mouse_keys & mouse_flags[i]))
				mode = 1;
			else if (!(cmd->mouse_keys & mouse_flags[i]) && (prev_cmd->mouse_keys & mouse_flags[i]))
				mode = 2;
			
			if (mode != 0) {
				INPUT* ip = &inputs[input_count++];
				ip->type = INPUT_MOUSE;
				
				if (mode == 1)
					ip->mi.dwFlags = mouse_down[i];
				else
					ip->mi.dwFlags = mouse_up[i];
			}
		}
	}
	
	for (int i = 0; i < MAX_INPUTS; ++i) {
		int mode = 0;
		int key = 0;
		
		if (cmd->keys[i] != 0 && prev_cmd->keys[i] == 0) {
			mode = 1;
			key = cmd->keys[i];
		} else if (cmd->keys[i] == 0 && prev_cmd->keys[i] != 0) {
			mode = 2;
			key = prev_cmd->keys[i];
		}
		
		int scan_key = 0;
		if (key > 0 && key < MAX_SCAN_KEYS) {
			if (s_vk_to_scan[key] != 0)
				scan_key = s_vk_to_scan[key];
		}
		
		if (mode != 0) {
			INPUT* ip = &inputs[input_count++];
			ip->type = INPUT_KEYBOARD;
			
			if (mode == 2)
				ip->ki.dwFlags |= KEYEVENTF_KEYUP;
			
			if (scan_key != 0) {
				ip->ki.dwFlags |= KEYEVENTF_SCANCODE;
				ip->ki.wScan = scan_key;
			} else {
				ip->ki.wVk = key;
			}
		}
	}
	
	if (input_count != 0)
		I(SendInput)(input_count, inputs, sizeof(INPUT));
}

user_cmd* c_input::get_user_cmd() {
	return &_cmds[_cmd_index % MAX_USER_CMDS];
}

void c_input::process_input() {
	{
		POINT cur_point;
		I(GetCursorPos)(&cur_point);
		
		int x = (int)(cur_point.x);
		int y = (int)(cur_point.y);
		
		int dx = x - _mouse_state.last_x;
		int dy = y - _mouse_state.last_y;
		
		if (x == _mouse_state.last_x && y == _mouse_state.last_y) {
			if (x > _center_x + 20 || x < _center_x - 20 || y > _center_y + 20 || y < _center_y - 20) {
				this->handle_mouse_move(x, y);
			}
		} else {
			int x2 = _mouse_state.cursor_x + dx;
			int y2 = _mouse_state.cursor_y + dy;
			this->handle_mouse_move(x2, y2);
		}
		
		_mouse_state.last_x = x;
		_mouse_state.last_y = y;
	}
	
	{
		for (int i = 0; i < MAX_CAPTURE_KEYS; ++i) {
			uint8_t key = s_vkey_capture[i];
			SHORT vk_state = I(GetAsyncKeyState)((int)key);
			
			key_state* state = &_key_states[i];
			
			if (state->inactive_time != -1)
				state->inactive_time += (int)_sample_time;
			
			if (vk_state != state->last_vk_state) {
				if (vk_state != 0)
					this->handle_key_press(i, 0);
				else
					this->handle_key_release(i, 0);
			}
			
			state->last_vk_state = (short)vk_state;
		}
	}
}

void c_input::handle_mouse_move(int x, int y) {
	_mouse_state.cursor_x = x;
	_mouse_state.cursor_y = y;
	
	if (_mouse_state.cursor_x < 0)
		_mouse_state.cursor_x = 0;
	
	if (_mouse_state.cursor_y < 0)
		_mouse_state.cursor_y = 0;
	
	// fix this
	if (_mouse_state.cursor_x > _width)
		_mouse_state.cursor_x = _width;
	
	if (_mouse_state.cursor_y > _height)
		_mouse_state.cursor_y = _height;
}

void c_input::handle_key_press(int key, int time) {
	key_state* state = &_key_states[key];
	if (state->down)
		return;
	
	state->down = true;
	state->inactive_time = -1;
	
	if (state->toggle)
		state->toggle = false;
	else
		state->toggle = true;
	
	_last_key_pressed = key;
}

void c_input::handle_key_release(int key, int time) {
	key_state* state = &_key_states[key];
	if (!state->down)
		return;
	
	state->down = false;
	state->inactive_time = 0;
	
	if (state->toggle)
		state->toggle = false;
	else
		state->toggle = true;
}

key_state* c_input::find_key(int vkey) {
	for (int i = 0; i < MAX_KEY_STATES; ++i) {
		uint8_t key = s_vkey_capture[i];
		if (key == vkey)
			return &_key_states[i];
	}
	
	BREAK();
	
	return nullptr;
}