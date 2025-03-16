#include "base.h"

c_hero hero_instance;
c_hero* g_hero = &hero_instance;

void c_hero::test() {
	float best_len = 1.e+9f;
	ow_object* best_obj = nullptr;
	
	_beasd = false;
	
	for (int i = 0; i < g_state->_object_count; ++i) {
		ow_object* obj = &(g_state->_objects[i]);
		if (obj->type != OBJECT_PLAYER)
			continue;
		
		if (!g_state->is_alive(obj))
			continue;
		
		if (g_state->get_team(local->object.filter_bits) != g_state->get_team(obj->filter_bits))
			continue;
		
		//if (obj->hero == HERO_ANA || obj->hero == HERO_LUCIO)
	//		continue;
		
		float len = (obj->position - local->vieworigin).length();
		if (obj->total_max_hp > 300.0f)
			len *= 0.5f;
		
		if (len < best_len) {
			best_len = len;
			best_obj = obj;
		}
	}
	
	if (best_obj != nullptr) {
		_beasd = true;
		_beasd_dir = (best_obj->position - local->vieworigin).normal();
		_beasd_dist = (best_obj->position - local->vieworigin).length();
	}
}

void c_hero::think() {
	this->test();
	
	g_cfg->_hero = local->object.hero;
	
	int hero = local->object.hero;
	switch (hero) {
		case HERO_SIGMA:
		case HERO_DVA:
		case HERO_REINHARDT: {
			g_tank->think();
			break;
		}
		case HERO_MCCREE: {
			g_defense->cree_think();
			break;
		}
		case HERO_SOLDIER: {
			g_offense->soldier_think();
			break;
		}
		case HERO_GENJI: {
			g_genji->think();
			break;
		}
	}
	
	if (g_cvars_misc->misc_auto_bhop.value != 0) {
		bool on_ground = true;
		
		switch (hero) {
			case HERO_GENJI:
			case HERO_HANZO: {
				break;
			}
			default: {
				on_ground = local->object.on_ground;
				break;
			}
		}
		
		if (on_ground == false)
			g_ow->_cmd.buttons &= ~IN_JUMP;
	}
	
	if (hero == HERO_MERCY) {
		Vector fwd = local->viewforward;
		
		ow_object* best_obj = nullptr;
		float best_diff = -1.0f;
		
		for (int i = 0; i < g_state->_object_count; ++i) {
			ow_object* obj = &(g_state->_objects[i]);
			if (obj->type == OBJECT_INVALID)
				continue;
			
			if (!g_state->is_alive(obj))
				continue;
			
			if (g_state->get_team(local->object.filter_bits) != g_state->get_team(obj->filter_bits))
				continue;
			
	 		Vector org = obj->model_center;
			Vector dt = (org - local->vieworigin);
			Vector dt_normal = dt.normal();
			
			float dist = dt.length_sqr();
			float diff = fwd.dot(dt_normal);
			
			if (diff < 0.707f)
				continue; // ignore any > 45 degrees behind
			
			if (dist > (15.0f * 15.0f))
				continue;
			
			if (diff > best_diff) {
				best_diff = diff;
				
				best_obj = obj;
			}
		}
		
		if (best_obj != nullptr) {
			if (g_input->find_key(VK_XBUTTON2)->down) {
				if (best_obj->health < best_obj->max_health) {
					g_ow->_cmd.buttons |= IN_ATTACK;
					g_ow->_cmd.buttons &= ~IN_ATTACK2;
				} else {
					g_ow->_cmd.buttons |= IN_ATTACK2;
					g_ow->_cmd.buttons &= ~IN_ATTACK;
				}
			}
		}
	}
}

int c_hero::get_aim_key(int value, int post_time) {
	if (value < 0 || value >= MAX_CAPTURE_KEYS - 1)
		return -1;
	
	key_state* state = (&(g_input->_key_states[value]));
	
	if (state->down)
		return 0;
	
	if (post_time > 0 && state->inactive_time != -1) {
		if (state->inactive_time < post_time)
			return 0;
	}
	
	return -1;
}

