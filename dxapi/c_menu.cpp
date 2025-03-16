#include "base.h"

c_menu menu_instance;
c_menu* g_menu = &menu_instance;

#define SECTION_SIZE 175.0f
#define SECTION_SPACING 10.0f

#define TAB_WIDTH 80.0f
#define TAB_HEIGHT 15.0f

#define ITEM_SPACE 18.0f

#define CHECKBOX_SIZE 13.0f

#define RADIO_HEIGHT 13.0f
#define RADIO_WIDTH 90.0f
#define RADIO_TICK_SIZE 13.0f

#define SLIDER_WIDTH 65.0f

#define KEYBIND_WIDTH 52.0f
#define KEYBIND_HEIGHT 13.0f

#define TAB_COUNT 8.0f

void c_menu::tab_select(float x, float y, int count) {
	char str[16];
	
	float w = RADIO_TICK_SIZE * TAB_COUNT;
	
	g_draw->draw_rect(x, y, w, RADIO_HEIGHT, 64, 64, 64);
	
	for (int i = 0; i < count; ++i) {
		float tick_x = x + (RADIO_TICK_SIZE * i);
		bool within = this->cursor_within(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE);
		bool pressing = within && _mouse1_state;
		
		if (_cur_tab == i)
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 102, 102, 102);
		else if (pressing)
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 244, 244, 244);
		else if (within)
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 224, 224, 224);
		else
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 192, 192, 192);
		
		I(wsprintfA)(str, "%i", i + 1);
		g_draw->draw_text_a(str, tick_x + 4.0f, y - 2.0f, 255, 255, 255);
		
		if (this->cursor_within(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE)) {
			if (_mouse1_release)
				_cur_tab = i;
		}
	}
	
	for (int i = 0; i < count - 1; ++i) {
		float tick_x = x + (RADIO_TICK_SIZE * i);
		g_draw->draw_rect(tick_x + RADIO_TICK_SIZE, y, 1.0f, RADIO_TICK_SIZE, 51, 51, 51);
	}
	
	g_draw->draw_outline_rect(x, y, w, RADIO_HEIGHT, 1.0f, 51, 51, 51);
	
	_radio_x = x;
	_radio_y = y;
	
	return true;
}

void c_menu::checkbox(char* name, float x, float y, var_t* var) {
	float box_x = x + SECTION_SIZE - (CHECKBOX_SIZE + 10.0f);
	bool within = this->cursor_within(box_x, y, CHECKBOX_SIZE, CHECKBOX_SIZE);
	bool pressing = false;
	
	if (within && _cur_var == nullptr) {
		if (_mouse1_state)
			pressing = true;
		
		if (_mouse1_release) {
			if (var->value != 0)
				var->value = 0;
			else
				var->value = 1;
		}
	}
	
	if (var->value != 0) {
		if (within)
			g_draw->draw_rect(box_x, y, CHECKBOX_SIZE, CHECKBOX_SIZE, 244, 244, 244);
		else
			g_draw->draw_rect(box_x, y, CHECKBOX_SIZE, CHECKBOX_SIZE, 224, 224, 224);
	} else {
		if (pressing)
			g_draw->draw_rect(box_x, y, CHECKBOX_SIZE, CHECKBOX_SIZE, 128, 128, 128);
		else if (within)
			g_draw->draw_rect(box_x, y, CHECKBOX_SIZE, CHECKBOX_SIZE, 96, 96, 96);
		else
			g_draw->draw_rect(box_x, y, CHECKBOX_SIZE, CHECKBOX_SIZE, 64, 64, 64);
	}
	
	if (var->value != 0) {
		if (pressing)
			g_draw->draw_rect(box_x + 3.0f, y + 3.0f, 7.0f, 7.0f, 64, 64, 64);
		else if (within)
			g_draw->draw_rect(box_x + 3.0f, y + 3.0f, 7.0f, 7.0f, _r + 16, _g + 16, _b + 16);
		else
			g_draw->draw_rect(box_x + 3.0f, y + 3.0f, 7.0f, 7.0f, _r, _g, _b);
		
		g_draw->draw_outline_rect(box_x + 3.0f, y + 3.0f, 7.0f, 7.0f, 1.0f, 51, 51, 51);
	}
	
	g_draw->draw_outline_rect(box_x, y, CHECKBOX_SIZE, CHECKBOX_SIZE, 1.0f, 51, 51, 51);
	
	g_draw->draw_text_a(name, x, y, 255, 255, 255);
}

