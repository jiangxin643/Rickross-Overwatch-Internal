#include "base.h"

static cvars_t s_cvars = {
	aim_type: {crc: HASH("aim_type"), value: 0},
	aim_key: {crc: HASH("aim_key"), value: -1},
	
	aim_hitbox: {crc: HASH("aim_hitbox"), value: 2},
	aim_fov: {crc: HASH("aim_fov"), value: 100},
	aim_silent_fov: {crc: HASH("aim_silent_fov"), value: 1},
	aim_random: {crc: HASH("aim_random"), value: 0},
	
	aim_smooth_min: {crc: HASH("aim_smooth_min"), value: 10},
	aim_smooth_max: {crc: HASH("aim_smooth_max"), value: 0},
	aim_smooth_scale_min: {crc: HASH("aim_smooth_scale_min"), value: 0},
	aim_smooth_scale_max: { crc: HASH("aim_smooth_scale_max"), value: 0},
	aim_smooth_fix: {crc: HASH("aim_smooth_fix"), value: 100},
	aim_smooth_accel: {crc: HASH("aim_smooth_accel"), value: 0},
	aim_smooth_accel_cap: {crc: HASH("aim_smooth_accel_cap"), value: 0},
	aim_smooth_sine: {crc: HASH("aim_smooth_sine"), value: 0},
	
	aim_autofire: {crc: HASH("aim_autofire"), value: 0},
	aim_movement: {crc: HASH("aim_movement"), value: 0},
	aim_single_target: {crc: HASH("aim_single_target"), value: 0},
	aim_ignore_objects: {crc: HASH("aim_ignore_objects"), value: 0},
	aim_wait_time: {crc: HASH("aim_wait_time"), value: 0},
	aim_post_time: {crc: HASH("aim_post_time"), value: 0},
	aim_autofire_time: {crc: HASH("aim_autofire_time"), value: 0},
	aim_range: {crc: HASH("aim_range"), value: 0},
	
	aim_silent_detach: {crc: HASH("aim_silent_detach"), value: 5},
	
	trig_type: {crc: HASH("trig_type"), value: 0},
	trig_key: {crc: HASH("trig_key"), value: -1},
	trig_delay: {crc: HASH("trig_delay"), value: 0},
	trig_post_time: {crc: HASH("trig_post_time"), value: 0},
	trig_head_only: {crc: HASH("trig_head_only"), value: 0},
	trig_min_accuracy: {crc: HASH("trig_min_accuracy"), value: 0},
	trig_history: {crc: HASH("trig_history"), value: 0},
	trig_history_pred: {crc: HASH("trig_history_pred"), value: 0},
	trig_objects: {crc: HASH("trig_objects"), value: 0},
	
	history_active: {crc: HASH("history_active"), value: 0},
	history_pred: {crc: HASH("history_pred"), value: 0},
	history_head_mode: {crc: HASH("history_head_mode"), value: 0},
	history_hitbox_scale: {crc: HASH("history_hitbox_scale"), value: 100},
	history_time_limit: {crc: HASH("history_time_limit"), value: 0},
	history_lowest: {crc: HASH("history_lowest"), value: 1},
	history_melee: {crc: HASH("history_melee"), value: 1},
	
	hero_block: {crc: HASH("hero_block"), value: 0},
	hero_block_key: {crc: HASH("hero_block_key"), value: -1},
};