void c_hero::aim_setup() {
	aim_data* aim = &(g_aim->_data);
	memset(aim, 0, sizeof(aim_data));
	
	{
		bool pressing = false;
		
		if (this->get_aim_key(g_cvars->aim_key.value, g_cvars->aim_post_time.value) != -1)
			pressing = true;
		
		if (g_cvars->aim_key.value == -1)
			pressing = true;
		
		if (g_cvars->aim_type.value == 0)
			pressing = false;
		
		if (pressing)
			aim->type = AIM_TYPE_PRIMARY;
	}
	
	{
		aim->silent =  g_cvars->aim_type.value == 2;
		
		aim->hitbox = g_cvars->aim_hitbox.value;
		
		if (aim->silent)
			aim->fov_mult = (float)(g_cvars->aim_silent_fov.value);
		else
			aim->fov_mult = (float)(g_cvars->aim_fov.value) * 0.01f;
		
		aim->random = (float)(g_cvars->aim_random.value) * 0.01f;
		
		aim->smooth = (float)(g_cvars->aim_smooth_min.value);
		aim->smooth_max = (float)(g_cvars->aim_smooth_max.value);
		aim->scale_min = (float)(g_cvars->aim_smooth_scale_min.value);
		aim->scale_max = (float)(g_cvars->aim_smooth_scale_max.value);
		aim->smooth_fix = (float)(g_cvars->aim_smooth_fix.value) * 0.01f;
		aim->smooth_accel = (float)(g_cvars->aim_smooth_accel.value);
		aim->smooth_accel_cap = (float)(g_cvars->aim_smooth_accel_cap.value) * 0.01f;
		aim->smooth_sine = (float)(g_cvars->aim_smooth_sine.value) * 0.01f;
	}
	
	{
		aim->auto_fire = g_cvars->aim_autofire.value;
		aim->movement = g_cvars->aim_movement.value != 0 ? true : false;
		aim->single_target = g_cvars->aim_single_target.value != 0 ? true : false;
		aim->ignore_objects = g_cvars->aim_ignore_objects.value != 0 ? true : false;
		aim->wait_time = (float)(g_cvars->aim_wait_time.value) / 100.0f;
		aim->post_time = (float)(g_cvars->aim_post_time.value) / 1000.0f;
		aim->autofire_time = (float)(g_cvars->aim_autofire_time.value) / 1000.0f;
		aim->range = (float)(g_cvars->aim_range.value) / 100.0f;
	}
}

void c_hero::post_aim_setup() {
	aim_data* aim = &(g_aim->_data);
	pseudo_cmd* ucmd = (pseudo_cmd*)&(g_ow->_cmd);
	
	int hero = local->object.hero;
	
	float attack_recovery = g_hero_data[hero].attack_recovery;
	float attack_range = g_hero_data[hero].attack_range;
	float attack_falloff_end = g_hero_data[hero].attack_falloff_end;
	
	float attack2_recovery = g_hero_data[hero].attack2_recovery;
	
	if (hero == HERO_GENJI) {
		float delta = g_state->_curtime - g_offense->_genji_next_secondary;
		if (delta < local->genji_primary)
			attack_recovery = g_hero_data[hero].attack2_recovery;
	}
	
	if (hero == HERO_ZARYA)
		attack_recovery = g_hero_data[hero].attack2_recovery;
	
	if (hero == HERO_WIDOW)
		attack_recovery = 1.0f;
	else if (hero == HERO_ASHE && (ucmd->buttons & IN_ATTACK2) == 0)
		attack_recovery = attack2_recovery;
	else if (hero == HERO_HANZO && local->ability_limit == 0x5a1)
		attack_recovery = 0.25f;
	
	aim->wait_time = attack_recovery * aim->wait_time;
	
	if (attack_range != 0.0f)
		aim->range = attack_range * std::max(aim->range, 1.0f);
	else if (aim->range != 0.0f)
		aim->range = std::max(attack_falloff_end, 1.0f) * aim->range; 
	else
		aim->range = 1.e+5f;
	
	if (hero == HERO_ZARYA)
		aim->hitbox = -1;
}