bool c_menu::radio_start(char* name, float x, float y, int min, int max, var_t* var) {
	float radio_x = x + SECTION_SIZE - RADIO_WIDTH - 10.0f;
	
	g_draw->draw_rect(radio_x, y, RADIO_WIDTH, 13.0f, 64, 64, 64);
	
	{
		float tick_x = radio_x;
		bool within = this->cursor_within(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE);
		bool pressing = within && _mouse1_state;
		
		if (var->value == min)
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 102, 102, 102);
		else if (pressing)
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 244, 244, 244);
		else if (within)
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 224, 224, 224);
		else
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 192, 192, 192);
		
		g_draw->draw_text_a("<", tick_x + 4.0f, y - 2.0f, 255, 255, 255);
		
		if (this->cursor_within(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE)) {
			if (_mouse1_release) {
				if (var->value > min)
					var->value -= 1;
			}
		}
	}
	
	{
		float tick_x = radio_x + RADIO_WIDTH - RADIO_TICK_SIZE;
		bool within = this->cursor_within(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE);
		bool pressing = within && _mouse1_state;
		
		if (var->value == max)
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 102, 102, 102);
		else if (pressing)
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 244, 244, 244);
		else if (within)
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 224, 224, 224);
		else
			g_draw->draw_rect(tick_x, y, RADIO_TICK_SIZE, RADIO_TICK_SIZE, 192, 192, 192);
		
		g_draw->draw_text_a(">", tick_x + 4.0f, y - 2.0f, 255, 255, 255);
		
		if (within) {
			if (_mouse1_release) {
				if (var->value < max)
					var->value += 1;
			}
		}
	}
	
	if (var->value < min)
		var->value = min;
	
	if (var->value > max)
		var->value = max;
	
	g_draw->draw_outline_rect(radio_x, y, RADIO_WIDTH, 13.0f, 1.0f, 51, 51, 51);
	
	g_draw->draw_text_a(name, x, y, 255, 255, 255);
	
	_radio_var = var;
	_radio_x = x;
	_radio_y = y;
	
	return true;
}

void c_menu::radio_end() {
	_radio_var = nullptr;
}

void c_menu::radio_entry(char* name, int index) {
	float x = _radio_x + SECTION_SIZE - (RADIO_WIDTH / 2.0f) - 10.0f;
	float y = _radio_y;
	
	if (_radio_var->value == index)
		g_draw->draw_centered_text_a(name, x, y, 255, 255, 255);
}

void c_menu::slider(char* name, float x, float y, int min, int max, var_t* var) {
	float slider_x = x + SECTION_SIZE - (SLIDER_WIDTH + 10.0f);
	float slider_y = y;
	bool within = this->cursor_within(slider_x - 10.0f, slider_y, SLIDER_WIDTH + 20.0f, 13.0f);
	bool pressing = within && _mouse1_state;
	
	float fill_x = slider_x + 3.0f;
	float fill_y = slider_y + 3.0f;
	
	g_draw->draw_rect(slider_x, slider_y, SLIDER_WIDTH, 13.0f, 224, 224, 224);
	g_draw->draw_outline_rect(slider_x, slider_y, SLIDER_WIDTH, 13.0f, 1.0f, 51, 51, 51);
	g_draw->draw_rect(fill_x, fill_y, (SLIDER_WIDTH - 6.0f), 7.0f, 64, 64, 64);
	
	{
		float frac = (float)(var->value - min) / (float)(max - min);
		float w = ((SLIDER_WIDTH - 6.0f) * frac);
		if (w < 0.0f)
			w = 0.0f;
		
		if (pressing)
			g_draw->draw_rect(fill_x, fill_y, w, 7.0f, _r + 32, _g + 32, _b + 32);
		else if (within)
			g_draw->draw_rect(fill_x, fill_y, w, 7.0f, _r + 16, _g + 16, _b + 16);
		else
			g_draw->draw_rect(fill_x, fill_y, w, 7.0f, _r, _g, _b);
		
		if (w > 0.0f)
			g_draw->draw_outline_rect(fill_x, fill_y, w, 7.0f, 1.0f, 51, 51, 51);
	}
	
	char str[64];
	I(wsprintfA)(str, "%i", var->value);
	g_draw->draw_centered_text_a(str, slider_x + (SLIDER_WIDTH / 2.0f), y, 255, 255, 255);
	
	g_draw->draw_text_a(name, x, y, 255, 255, 255);
	
	if (within) {
		if (_mouse1_state) {
			float dist = (float)(_mouse_x) - slider_x;
			var->value = min + (int)(((float)(max) * (dist / SLIDER_WIDTH)) + 0.5f);
		}
		
		if (_left_press)
			var->value -= 1;
		
		if (_right_press)
			var->value += 1;
	}
	
	if (var->value < min)
		var->value = min;
	
	if (var->value > max)
		var->value = max;
}

