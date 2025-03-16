#pragma once

struct var_t {
	uint32_t crc;
	int value;
};

struct cvars_t {
	var_t aim_type;
	var_t aim_key;
	
	var_t aim_hitbox;
	var_t aim_fov;
	var_t aim_silent_fov;
	var_t aim_random;
	
	var_t aim_smooth_min;
	var_t aim_smooth_max;
	var_t aim_smooth_scale_min;
	var_t aim_smooth_scale_max;
	var_t aim_smooth_fix;
	var_t aim_smooth_accel;
	var_t aim_smooth_accel_cap;
	var_t aim_smooth_sine;
	
	var_t aim_autofire;
	var_t aim_movement;
	var_t aim_single_target;
	var_t aim_ignore_objects;
	var_t aim_wait_time;
	var_t aim_post_time;
	var_t aim_autofire_time;
	var_t aim_range;
	var_t aim_silent_detach;
	
	var_t trig_type;
	var_t trig_key;
	var_t trig_delay;
	var_t trig_post_time;
	var_t trig_head_only;
	var_t trig_min_accuracy;
	var_t trig_history;
	var_t trig_history_pred;
	var_t trig_objects;
	
	var_t history_active;
	var_t history_pred;
	var_t history_head_mode;
	var_t history_hitbox_scale;
	var_t history_time_limit;
	var_t history_lowest;
	var_t history_melee;
	
	var_t hero_block;
	var_t hero_block_key;
};

struct cvars_misc_t {
	var_t heal_type;
	var_t heal_key;
	
	var_t heal_fov;
	
	var_t heal_smooth;
	
	var_t zen_type;
	var_t zen_key;
	var_t zen_trig;
	var_t zen_wait_orbs;
	var_t zen_melee;
	
	var_t esp_active;
	var_t esp_osd;
	var_t esp_draw_fov;
	var_t esp_box;
	var_t esp_health_bar;
	var_t esp_health_bar_frac;
	var_t esp_class;
	var_t esp_health;
	var_t esp_cd;
	var_t esp_ult;
	var_t esp_debug;
	
	var_t misc_text_outline;
	var_t misc_auto_melee;
	var_t misc_auto_bhop;
	var_t misc_prediction_limit;
	
	var_t rage_mode;
	var_t anti_aim;
	var_t heal_bot;
	
	var_t test;
	var_t test2;
	var_t test3;
};

#define CVARS_COUNT (sizeof(cvars_t) / sizeof(var_t))
#define CVARS_MISC_COUNT (sizeof(cvars_misc_t) / sizeof(var_t))
#define CVARS_TOTAL_COUNT ((CVARS_COUNT * 33) + CVARS_MISC_COUNT)
// 33 = HERO_MAX

#define MAX_SETS CVARS_TOTAL_COUNT

struct set_t {
	uint32_t crc;
	int value;
};

class c_cfg {
public:
	void read_var(var_t* var, uint32_t crc);
	void write_var(var_t* var, uint32_t crc);
	void read_frame();
	void write_frame();
	void read();
	void write();
	
	set_t _sets[MAX_SETS];
	int _hero;
	int _last_hero;
};

extern c_cfg* g_cfg;

extern cvars_t* g_cvars;
extern cvars_misc_t* g_cvars_misc;