void c_hero::aim_think() {
	aim_data* aim = &(g_aim->_data);
	
	pseudo_cmd* ucmd = (pseudo_cmd*)&(g_ow->_cmd);
	int hero = local->object.hero;
	
	this->aim_setup();
	
	if (this->get_aim_key(g_cvars_misc->rage_mode.value, 0) != -1) {
		memset(aim, 0, sizeof(aim_data));
		
		aim->type = AIM_TYPE_RAGE;
		aim->silent = false;
		
		aim->auto_fire = 2;
		aim->range = 200.0f;
		
		aim->distance_sorting = false;
	}
	
	this->post_aim_setup();
	
	switch (hero) {
		case HERO_ZEN: {
			bool pressing = false;
			
			if (g_cvars_misc->zen_type.value == 1) {
				if (this->get_aim_key(g_cvars_misc->zen_key.value, 0) != -1)
					pressing = true;
				
				if (g_cvars_misc->zen_key.value == -1)
					pressing = true;
				
				if (pressing)
					aim->auto_fire = 1;
			} else if (g_cvars_misc->zen_type.value == 2) {
				if (ucmd->buttons & IN_ATTACK2) {
					pressing = true;
					aim->auto_fire = 0;
				}
				
			} else
				pressing = false;
			
			if (pressing)
				aim->type = AIM_TYPE_ZEN;
			
			float wait = (float)(g_cvars_misc->zen_wait_orbs.value) * 0.2f;
			if (g_aim->_last_type == AIM_TYPE_ZEN && aim->type == AIM_TYPE_NONE && local->zen_orbs >= wait) {
				aim->type = AIM_TYPE_ZEN;
				aim->auto_fire = 0;
			}
			
			break;
		}
		case HERO_ANA: {
			bool pressing = false;
			
			if (this->get_aim_key(g_cvars_misc->heal_key.value, 0) != -1)
				pressing = true;
			
			if (g_cvars_misc->heal_key.value == -1)
				pressing = true;
			
			if (g_cvars_misc->heal_type.value == 0)
				pressing = false;
			
			if (pressing) {
				memset(aim, 0, sizeof(aim_data));
				
				aim->type = AIM_TYPE_HEAL;
				aim->silent =  g_cvars_misc->heal_type.value == 2;
					
				aim->hitbox = -1;
				
				aim->fov_mult = (float)(g_cvars_misc->heal_fov.value);
				
				aim->smooth = (float)(g_cvars_misc->heal_smooth.value);
				
				aim->auto_fire = true;
				aim->wait_time = 0.5f;
			}
		
			break;
		}
		case HERO_WIDOW: {
			if (ucmd->buttons & IN_ATTACK && local->sniper_charge < 1.e-5f && false) {
				memset(aim, 0, sizeof(aim_data));
				
				aim->type = AIM_TYPE_SECONDARY;
				
				aim->range = 20.0f;
				aim->smooth = 5.0f;
				aim->smooth_fix = 1.0f;
				
				aim->single_target = true;
			}
			
			break;
		}
		case HERO_ECHO: {
			if (local->ability_limit == 0x6fc && false) {
				memset(aim, 0, sizeof(aim_data));
				
				aim->type = AIM_TYPE_ABILITY2;
				
				aim->range = 16.0f;
				aim->smooth = 5.0f;
				aim->smooth_fix = 1.0f;
				
				aim->hitbox = -1;
				
				aim->single_target = true;
			} else if (local->ability_limit == 0x1e && false) {
				memset(aim, 0, sizeof(aim_data));
				
				aim->type = AIM_TYPE_SECONDARY;
				
				aim->range = 10.0f;
				aim->smooth = 5.0f;
				aim->smooth_fix = 1.0f;
				
				aim->hitbox = -1;
				
				aim->single_target = true;
			}
			
			break;
		}
		case HERO_GENJI: {		
			bool deflecting = false;
			
			ow_condition* deflect = g_state->lookup_condition(&(local->object), HASH("usage_e"));
			if (deflect) {
				if (deflect->data.u32 & 1)
					deflecting = true;
			}
			
			if (deflecting && false) {
				memset(aim, 0, sizeof(aim_data));
				
				aim->type = AIM_TYPE_ABILITY2;
				
				aim->range = 50.0f;
				aim->smooth = 5.0f;
				aim->smooth_fix = 1.0f;
				
				aim->single_target = true;
			}
			
			break;
		}
		case HERO_TRACER: {
			bool ult = false;
			if (ucmd->buttons & IN_ABILITY3) {
				if (local->ability_limit == 0x16)
					ult = true;
				
				if (ucmd->buttons & IN_USE)
					ult = true;
			}
			
			if (ult && false) {
				aim->type = AIM_TYPE_ABILITY3;
				
				aim->range = 5.0f; // max range about 10
				aim->smooth = 5.0f;
				aim->smooth_fix = 1.0f;
				
				aim->hitbox = -1;
			}
			
			break;
		}
		case HERO_MCCREE: {
			if (ucmd->buttons & IN_ATTACK2 && false) {
				memset(aim, 0, sizeof(aim_data));
				
				aim->type = AIM_TYPE_SECONDARY;
				
				aim->hitbox = HITBOX_CHEST;
				
				aim->range = 7.5f;
				aim->smooth = 5.0f;
				aim->smooth_fix = 1.0f;
				
				aim->single_target = true;
			}
			
			break;
		}
		case HERO_ROADHOG: {
			float hook_time = 0.0f;
			
			ow_condition* hook = g_state->lookup_condition(&(local->object), HASH("hog_hook_time"));
			if (hook)
				hook_time = hook->data.fl;
			
			float cooldown = 0.0f;
			ow_condition* shift = g_state->lookup_condition(&(local->object), HASH("cooldown_shift"));
			if (shift)
				cooldown = shift->data.fl;
			
			bool crosshair = false;
			float dt = g_state->_curtime - g_local->_hook_attach_time;
			
			if (dt < 0.5f)
				crosshair = true;
			
			if (local->move_speed_mult == 0.75f && aim->type != AIM_TYPE_NONE) {
				
			} else if (crosshair) { // hook attached
				memset(aim, 0, sizeof(aim_data));
				
				aim->type = AIM_TYPE_ABILITY1;
				
				aim->fov_mult = 5.0f;
				
				aim->smooth = 3.0f;
				
				aim->range = 3.0f;
				
				aim->single_target = true;
				aim->distance_sorting = false;
			} else if (local->throwing_hook) { // throwing hook
				aim->type = AIM_TYPE_HOOK;
				
				aim->fov_mult = 5.0f;
				aim->hitbox = -1;
				aim->random = 0.0f;
				
				aim->smooth = 5.0f;
				aim->smooth_max = 0.0f;
				aim->scale_min = 0.0f;
				aim->scale_max = 0.0f;
				aim->smooth_fix = 1.0f;
				
				aim->auto_fire = false;
				aim->movement = false;
				aim->wait_time = 0.0f;
				aim->post_time = 0.0f;
				aim->autofire_time = 0.0f;
				aim->range = 25.0f;
				
				aim->single_target = true;
			} 
			
			break;
		}
		/*case HERO_ZARYA: {
			aim->hitbox = -1;
			aim->hitscan = 0;
			aim->pointscan = 0;
			
			bool right_click = false;
			
			bool key = this->get_ability_mode() != -1;
			if (key)
				right_click = true;
			
			if (right_click && g_cvars->ability_smooth.value > 0) {
				aim->type = AIM_TYPE_SECONDARY;
				//aim->auto_fire = true;
				
				//aim->fov_mult = 5.0f;
				//aim->fov_time = 0.0f;
				//aim->wait_time = 0.0f;
				aim->range = 100.0f;
				aim->smooth = (float)(g_cvars->ability_smooth.value);
				aim->smooth_fix = 0.0f;
				
				aim->projectile_speed = 25.0f;
				
				aim->single_target = false;
				//aim->force_every_frame = true;
				
				trig->type = TRIG_TYPE_SECONDARY;
				trig->hitbox = -1;
				trig->projectile_speed = 25.0f;
			}
			
			break;
		}*/
		case HERO_MERCY: {
			if (local->cur_weapon == 1.0f)
				aim->type = 0;
			
			break;
		}
		default: {
			
			
			break;
		}
	}
	
	if (!g_state->is_fire_ready())
		aim->type = AIM_TYPE_NONE;
	
	g_aim->_last_type = aim->type;
}