void c_menu::slider_minmax(char* name, float x, float y, int min, int max, var_t* var_min, var_t* var_max) {
	float slider_x = x + SECTION_SIZE - (SLIDER_WIDTH + 10.0f);
	float slider_y = y;
	bool within = this->cursor_within(slider_x - 10.0f, slider_y, SLIDER_WIDTH + 20.0f, 13.0f);
	
	float fill_x = slider_x + 3.0f;
	float fill_y = slider_y + 3.0f;
	
	g_draw->draw_rect(slider_x, slider_y, SLIDER_WIDTH, 13.0f, 224, 224, 224);
	g_draw->draw_outline_rect(slider_x, slider_y, SLIDER_WIDTH, 13.0f, 1.0f, 51, 51, 51);
	g_draw->draw_rect(fill_x, fill_y, (SLIDER_WIDTH - 6.0f), 7.0f, 64, 64, 64);
	
	{
		float frac = (float)(var_max->value - min) / (float)(max - min);
		float w = ((SLIDER_WIDTH - 6.0f) * frac);
		if (w < 0.0f)
			w = 0.0f;
		
		if (within && _mouse2_state)
			g_draw->draw_rect(fill_x, fill_y, w, 7.0f, _r + 32, _g + 32, _b + 32);
		else if (within)
			g_draw->draw_rect(fill_x, fill_y, w, 7.0f, _r + 16, _g + 16, _b + 16);
		else
			g_draw->draw_rect(fill_x, fill_y, w, 7.0f, _r, _g, _b);
		
		if (w > 0.0f)
			g_draw->draw_outline_rect(fill_x, fill_y, w, 7.0f, 1.0f, 51, 51, 51);
	}
	
	{
		float frac = (float)(var_min->value - min) / (float)(max - min);
		float w = ((SLIDER_WIDTH - 6.0f) * frac);
		if (w < 0.0f)
			w = 0.0f;
		
		if (within && _mouse1_state)
			g_draw->draw_rect(fill_x, fill_y, w, 7.0f, _r/2 + 32, _g/2 + 32, _b/2 + 32);
		else if (within)
			g_draw->draw_rect(fill_x, fill_y, w, 7.0f, _r/2 + 16, _g/2 + 16, _b/2 + 16);
		else
			g_draw->draw_rect(fill_x, fill_y, w, 7.0f, _r/2, _g/2, _b/2);
		
		if (w > 0.0f)
			g_draw->draw_outline_rect(fill_x, fill_y, w, 7.0f, 1.0f, 51, 51, 51);
	}
	
	char str[64];
	I(wsprintfA)(str, "%i/%i", var_min->value, var_max->value);
	g_draw->draw_centered_text_a(str, slider_x + (SLIDER_WIDTH / 2.0f), y, 255, 255, 255);
	
	g_draw->draw_text_a(name, x, y, 255, 255, 255);
	
	if (within) {
		float dist = (float)(_mouse_x) - slider_x;
		float value = min + (int)(((float)(max) * (dist / SLIDER_WIDTH)) + 0.5f);
		
		if (_mouse1_state) {
			var_min->value = value;
			
			if (var_min->value > var_max->value)
				var_min->value = var_max->value;
		} else if (_mouse2_state) {
			var_max->value = value;
			
			if (var_max->value < var_min->value)
				var_max->value = var_min->value;
		}
		
		//if (_left_press)
			//var->value -= 1;
		
		//if (_right_press)
			//var->value += 1;
	}
	
	if (var_min->value < min)
		var_min->value = min;
	
	if (var_min->value > max)
		var_min->value = max;
	
	if (var_max->value < min)
		var_max->value = min;
	
	if (var_max->value > max)
		var_max->value = max;
}

