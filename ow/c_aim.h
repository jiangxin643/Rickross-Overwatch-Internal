#pragma once

enum {
	AIM_TYPE_NONE = 0,
	AIM_TYPE_PRIMARY = 1,
	AIM_TYPE_SECONDARY = 2,
	AIM_TYPE_ABILITY1 = 3,
	AIM_TYPE_ABILITY2 = 4,
	AIM_TYPE_ABILITY3 = 5,
	AIM_TYPE_HOOK = 6,
	AIM_TYPE_HEAL = 7,
	AIM_TYPE_ZEN = 9,
	AIM_TYPE_RAGE = 128,
};

struct aim_data {
	int type;
	bool silent;
	
	float fov_mult;
	int hitbox;
	float random;
	
	float smooth;
	float smooth_max;
	float scale_min;
	float scale_max;
	float smooth_fix;
	
	float smooth_accel;
	float smooth_accel_cap;
	float smooth_sine;
	
	int auto_fire;
	bool movement;
	bool ignore_objects;
	bool single_target;
	float wait_time;
	float post_time;
	float autofire_time;
	float range;
	
	int hitbox_level;
	
	bool distance_sorting;
};

class c_aim {
public:
	bool is_valid_aim_ent(ow_object* obj);
	bool is_valid_target(ow_object* obj);
	int get_hitbox_level(ow_object* obj);
	bool get_heal_pos(ow_object* obj, Vector* pos_out, float* diff_out);
	bool get_aim_pos(ow_object* obj, Vector* pos_out, float* diff_out);
	bool get_target();
	bool get_healing_target();
	
	void think();
	
	bool update();
	
	bool pre_aim();
	void apply_smooth(Vector forward, bool silent);
	bool set_aim();
	bool set_aim_silent();
	void autofire_think();
	
	void update_silent_aim();
	void force_tick();
	
	void get_smooth_scale(Vector viewforward, Vector dir, float* smooth);
	void apply_smooth_friction();
	void cap_smooth_speed();
	
	aim_data _data;
	int _last_type;
	
	Vector _smooth_base;
	
	ow_object* _aim_obj;
	Vector _aim_pos;
	
	ow_object* _last_obj;
	
	bool _aiming;
	bool _arrow_drawn;
	bool _arrow_drawn_silent;
	bool _on_heal_target;
	bool _zen_charging;
	
	float _aim_time;
	
	Vector _silent_delta;
	
	float _key_time;
	float _autofire_time;
	
	int _backward_tick;
	float _last_aimed;
	
	Vector _aim_dir;
	
	uint32_t _xseed;
	uint32_t _yseed;
	uint32_t _zseed;
};

extern c_aim* g_aim;