void c_hero::trig_think() {
	trig_data* trig = &(g_trig->_data);
	memset(trig, 0, sizeof(trig_data));
	
	if (g_aim->_data.type == AIM_TYPE_RAGE)
		return;
	
	int hero = local->object.hero;
	
	{
		trig->fire_delay = (float)(g_cvars->trig_delay.value) / 1000.0f;
		trig->head_only = g_cvars->trig_head_only.value != 0 ? true : false;
		trig->hitbox_scale = (float)(g_cvars->history_hitbox_scale.value) / 100.0f;
		trig->min_accuracy = (float)(g_cvars->trig_min_accuracy.value) / 100.0f;
		trig->history = g_cvars->trig_history.value != 0 ? true : false;
		trig->history_pred = g_cvars->trig_history_pred.value != 0 ? true : false;
		trig->objects = g_cvars->trig_objects.value != 0 ? true : false;
	}
	
	if (g_cvars->trig_type.value != 0) {
		if (this->get_aim_key(g_cvars->trig_key.value, g_cvars->trig_post_time.value) != -1)
			trig->pressing = true;
		
		if (g_cvars->trig_key.value == -1)
			trig->pressing = true;
		
		if (trig->pressing)
			trig->type = hero == HERO_GENJI ? TRIG_TYPE_SECONDARY : TRIG_TYPE_PRIMARY;
		
		if (g_cvars_misc->zen_trig.value != 0 && g_aim->_data.type == AIM_TYPE_ZEN)
			trig->type = TRIG_TYPE_ZEN;
	}
	
	if (trig->pressing) {
		if (g_trig->_press_time == 0.0f)
			g_trig->_press_time = g_state->_curtime;
		
		g_trig->_press_time_diff = g_state->_curtime - g_trig->_press_time;
	} else {
		g_trig->_press_time = 0.0f;
		g_trig->_press_time_diff = 0.0f;
	}
}