void c_menu::key_bind(char* name, float x, float y, var_t* var) {
	float box_x = x + SECTION_SIZE - (KEYBIND_WIDTH + 10.0f);
	
	g_draw->draw_rect(box_x, y, KEYBIND_WIDTH, KEYBIND_HEIGHT, 64, 64, 64);
	g_draw->draw_outline_rect(box_x, y, KEYBIND_WIDTH, KEYBIND_HEIGHT, 1.0f, 51, 51, 51);
	
	g_draw->draw_text_a(name, x, y, 255, 255, 255);
	
	bool avoid_this_frame = false;
	
	if (_cur_var == var) {
		if (_mouse1_click)
			var->value = 0;
		else if (_key_pressed_changed == 8) // VK_ESCAPE
			var->value = -1;
		else if (_key_pressed_changed != -1)
			var->value = _key_pressed_changed;
		
		if (_mouse1_click || _key_pressed_changed == 8 || _key_pressed_changed != -1) {
			_cur_var = nullptr;
			avoid_this_frame = true;
		}
	}
	
	if (this->cursor_within(box_x, y, KEYBIND_WIDTH, KEYBIND_HEIGHT) && !avoid_this_frame) {
		if (_mouse1_click)
			_cur_var = var;
	}
	
	if (_cur_var == var) {
		g_draw->draw_centered_text_a("*press*", box_x + (KEYBIND_WIDTH / 2.0f), y, 255, 255, 255);
	} else {
		int value = var->value;
		
		if (value >= 0 && value < MAX_CAPTURE_KEYS - 1) {
			if (s_vkey_names[value])
				g_draw->draw_centered_text_a(s_vkey_names[value], box_x + (KEYBIND_WIDTH / 2.0f), y, 255, 255, 255);
			else
				g_draw->draw_centered_text_a("null", box_x + (KEYBIND_WIDTH / 2.0f), y, 255, 255, 255);
		} else {
			g_draw->draw_centered_text_a("none", box_x + (KEYBIND_WIDTH / 2.0f), y, 255, 255, 255);
		}
	}
}

bool c_menu::quadrant(float* x, float* y) {
	if (_quad_index == 1)
		return false;
	
	float x2 = _base_x + SECTION_SPACING;
	float y2 = _base_y + SECTION_SPACING;
	
	int q = _quad_index;
	
	if (q & 1)
		x2 += SECTION_SIZE + SECTION_SPACING;
	
	if (q & 2)
		y2 += SECTION_SIZE + SECTION_SPACING;
	
	float x3 = x2;
	float y3 = y2;
	float w = SECTION_SIZE;
	float h = SECTION_SIZE;
	
	int test = _cur_tab;
	uint32_t mode = FNV1A_RT((char*)&(test), 4);
	
	for (int i = 0; i < 8; ++i) {
		int n = (mode & (1<<0) ? 8 : -16) * i;
		
		int r = 192 + n;
		int g = 192 + n;
		int b = 192 + n;
		
		if (mode & (1<<1)) {
			n = (mode & (1<<0) ? 8 : -16) * i;
			
			r = 64 - n;
			g = 64 - n;
			b = 64 - n;
		}
		
		g_draw->draw_rect(x3, y3, w, h, r, g, b);
		
		x3 += SECTION_SIZE / 16.0f;
		y3 += SECTION_SIZE / 16.0f;
		w -= SECTION_SIZE / 8.0f;
		h -= SECTION_SIZE / 8.0f;
	}
	
	g_draw->draw_outline_rect(x2, y2, SECTION_SIZE, SECTION_SIZE, 1.0f, 51, 51, 51);
	
	*x = x2 + 5.0f;
	*y = y2 + 5.0f;
	
	_quad_index += 1;
	
	return true;
}

