#pragma once

class c_menu {
public:
	void tab_select(float x, float y, int count);
	void checkbox(char* name, float x, float y, class var_t* var);
	bool radio_start(char* name, float x, float y, int min, int max, class var_t* var);
	void radio_end();
	void radio_entry(char* name, int index);
	void slider(char* name, float x, float y, int min, int max, class var_t* var);
	void slider_minmax(char* name, float x, float y, int min, int max, class var_t* var_min, class var_t* var_max);
	void key_bind(char* name, float x, float y, class var_t* var);
	bool quadrant(float* x, float* y);
	
	void paint_aim_a();
	void paint_aim_b();
	void paint_aim_c();
	void paint_trig();
	void paint_history();
	void paint_hero();
	void paint_esp();
	void paint_misc();
	void paint();
	
	bool cursor_within(float x, float y, float w, float h);
	
	bool _init;
	
	int _r;
	int _g;
	int _b;
	
	float _base_x;
	float _base_y;
	float _root_x;
	float _root_y;
	
	bool _insert_state;
	
	bool _drawing;
	
	int _mouse_x;
	int _mouse_y;
	int _mouse_x_last;
	int _mouse_y_last;
	int _mouse_dt_x;
	int _mouse_dt_y;
	
	bool _mouse1_state;
	bool _mouse1_click;
	bool _mouse1_release;
	
	bool _mouse2_state;
	bool _mouse2_click;
	
	bool _left_state;
	bool _left_press;
	
	bool _right_state;
	bool _right_press;
	
	int _cur_tab;
	
	int _quad_index;
	
	int _key_pressed_changed;
	int _last_key_pressed;
	
	class var_t* _cur_var;
	
	class var_t* _radio_var;
	float _radio_x;
	float _radio_y;
};

extern c_menu* g_menu;