// YASRJ (workshop code) for info on melee hitboxes
void c_hero::auto_melee() { // auto melee when friendly hog gets a hook?
	if (!g_cvars_misc->misc_auto_melee.value)
		return;
	
	if (local->object.hero == HERO_REINHARDT || local->object.hero == HERO_BRIG)
		return;
	
	pseudo_cmd* ucmd = (pseudo_cmd*)&(g_ow->_cmd);
	Vector fwd = ucmd->silent.normal();
	
	for (int i = 0; i < g_state->_object_count; ++i) {
		ow_object* obj = &(g_state->_objects[i]);
		if (obj->type != OBJECT_PLAYER)
			continue;
		
		if (!g_state->is_alive(obj))
			continue;
		
		if (g_state->get_team(local->object.filter_bits) == g_state->get_team(obj->filter_bits))
			continue;
		
		Vector org = obj->position_root;
		// 1.5 distance, 1.0 radius
		if (g_anim->_melee_frame != 0) {
			int f = g_anim->_frame_count - g_anim->_melee_frame;
			if (f < 0)
				f = 0;
			
			c_anim_frame* frame = &(g_anim->_frames[f % MAX_ANIM_FRAMES]);
			for (int n = 0; n < frame->_object_count; ++n) {
				c_anim_object* anim_obj = &(frame->_objects[n]);
				if (anim_obj->_entity_parent == obj->entity_parent) {
					org = anim_obj->_position_root;
					break;
				}
			}
		}
		
		float height = obj->model_max.y - obj->position_root.y;
		
		Vector melee = local->vieworigin + (fwd * 1.5f);
		
		Vector dt = (org - melee);
		Vector dt_normal = dt.normal();
		
		float intersect = false;
		
		for (float f = 0.0f; f <= 0.8f; f += 0.2f) {
			if (((org + Vector(0.0f, height * f, 0.0f)) - melee).length() < 1.0f + 0.4f) {
				intersect = true;
				break;
			}
		}
		
		if (intersect == false)
			continue;
		
		float damage = 30.0f;
		if (obj->armor > 0.0f)
			damage -= 5.0f;
		
		if (obj->total_hp <= damage) {
			if (local->ability_limit != 0xD)
				g_ow->_cmd.buttons |= IN_MELEE;
			
			break;
		}
	}
}