void c_menu::paint_aim_a() {
	float x = 0.0f;
	float y = 0.0f;
	
	if (this->quadrant(&x, &y)) {
		int i = 0;
		
		//g_draw->draw_centered_text_a("Aim 1", x + (SECTION_SIZE / 2.0f), y + (15.0f * (float)(i++)), 255, 255, 255);
		
		if (this->radio_start("type", x, y + (ITEM_SPACE * (float)(i++)), 0, 2, &(g_cvars->aim_type))) {
			this->radio_entry("off", 0);
			this->radio_entry("engine", 1);
			this->radio_entry("silent", 2);
			
			this->radio_end();
		}
		
		this->key_bind("key", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->aim_key));
		
		if (this->radio_start("hitbox", x, y + (ITEM_SPACE * (float)(i++)), 0, 3, &(g_cvars->aim_hitbox))) {
			this->radio_entry("head", 0);
			this->radio_entry("body", 1);
			this->radio_entry("closest", 2);
			this->radio_entry("h+b", 3);
			
			this->radio_end();
		}
		
		if (g_cvars->aim_type.value == 2)
			this->slider("FOV degree", x, y + (ITEM_SPACE * (float)(i++)), 0, 90, &(g_cvars->aim_silent_fov));
		else
			this->slider("FOV %", x, y + (ITEM_SPACE * (float)(i++)), 0, 1000, &(g_cvars->aim_fov));
		
		this->slider("random point %", x, y + (ITEM_SPACE * (float)(i++)), 0, 100, &(g_cvars->aim_random));
	}
}

void c_menu::paint_aim_b() {
	float x = 0.0f;
	float y = 0.0f;
	
	if (this->quadrant(&x, &y)) {
		int i = 0;
		
		if (g_cvars->aim_smooth_scale_max.value > 0)
			this->slider_minmax("smooth min/max", x, y + (ITEM_SPACE * (float)(i++)), 0, 40, &(g_cvars->aim_smooth_min), &(g_cvars->aim_smooth_max));
		else
			this->slider("smooth", x, y + (ITEM_SPACE * (float)(i++)), 0, 40, &(g_cvars->aim_smooth_min));
		
		this->slider_minmax("scale min/max", x, y + (ITEM_SPACE * (float)(i++)), 0, 10, &(g_cvars->aim_smooth_scale_min), &(g_cvars->aim_smooth_scale_max));
		this->slider("correction", x, y + (ITEM_SPACE * (float)(i++)), 0, 100, &(g_cvars->aim_smooth_fix));
		
		this->slider("accel", x, y + (ITEM_SPACE * (float)(i++)), 0, 50, &(g_cvars->aim_smooth_accel));
		this->slider("accel cap", x, y + (ITEM_SPACE * (float)(i++)), 0, 300, &(g_cvars->aim_smooth_accel_cap));
		this->slider("sine %", x, y + (ITEM_SPACE * (float)(i++)), 0, 100, &(g_cvars->aim_smooth_sine));
	}
}

void c_menu::paint_aim_c() {
	float x = 0.0f;
	float y = 0.0f;
	
	if (this->quadrant(&x, &y)) {
		int i = 0;
		
		if (this->radio_start("auto fire", x, y + (ITEM_SPACE * (float)(i++)), 0, 2, &(g_cvars->aim_autofire))) {
			this->radio_entry("off", 0);
			this->radio_entry("always", 1);
			this->radio_entry("target", 2);
			
			this->radio_end();
		}
		
		this->checkbox("movement only", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->aim_movement));
		this->checkbox("single target", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->aim_single_target));
	//	this->checkbox(x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->aim_ignore_objects));
		this->slider("wait time %", x, y + (ITEM_SPACE * (float)(i++)), 0, 100, &(g_cvars->aim_wait_time));
		this->slider("post time", x, y + (ITEM_SPACE * (float)(i++)), 0, 500, &(g_cvars->aim_post_time));
		this->slider("autofire time", x, y + (ITEM_SPACE * (float)(i++)), 0, 500, &(g_cvars->aim_autofire_time));
		this->slider("range %", x, y + (ITEM_SPACE * (float)(i++)), 0, 100, &(g_cvars->aim_range));
		
		if (g_cvars->aim_type.value == 2)
			this->slider("detach smooth", x, y + (ITEM_SPACE * (float)(i++)), 0, 40, &(g_cvars->aim_silent_detach));
	}
}