static cvars_misc_t s_cvars_misc = {
	heal_type: {crc: HASH("heal_type"), value: 0},
	heal_key: {crc: HASH("heal_key"), value: 0},
	
	heal_fov: {crc: HASH("heal_fov"), value: 1},
	
	heal_smooth: {crc: HASH("heal_smooth"), value: 3},
	
	zen_type: {crc: HASH("zen_type"), value: 0},
	zen_key: {crc: HASH("zen_key"), value: 0},
	zen_trig: {crc: HASH("zen_trig"), value: 0},
	zen_wait_orbs: {crc: HASH("zen_wait_orbs"), value: 0},
	zen_melee: {crc: HASH("zen_melee"), value: 0},
	
	esp_active: {crc: HASH("esp_active"), value: 1},
	esp_osd: {crc: HASH("esp_osd"), value: 1},
	esp_draw_fov: {crc: HASH("esp_draw_fov"), value: 1},
	esp_box: {crc: HASH("esp_box"), value: 1},
	esp_health_bar: {crc: HASH("esp_health_bar"), value: 1},
	esp_health_bar_frac: {crc: HASH("esp_health_bar_frac"), value: 10},
	esp_class: {crc: HASH("esp_class"), value: 5},
	esp_health: {crc: HASH("esp_health"), value: 5},
	esp_cd: {crc: HASH("esp_cd"), value: 5},
	esp_ult: {crc: HASH("esp_ult"), value: 5},
	esp_debug: {crc: HASH("esp_debug"), value: 0},
	
	misc_text_outline: {crc: HASH("misc_text_outline"), value: 0},
	misc_auto_melee: {crc: HASH("misc_auto_melee"), value: 0},
	misc_auto_bhop: {crc: HASH("misc_auto_bhop"), value: 0},
	misc_prediction_limit: {crc: HASH("misc_prediction_limit"), value: 0},
	
	rage_mode: {crc: HASH("rage_mode"), value: -1},
	anti_aim: {crc: HASH("anti_aim"), value: 0},
	heal_bot: {crc: HASH("heal_bot"), value: 0},
	
	//misc_smooth_view: {name: "smooth view (clientside)", crc: HASH("misc_smooth_view"), value: 1, min: 0, max: 1, offset: 1},
	
	test: {crc: HASH("test"), value: 1},
	test2: {crc: HASH("test2"), value: 1},
	test3: {crc: HASH("test3"), value: 1},
};

cvars_t* g_cvars = &s_cvars;
cvars_misc_t* g_cvars_misc = &s_cvars_misc;

c_cfg cfg_instance;
c_cfg* g_cfg = &cfg_instance;

void c_cfg::read_var(var_t* var, uint32_t crc) {
	for (int i = 0; i < MAX_SETS; ++i) {	
		if (_sets[i].crc == crc) {
			var->value = _sets[i].value;
			return;
		}
	}
}

void c_cfg::write_var(var_t* var, uint32_t crc) {
	int first_null = -1;
	
	for (int i = 0; i < MAX_SETS; ++i) {
		if (_sets[i].crc == 0) {
			first_null = i;
			continue;
		}
		
		if (_sets[i].crc == crc) {
			_sets[i].value = var->value;
			return;
		}
	}
	
	if (first_null != -1) {
		_sets[first_null].crc = crc;
		_sets[first_null].value = var->value;
	}
}

void c_cfg::read_frame() {
	g_console->print("g_config loaded", 4.0f);
	this->read();
	
	for (int i = 0; i < CVARS_COUNT; ++i) {
		var_t* var = &(((var_t*)(g_cvars))[i]);
		uint32_t crc = var->crc ^ FNV1A_RT((char*)(&_hero), 4);
		this->read_var(var, crc);
	}
	
	for (int i = 0; i < CVARS_MISC_COUNT; ++i) {
		var_t* var = &(((var_t*)(g_cvars_misc))[i]);
		this->read_var(var, var->crc);
	}
}

void c_cfg::write_frame() {
	g_console->print("g_config initialized", 4.0f);
	for (int i = 0; i < CVARS_COUNT; ++i) {
		var_t* var = &(((var_t*)(g_cvars))[i]);
		uint32_t crc = var->crc ^ FNV1A_RT((char*)(&_hero), 4);
		this->write_var(var, crc);
	}
	
	for (int i = 0; i < CVARS_MISC_COUNT; ++i) {
		var_t* var = &(((var_t*)(g_cvars_misc))[i]);
		this->write_var(var, var->crc);
	}
	
	this->write();
}

void c_cfg::read() {
	char temp[MAX_PATH];
	I(GetTempPathA)(MAX_PATH, temp);
	
	char file_name[MAX_PATH+16];
	I(wsprintfA)(file_name, "%s\\ow.dump", temp);
	
	HANDLE bin = I(CreateFileA)(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (bin == INVALID_HANDLE_VALUE)
		return;
	
	uint32_t bytes_read = 0;
	I(ReadFile)(bin, &_sets, sizeof(set_t) * MAX_SETS, (PDWORD)(&bytes_read), NULL);
	
	I(CloseHandle)(bin);
}

void c_cfg::write() {
	char temp[MAX_PATH];
	I(GetTempPathA)(MAX_PATH, temp);
	
	char file_name[MAX_PATH+16];
	I(wsprintfA)(file_name, "%s\\ow.dump", temp);
	
	HANDLE bin = I(CreateFileA)(file_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (bin == INVALID_HANDLE_VALUE)
		return;
	
	uint32_t bytes_written = 0;
	I(WriteFile)(bin, &_sets, sizeof(set_t) * MAX_SETS, (PDWORD)(&bytes_written), NULL);
	
	I(CloseHandle)(bin);
}