void c_hero::zarya_super_jump() {
	if (local->object.hero != HERO_ZARYA)
		return;
	
	pseudo_cmd* ucmd = (pseudo_cmd*)&(g_ow->_cmd);
	
	if (ucmd->buttons & IN_USE) {
		Vector angles = Vector(ucmd->silent.pitch(), ucmd->silent.yaw(), 0.0f);
		
		int n = _jump_time % (62*6);
		_jump_time = std::min(_jump_time + 1, (62*5) + 5 + 1);
		
		if (n == 0) {
			angles.x = -89.0f;
			ucmd->buttons |= IN_ATTACK2;
		} else if (n == (62*5) + 5) {
			angles.x = 89.0f;
			ucmd->buttons |= IN_ATTACK2|IN_JUMP;
		}
		
		ucmd->silent = angles.forward().normal();
	} else {
		_jump_time = 0;
	}
}

void c_hero::fire_time_think() {
	int hero = local->object.hero;
	switch (hero) {
		case HERO_ECHO: {
			g_offense->echo_fire_think();
			break;
		}
		case HERO_SOLDIER: {
			g_offense->soldier_fire_think();
			break;
		}
		case HERO_GENJI: {
			g_offense->genji_fire_think();
			break;
		}
	}
}

int c_hero::get_shift_type(int hero) {
	int type = 0;
	
	switch (hero) {
		case HERO_ROADHOG: {
			type = CDTYPE_STUN;
			break;
		}
		case HERO_ASHE: {
			type = CDTYPE_CC_EAT;
			break;
		}
		case HERO_JUNK: {
			type = CDTYPE_CC_EAT;
			break;
		}
		case HERO_ANA: {
			type = CDTYPE_STUN_EAT;
			break;
		}
		case HERO_BRIG: {
			type = CDTYPE_CC;
			break;
		}
	}
	
	return type;
}

int c_hero::get_e_type(int hero) {
	int type = 0;
	
	switch (hero) {
		case HERO_REINHARDT: {
			type = CDTYPE_DAMAGE_EAT;
			break;
		}
		case HERO_SIGMA: {
			type = CDTYPE_STUN;
			break;
		}
		case HERO_ASHE: {
			type = CDTYPE_DAMAGE_EAT;
			break;
		}
		case HERO_MCCREE: {
			type = CDTYPE_STUN_EAT;
			break;
		}
		case HERO_ECHO: {
			type = CDTYPE_DAMAGE;
			break;
		}
		/*case HERO_SOMBRA: {
			type = CDTYPE_DAMAGE_EAT;
			break; // i don't think this can be eaten
		}*/
		case HERO_WIDOW: {
			type = CDTYPE_DAMAGE_EAT;
			break;
		}
		case HERO_ANA: {
			type = CDTYPE_DAMAGE_EAT;
			break;
		}
		case HERO_MOIRA: {
			type = CDTYPE_DAMAGE_EAT;
			break;
		}
	}
	
	return type;
}

int c_hero::get_m2_type(int hero) {
	int type = 0;
	
	switch (hero) {
		case HERO_ORISA: {
			type = CDTYPE_CC_EAT;
			break;
		}
		case HERO_ECHO: {
			type = CDTYPE_DAMAGE_EAT;
			break;
		}
		case HERO_SOLDIER: {
			type = CDTYPE_DAMAGE_EAT;
			break;
		}
		case HERO_SOMBRA: {
			type = CDTYPE_STUN;
			break;
		}
		case HERO_SYM: {
			type = CDTYPE_DAMAGE_EAT;
			break;
		}
		case HERO_BRIG: {
			type = CDTYPE_STUN;
			break;
		}
		case HERO_LUCIO: {
			type = CDTYPE_CC;
			break;
		}
	}
	
	return type;
}

int c_hero::get_ult_type(int hero) {
	switch (hero) {
		case HERO_ROADHOG:
		case HERO_ZARYA:
		case HERO_HANZO:
		case HERO_MEI:
		case HERO_PHARAH:
		case HERO_REAPER:
		case HERO_TRACER: {
			return CDTYPE_ULT_EAT;
		}
	}
	
	return 0;
}