void c_menu::paint_trig() {
	float x = 0.0f;
	float y = 0.0f;
	
	if (this->quadrant(&x, &y)) {
		int i = 0;
		
		if (this->radio_start("type", x, y + (ITEM_SPACE * (float)(i++)), 0, 1, &(g_cvars->trig_type))) {
			this->radio_entry("off", 0);
			this->radio_entry("primary", 1);
			/*this->radio_entry("alt", 2);
			this->radio_entry("ability1", 3);
			this->radio_entry("ability2", 4);
			*/
			this->radio_end();
		}
		
		this->key_bind("key", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->trig_key));
		this->slider("delay (ms)", x, y + (ITEM_SPACE * (float)(i++)), 0, 500, &(g_cvars->trig_delay));
		this->slider("post time (ms)", x, y + (ITEM_SPACE * (float)(i++)), 0, 500, &(g_cvars->trig_post_time));
		this->checkbox("head only", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->trig_head_only));
		
		switch (g_cfg->_hero) {
			case HERO_WIDOW:
			case HERO_HANZO: {
				this->slider("min accuracy %", x, y + (ITEM_SPACE * (float)(i++)), 0, 100, &(g_cvars->trig_min_accuracy));
				break;
			}
			case HERO_ROADHOG:
			case HERO_REAPER: {
				this->slider("min damage %", x, y + (ITEM_SPACE * (float)(i++)), 0, 200, &(g_cvars->trig_min_accuracy));
				break;
			}
		}
		
		//this->checkbox("use history", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->trig_history));
		//this->checkbox("history pred", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->trig_history_pred));
		
		this->checkbox("shoot objects", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->trig_objects));
	}
}

void c_menu::paint_history() {
	float x = 0.0f;
	float y = 0.0f;
	
	if (this->quadrant(&x, &y)) {
		int i = 0;
		/*
		this->checkbox("active", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->history_active));
		this->checkbox("ping pred", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->history_pred));
		
		if (this->radio_start("head mode", x, y + (ITEM_SPACE * (float)(i++)), 0, 2, &(g_cvars->history_head_mode))) {
			this->radio_entry("default", 0);
			this->radio_entry("priority", 1);
			this->radio_entry("disable", 2);
			
			this->radio_end();
		}
		
		this->slider("hitbox scale %", x, y + (ITEM_SPACE * (float)(i++)), 50, 100, &(g_cvars->history_hitbox_scale));
		this->slider("time limit", x, y + (ITEM_SPACE * (float)(i++)), 0, 260, &(g_cvars->history_time_limit));
		
		if (g_hero_data[g_cfg->_hero].attack_speed > 0.0f 
			|| g_hero_data[g_cfg->_hero].attack2_speed > 0.0f
			|| g_hero_data[g_cfg->_hero].ability1_speed > 0.0f
			|| g_hero_data[g_cfg->_hero].ability2_speed > 0.0f
			|| g_hero_data[g_cfg->_hero].ult_speed > 0.0f)
			this->checkbox("force lowest pred", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->history_lowest));
		
		this->checkbox("closest melee", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->history_melee));
		*/
	}
}

void c_menu::paint_hero() {
	float x = 0.0f;
	float y = 0.0f;
	
	if (this->quadrant(&x, &y)) {
		int i = 0;
		
		if (g_cfg->_hero == HERO_ANA) {
			if (this->radio_start("heal type", x, y + (ITEM_SPACE * (float)(i++)), 0, 2, &(g_cvars_misc->heal_type))) {
				this->radio_entry("off", 0);
				this->radio_entry("engine", 1);
				this->radio_entry("silent", 2);
				
				this->radio_end();
			}
			
			this->key_bind("heal key", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->heal_key));
			
			this->slider("FOV degree", x, y + (ITEM_SPACE * (float)(i++)), 0, 90, &(g_cvars_misc->heal_fov));
			
			this->slider("smooth factor", x, y + (ITEM_SPACE * (float)(i++)), 0, 40, &(g_cvars_misc->heal_smooth));
		}
		
		if (g_cfg->_hero == HERO_ZEN) {
			if (this->radio_start("alt type", x, y + (ITEM_SPACE * (float)(i++)), 0, 2, &(g_cvars_misc->zen_type))) {
				this->radio_entry("off", 0);
				this->radio_entry("key", 1);
				this->radio_entry("+attack2", 2);
				
				this->radio_end();
			}
			
			if (g_cvars_misc->zen_type.value == 1)
				this->key_bind("alt key", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->zen_key));
			
			this->checkbox("alt trig", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->zen_trig));
			
			this->slider("wait orbs", x, y + (ITEM_SPACE * (float)(i++)), 0, 5, &(g_cvars_misc->zen_wait_orbs));
			
			this->checkbox("anim cancel", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->zen_melee));
		}
		
		if (g_cfg->_hero == HERO_DVA
			|| g_cfg->_hero == HERO_SIGMA
			|| g_cfg->_hero == HERO_REINHARDT) {
			this->checkbox("cd block", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->hero_block));
			
			this->key_bind("cd key", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars->hero_block_key));
		}
	}
}

void c_menu::paint_esp() {
	float x = 0.0f;
	float y = 0.0f;
	
	if (this->quadrant(&x, &y)) {
		int i = 0;
		
		this->checkbox("ESP enabled", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->esp_active));
		this->checkbox("OSD enabled", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->esp_osd));
		//this->checkbox("FOV enabled", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->esp_draw_fov));
		
		
		this->slider("box ESP", x, y + (ITEM_SPACE * (float)(i++)), 0, 3, &(g_cvars_misc->esp_box));
		
		this->slider("hp bar ESP", x, y + (ITEM_SPACE * (float)(i++)), 0, 5, &(g_cvars_misc->esp_health_bar));
		this->slider("bar fraction", x, y + (ITEM_SPACE * (float)(i++)), 5, 5, &(g_cvars_misc->esp_health_bar_frac));
		
		this->slider("class ESP", x, y + (ITEM_SPACE * (float)(i++)), 0, 5, &(g_cvars_misc->esp_class));
		this->slider("health ESP", x, y + (ITEM_SPACE * (float)(i++)), 0, 5, &(g_cvars_misc->esp_health));
		this->slider("cd ESP", x, y + (ITEM_SPACE * (float)(i++)), 0, 5, &(g_cvars_misc->esp_cd));
		this->slider("ult ESP", x, y + (ITEM_SPACE * (float)(i++)), 0, 5, &(g_cvars_misc->esp_ult));
	}
}

void c_menu::paint_misc() {
	float x = 0.0f;
	float y = 0.0f;
	
	if (this->quadrant(&x, &y)) {
		int i = 0;
		
		this->key_bind("rage mode", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->rage_mode));
		
		if (this->radio_start("anti aim", x, y + (ITEM_SPACE * (float)(i++)), 0, 4, &(g_cvars_misc->anti_aim))) {
			this->radio_entry("off", 0);
			this->radio_entry("backward", 1);
			this->radio_entry("45jitter", 2);
			this->radio_entry("90jitter", 3);
			this->radio_entry("spin", 4);
			
			this->radio_end();
		}
		
		//this->checkbox(x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->heal_bot));
		this->checkbox("text outline", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->misc_text_outline));
		this->checkbox("auto melee", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->misc_auto_melee));
		this->checkbox("auto bhop", x, y + (ITEM_SPACE * (float)(i++)), &(g_cvars_misc->misc_auto_bhop));
		//this->slider("predict limit", x, y + (ITEM_SPACE * (float)(i++)), 0, 500, &(g_cvars_misc->misc_prediction_limit));
		//this->slider("debug ESP", x, y + (ITEM_SPACE * (float)(i++)), 0, 1, &(g_cvars_misc->esp_debug));
		//this->slider("test", x, y + (ITEM_SPACE * (float)(i++)), 0, 1000, &(g_cvars_misc->test));
		//this->slider(x, y + (ITEM_SPACE * (float)(i++)), 0, 1000, &(g_cvars_misc->test2));
	}
}

void c_menu::paint() {
	uint32_t color = g_hero_data[g_cfg->_hero].color;
	_r = (color >> 16) & 0xFF;
	_g = (color >> 8) & 0xFF;
	_b = color & 0xFF;
	
	_base_x = 1920.0f / 4.0f;
	_base_y = 100.0f;
	
	_mouse_x = g_input->_mouse_state.cursor_x;
	_mouse_y = g_input->_mouse_state.cursor_y;
	
	_mouse_dt_x = _mouse_x - _mouse_x_last;
	_mouse_dt_y = _mouse_y - _mouse_y_last;
	
	_mouse_x_last = _mouse_x;
	_mouse_y_last = _mouse_y;
	
	_quad_index = 0;
	
	if (g_input->_last_key_pressed != _last_key_pressed)
		_key_pressed_changed = g_input->_last_key_pressed;
	else
		_key_pressed_changed = -1;
	
	_last_key_pressed = g_input->_last_key_pressed;
	
	if (!_init)
		g_cfg->read_frame();
	
	_init = true;
	
	if (g_cfg->_hero != g_cfg->_last_hero)
		g_cfg->read_frame();
	
	g_cfg->_last_hero = g_cfg->_hero;
	
	if (g_input->find_key(VK_INSERT)->down) {
		if (!_insert_state) {
			_drawing = !_drawing;
			
			if (_drawing)
				g_cfg->read_frame();
			else
				g_cfg->write_frame();
		}
		
		_insert_state = true;
	} else {
		_insert_state = false;
	}
	
	_mouse1_click = false;
	_mouse1_release = false;
	if (g_input->find_key(VK_LBUTTON)->down) {
		if (!_mouse1_state)
			_mouse1_click = true;
		
		_mouse1_state = true;
	} else {
		if (_mouse1_state)
			_mouse1_release = true;
		
		_mouse1_state = false;
	}
	
	_mouse2_click = false;
	if (g_input->find_key(VK_RBUTTON)->down) {
		if (!_mouse2_state)
			_mouse2_click = true;
		
		_mouse2_state = true;
	} else {
		_mouse2_state = false;
	}
	
	_left_press = false;
	if (g_input->find_key(VK_LEFT)->down) {
		if (!_left_state) {
			_left_press = true;
		}
		
		_left_state = true;
	} else {
		_left_state = false;
	}
	
	_right_press = false;
	if (g_input->find_key(VK_RIGHT)->down) {
		if (!_right_state) {
			_right_press = true;
		}
		
		_right_state = true;
	} else {
		_right_state = false;
	}
	
	if (_drawing == false)
		return;
	
	float xmin = 1920.0f * 0.25f;
	float ymin = 1080.0f * 0.25f;
	float xmax = 1920.0f * 0.75f;
	float ymax = 1080.0f * 0.75f;
	
	const float background_size = (SECTION_SIZE) + (SECTION_SPACING * 2.0f);
	
	float x = _base_x;
	float y = _base_y;
	
	{
		float y2 = y - 20.0f;
		
		g_draw->draw_outline_rect(x, y2, background_size, background_size + 20.0f, 8.0f, _r, _g, _b);
		g_draw->draw_outline_rect(x, y2, background_size, background_size + 20.0f, 4.0f, 224, 224, 224);
		
		g_draw->draw_outline_rect(x, y2, background_size, background_size + 20.0f, 1.0f, 51, 51, 51);
		g_draw->draw_outline_rect(x + 8.0f, y2 + 8.0f, background_size - 16.0f, background_size + 20.0f - 16.0f, 1.0f, 51, 51, 51);
	}
	
	{
		float x2 = x + SECTION_SPACING + 3.0f;
		float y2 = (y - (20.f - 8.0f)) + 3.0f;
		
		g_draw->draw_text_a(g_hero_data[g_cfg->_hero].name, x2, y2, 255, 255, 255);
	}
	
	{
		float x2 = x + (SECTION_SPACING * 0.5f) + SECTION_SIZE - (RADIO_TICK_SIZE * TAB_COUNT);
		float y2 = y - (RADIO_HEIGHT * 0.5f);
		
		this->tab_select(x2, y2, 8);
	}
	
	switch (_cur_tab) {
		case 0: {
			this->paint_aim_a();
			break;
		}
		case 1: {
			this->paint_aim_b();
			break;
		}
		case 2: {
			this->paint_aim_c();
			break;
		}
		case 3: {
			this->paint_trig();
			break;
		}
		case 4: {
			this->paint_history();
			break;
		}
		case 5: {
			this->paint_hero();
			break;
		}
		case 6: {
			this->paint_esp();
			break;
		}
		case 7: {
			this->paint_misc();
			break;
		}
	}
}

bool c_menu::cursor_within(float x, float y, float w, float h) {
	float m0 = (float)(_mouse_x);
	float m1 = (float)(_mouse_y);
	
	return m0 > x && m1 > y && m0 < (x + w) && m1 < (y + h);
}