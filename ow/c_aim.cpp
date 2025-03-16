#include "base.h"

c_aim aim_instance;
c_aim* g_aim = &aim_instance;

bool c_aim::is_valid_aim_ent(ow_object* obj) {
	if (obj->type == OBJECT_PLAYER && g_state->is_player(obj->filter_bits)) {
		bool team_mate = local->team == g_state->get_team(obj->filter_bits);
		
		if (local->team != 3 && local->object.hero != HERO_ANA) {
			if (team_mate)
				return false;
		}
		
		switch (obj->hero) {
			case HERO_TRACER: {
				ow_condition* e = g_state->lookup_condition(obj, HASH("usage_e"));
				if (e) {
					if (e->data.u32 & 1)
						return false;
				}
				
				break;
			}
			case HERO_GENJI: {
				ow_condition* e = g_state->lookup_condition(obj, HASH("usage_e"));
				if (e && team_mate == false) {
					if (e->data.u32 & 1)
						return false;
				}
				
				break;
			}
			case HERO_REAPER: {
				ow_condition* shift = g_state->lookup_condition(obj, HASH("usage_shift"));
				if (shift && team_mate == false) {
					if (shift->data.u32 & 1)
						return false;
				}
				
				break;
			}
			case HERO_MOIRA:
			case HERO_MEI: {
				ow_condition* shift = g_state->lookup_condition(obj, HASH("usage_shift"));
				if (shift) {
					if (shift->data.u32 & 1)
						return false;
				}
				
				break;
			}
		}
		
		
	//	if (obj->hero == HERO_DVA)
		//	return false;
		
		return true;
	}
	
	if (obj->type == OBJECT_DYNAMITE) {
		if (local->object.hero != HERO_ASHE)
			return false;
		
		if (local->team == 3)
			return false;
		
		if (g_state->is_object_destroyed(obj->filter_bits))
			return false;
		
		if (local->team != g_state->get_team(obj->filter_bits))
			return false;
		
		return true;
	}
	
	return false;
}

bool c_aim::is_valid_target(ow_object* obj) {
	if (!g_state->is_alive(obj))
		return false;
	
	if (!this->is_valid_aim_ent(obj))
		return false;
	
	return true;
}

int c_aim::get_hitbox_level(ow_object* obj) {
	if (local->object.hero == HERO_ROADHOG) {
		switch (obj->hero) {
			case HERO_BRIG:
			//case HERO_MERCY:
			//case HERO_ZEN:
			case HERO_REINHARDT:
			case HERO_ZARYA: {
				return 0;
			}
			case HERO_REAPER:
			case HERO_MERCY:
			case HERO_ZEN:
			case HERO_SIGMA: {
				return 2;
			}
			case HERO_ROADHOG:
			case HERO_ASHE:
			case HERO_GENJI:
			case HERO_SOLDIER:
			case HERO_DOOM: {
				return 1;
			}
		}
		
		return 1;
	}
	
	return _data.hitbox_level;
}

bool c_aim::get_heal_pos(ow_object* obj, Vector* pos_out, float* diff_out) {
	int value = g_hero_data[obj->hero].value;
	
	Vector vieworigin = local->vieworigin;
	Vector viewforward = local->viewforward;
	
	Vector org = obj->type == OBJECT_PLAYER ? obj->model_center: obj->position;
	
	Vector dt = (org - local->vieworigin);
	Vector dt_normal = dt.normal();
	
	float dist = dt.length_sqr();
	float diff = viewforward.dot(dt_normal);
	
	if (local->team != g_state->get_team(obj->filter_bits))
		return false;
	
	if (obj->total_hp == obj->total_max_hp)
		return false;
	
	if (value < 3000) {
		if (local->ammo_percent < 0.5f) {
			//if ((obj->total_max_hp - obj->total_hp) < 50)
			//	return false;
		}
	}
	
	if (obj->is_visible == false)
		return false;
	
	if (_data.fov_mult > 0.0f) {
		if (diff < cosf(DEG2RAD(_data.fov_mult)))
			return false;
	}
	
	int points = 0;
	
	if (value < 3000) {
		float frac = std::min(obj->total_hp / std::max(obj->total_max_hp, 1.0f), 1.0f);
		
		points = (int)(1000.0f * frac);
		if (obj->total_hp < 125) // one shot range
			points = value;
	} else {
		points = 1.e+9;
	}
	
	float calc = (float)(points) / 1000.0f;
	
	{
		if (pos_out)
			*pos_out = org;
		
		if (diff_out)
			*diff_out = calc;
		
		return true;
	}
	
	return false;
}

bool c_aim::get_aim_pos(ow_object* obj, Vector* pos_out, float* diff_out) {
	bool team_mate = false;
	if (local->team != 3) {
		if (local->team == g_state->get_team(obj->filter_bits))
			team_mate = true;
	}
	
	if (_data.type == AIM_TYPE_RAGE && team_mate)
		return false;
	
	if (team_mate) {
		if (obj->total_hp == obj->total_max_hp)
			return false;
	}
	
	Vector vieworigin = local->vieworigin;
	Vector viewforward = local->viewforward;
	
	Vector org = obj->type == OBJECT_PLAYER ? obj->model_center : obj->position;
	
	if (_data.type == AIM_TYPE_SECONDARY && local->object.hero == HERO_ZARYA) {
		org = obj->model_min;
		org.y += (obj->model_max.y - obj->model_min.y) * 0.1f;
	}
	
	Vector delta = obj->predicted_delta;
	
	if (_data.silent == false && _data.fov_mult > 0.0f && obj->type == OBJECT_PLAYER) {
		//g_anim->_intersect_flags = (1<<_data.hitbox);
		g_anim->_intersect_mult = _data.fov_mult;
		
		if (!g_anim->is_in_fov(obj, viewforward, delta, 0))
			return false;
	}
	
	Vector dt = (org - vieworigin);
	Vector dt_normal = dt.normal();
	
	float dist = dt.length_sqr();
	float diff = viewforward.dot(dt_normal);
	
	if (_data.type != AIM_TYPE_RAGE) {
		if (diff < 0.0f)
			return false; // ignore any > 90 degrees behind
	}
	
	if (obj->type == OBJECT_PLAYER) {
		if (dist > _data.range * _data.range)
			return false;
	} else {
		if (diff < cosf(DEG2RAD(5.0f)))
			return false;
	}
	
	if (_data.type == AIM_TYPE_ABILITY1 && local->object.hero == HERO_ROADHOG) {
		if (diff < cosf(DEG2RAD(45.0f)))
			return false;
	}
	
	if (obj->type != OBJECT_PLAYER || _data.hitbox == -1) {
		if (obj->is_visible == false)
			return false;
		
		if (_data.silent && _data.fov_mult > 0.0f) {
			if (diff < cosf(DEG2RAD(_data.fov_mult)))
				return false;
		}
		
		if (pos_out)
			*pos_out = org + delta;
		
		if (diff_out)
			*diff_out = diff;
		
		return true;
	}
	
	matrix_t maty;
	OW_InitY(obj->rotation.x, maty);
	
	int best_points = -1;
	
	Vector position_root = obj->position_root;
	
	bool bot = obj->hero == HERO_TRAININGBOT1 || obj->hero == HERO_TRAININGBOT4;
	
	bool head_visible = obj->hitbox_visible[HITBOX_HEAD];
	bool chest_visible = obj->hitbox_visible[HITBOX_CHEST];
	
	if (bot) {
		Vector head = obj->position_root + Vector(0.0f, 1.96f, 0.0f);
		Vector body = obj->position_root + Vector(0.0f, 1.3f, 0.0f);
		
		for (int i = 0; i < 2; ++i) {
			Vector base = {};
			
			if (_data.hitbox == 3) {
				if (obj->hitbox_id[i] != obj->hitbox_map[HITBOX_HEAD] && obj->hitbox_id[i] != obj->hitbox_map[HITBOX_CHEST])
					continue;
			}
			
			if (_data.hitbox >= 0 && _data.hitbox < HITBOX_MAX) {
				if (i != _data.hitbox)
					continue;
			}
			
			if (i == 0 && head_visible == false)
				continue;
			
			if (i == 1 && chest_visible == false)
				continue;
			
			if (i == 0)
				base = head;
			else
				base = body;
			
			Vector pos = base + delta;
			Vector dir = (pos - vieworigin).normal();
			
			int points = 0;
			float diff = dir.dot(viewforward);
			
			if (_data.silent && _data.fov_mult > 0.0f) {
				if (diff < cosf(DEG2RAD(_data.fov_mult)))
					continue;
			}
			
			if (true) {
				float sorting = (1.0f + diff) * 1.e+4f;
				points += (int)(sorting);
				
				if (team_mate)
					points += 1.e+8f;
			}
			
			if (points > best_points) {
				best_points = points;
				
				if (pos_out)
					*pos_out = pos;
				
				if (diff_out)
					*diff_out = diff;
			}
		}
		
		if (best_points == -1)
			return false;
		
		return true;
	}
	
	for (int i = 0; i < obj->hitbox_count; ++i) {
		if (head_visible == false || chest_visible == false) {
			if (obj->hitbox_id[i] == obj->hitbox_map[HITBOX_HEAD]) {
				if (head_visible == false)
					continue;
			} else if (obj->hitbox_id[i] == obj->hitbox_map[HITBOX_CHEST]) {
				if (chest_visible == false)
					continue;
			} else {
				continue;
			}
		}
		
		if (_data.hitbox == 3) {
			if (obj->hitbox_id[i] != obj->hitbox_map[HITBOX_HEAD] && obj->hitbox_id[i] != obj->hitbox_map[HITBOX_CHEST])
				continue;
		}
		
		if (_data.hitbox >= 0 && _data.hitbox < HITBOX_MAX) {
			if (obj->hitbox_id[i] != obj->hitbox_map[_data.hitbox])
				continue;
		}
		
		Vector base = {};
		Vector point_offset = {};
		
		float radius = obj->hitbox_radius[i];
		
		if (_data.random > 0.0f) {
			uint32_t xseed = _xseed;
			uint32_t yseed = _yseed;
			uint32_t zseed = _zseed;
			
			int test = (int)(xseed % 100) * ((xseed & 1) ? -1 : 1);
			int test2 = (int)(yseed % 100) * ((yseed & 1) ? -1 : 1);
			int test3 = (int)(zseed % 100) * ((zseed & 1) ? -1 : 1);
			
			float x = radius * (0.01f * (float)(test));
			float y = radius * (0.01f * (float)(test2));
			float z = radius * (0.01f * (float)(test3));
			
			point_offset = Vector(x, y, z) * _data.random;
		}
		
		if (local->object.hero == HERO_ROADHOG) {
			switch (obj->hero) {
				case HERO_ROADHOG:
				case HERO_DOOM:
				case HERO_SOLDIER:
				case HERO_GENJI:
				case HERO_ASHE:
				case HERO_HANZO: {
					point_offset += Vector(0.0f, -0.05f, 0.0f);
					break;
				}
			}
			
			if (obj->hero == HERO_ANA)
				point_offset += Vector(0.0f, -0.06f, 0.0f);
			
			if (obj->hero == HERO_MEI)
				point_offset += Vector(0.0f, -0.09f, 0.0f);
			
			if (obj->hero == HERO_SOMBRA)
				point_offset += Vector(0.0f, 0.05f, 0.0f);
			
			if (obj->hero == HERO_ORISA)
				point_offset += Vector(0.0f, 0.06f, 0.0f);
			
			if (obj->hero == HERO_MERCY)
				point_offset += Vector(0.0f, 0.08f, 0.0f);
			
			if (obj->hero == HERO_TORB)
				point_offset += Vector(-0.05f, -0.05f, 0.0f);
		}
		
		int hitbox_level = this->get_hitbox_level(obj);
		
		{
			Vector offset = obj->hitbox_offset[i];
			/*Vector min = offset + Vector(box->min[0], box->min[1], box->min[2]);
			Vector max = offset + Vector(box->max[0], box->max[1], box->max[2]);
			
			if (i == HITBOX_HEAD) {
				if (hitbox_level == 1)
					offset = min;
				else if (hitbox_level == 2)
					offset = max;
			}*/
			
			offset += point_offset;
			
			OW_Transform(offset, maty, &base);
			base += position_root;
		}
		
		Vector pos = base + delta;
		Vector dir = (pos - vieworigin).normal();
		
		int points = 0;
		float diff = dir.dot(viewforward);
		
		if (_data.silent && _data.fov_mult > 0.0f) {
			if (diff < cosf(DEG2RAD(_data.fov_mult)))
				continue;
		}
		
		if (_data.distance_sorting) {
			if (_data.type == AIM_TYPE_ABILITY1 && local->object.hero == HERO_ROADHOG) {
				Vector hook_dt = (local->hog_hook_pos - vieworigin);
				dist = (hook_dt.x * hook_dt.x) + (hook_dt.z * hook_dt.z);
			}
			
			if (_data.type == AIM_TYPE_RAGE && obj->total_max_hp > 300.0f)
				dist /= 100.0f;
			
			if (dist > 1.e-9f)
				diff = 1.0f / dist;
		}
		
		if (true) {
			float sorting = (1.0f + diff) * 1.e+4f;
			points += (int)(sorting);
		}
		
		if (points > best_points) {
			best_points = points;
			
			if (pos_out)
				*pos_out = pos;
			
			if (diff_out)
				*diff_out = diff;
		}
	}
	
	if (best_points == -1)
		return false;
	
	return true;
}

bool c_aim::get_target() {
	float best_diff = -1.0f;
	
	_aim_obj = nullptr;
	
	for (int i = 0; i < g_state->_object_count; ++i) {
		ow_object* obj = &(g_state->_objects[i]);
		
		if (_data.single_target) {
			if (_last_obj != nullptr) {
				if (obj != _last_obj)
					continue;
			}
		}
		
		if (!this->is_valid_target(obj))
			continue;
		
		Vector pos = Vector(0.0f, 0.0f, 0.0f);
		float diff = 0.0f;
		
		if (_data.type == AIM_TYPE_HEAL) {
			if (!this->get_heal_pos(obj, &pos, &diff))
				continue;
		} else {
			if (!this->get_aim_pos(obj, &pos, &diff))
				continue;
		}
		
		if (_data.type == AIM_TYPE_RAGE) {
			if (obj == _last_obj)
				diff = 1.e+9f;
		}
		
		if (diff > best_diff) {
			best_diff = diff;
			
			_aim_obj = obj;
			_aim_pos = pos;
		}
	}
	
	if (best_diff != -1.0f) {
		if (_data.type == AIM_TYPE_RAGE) {
			if (_aim_obj != _last_obj)
				_last_obj = nullptr;
		}
		
		if (_last_obj == nullptr)
			_last_obj = _aim_obj;
		
		return true;
	}
	
	return false;
}

void c_aim::think() {
	_aiming = false;
	_on_heal_target = false;
	
	g_hero->aim_think();
	
	if (!this->update()) {
		_aim_time = 0.0f;
		_xseed = FNV1A_RT((char*)&(g_ow->_game_tickcount), 2);
		_yseed = FNV1A_RT((char*)&(g_ow->_game_tickcount), 3);
		_zseed = FNV1A_RT((char*)&(g_ow->_game_tickcount), 4);
	}
	
	this->update_silent_aim();
	
	this->autofire_think();
	
	this->apply_smooth_friction();
	this->cap_smooth_speed();
}

bool c_aim::update() {
	if (_data.type == AIM_TYPE_NONE) {
		_key_time = 0.0f;
		_autofire_time = 0.0f;
		_last_obj = nullptr;
		
		pseudo_cmd* change_cmd = &(g_ow->_cmd);
		
		if (_arrow_drawn) {
			change_cmd->buttons |= IN_ATTACK|IN_ATTACK2;
			
			_arrow_drawn = false;
		}
		
		if (_zen_charging) {
			float wait = (float)(g_cvars_misc->zen_wait_orbs.value) * 0.2f;
			if (local->zen_orbs < wait)
				change_cmd->buttons |= IN_ATTACK2;
			
			float ammo = 0.0f;
			
			ow_condition* cond = g_state->lookup_condition(&(local->object), HASH("zen_ammo"));
			if (cond)
				ammo = cond->data.fl;
			
			if (ammo > 0.0f) {
				if (g_cvars_misc->zen_melee.value != 0)
					change_cmd->buttons |= IN_MELEE;
			}
			
			_zen_charging = false;
		}
		
		return false;
	}
	
	if (local->object.hero == HERO_HANZO && local->ability_limit != 0x5a1 && _data.type != AIM_TYPE_RAGE) {
		if (_data.auto_fire != 0) {
			pseudo_cmd* change_cmd = &(g_ow->_cmd);
			
			change_cmd->buttons |= IN_ATTACK;
			
			if (local->sniper_charge < _data.wait_time)
				_arrow_drawn = true;
			else
				_arrow_drawn = false;
		}
	}
	
	if (local->object.hero == HERO_ZEN) {
		if (_data.auto_fire != 0) {
			pseudo_cmd* change_cmd = &(g_ow->_cmd);
			
			bool cycle = true;
			
			if (local->zen_orbs == 0.0f) {
				if (g_ow->_game_tickcount % 2)
					cycle = false;
			}
			
			if (cycle)
				change_cmd->buttons |= IN_ATTACK2;
			
			_zen_charging = true;
		}
	}
	
	_key_time += OW_TICK_INTERVAL;
	_autofire_time += OW_TICK_INTERVAL;
	
	if (!this->get_target())
		return false;
	
	if (!this->set_aim())
		return false;
	
	return true;
}

bool c_aim::pre_aim() {
	bool aim = true;
	
	if (local->fire_time != 0.0f && _data.wait_time != 0.0f) {
		if (local->object.hero == HERO_WIDOW || (local->object.hero == HERO_HANZO && local->ability_limit != 0x5a1)) {
			if (local->sniper_charge < _data.wait_time)
				aim = false;
		} else {
			if (local->fire_time < _data.wait_time)
				aim = false;
		}
	}
	
	if (_data.type == AIM_TYPE_ZEN) {
		float wait = (float)(g_cvars_misc->zen_wait_orbs.value) * 0.2f;
		if (local->zen_orbs < wait)
			aim = false;
	} 
	
	if (_aim_obj->type != OBJECT_PLAYER) {
		aim = true;
		// object aim stuff
	}
	
	Vector v1 = _aim_obj->velocity.normal();
	Vector v2 = local->object.velocity.normal();
	
	float move_dir = v1.dot(v2);
	
	Vector dt = (_aim_pos - local->vieworigin).normal();
	_aim_dir = dt;
	
	if (_data.movement) {
		if (move_dir < 0.0f)
			aim = false;
	}
	
	return aim;
}

void c_aim::apply_smooth(Vector forward, bool silent) {
	if (_data.smooth < 1.0f)
		return;
	
	Vector dt = _aim_dir;
	
	float smooth = _data.smooth;
	if (_data.scale_max > 0.0f)
		smooth = _data.smooth_max;
	 
	this->get_smooth_scale(forward, dt, &smooth);
	
	float smooth_mul = (1.0f / smooth);
	
	if (_data.smooth_sine > 0.0f) {
		float sine_length = _data.smooth_sine;

		// 5.0f = 200 MS
		float angle = fmodf(5.0f * (g_state->_curtime / OW_TICK_INTERVAL), 360.0f);
		float sine = (1.5f - (0.5f * (1.0f - sine_length))) - (fabsf(sinf(DEG2RAD(angle))) * sine_length);
		
		smooth_mul *= sine;
	}
	
	Vector accel = dt * 1.e+5f;//std::max(_data.smooth_accel / 10.0f, 1.0f);
	for (int i = 0; i < 3; ++i) {
		if (accel[i] < -1.0f)
			accel[i] = -1.0f;
		
		if (accel[i] > 1.0f)
			accel[i] = 1.0f;
	}
	
	_smooth_base += accel * _data.smooth_accel * OW_TICK_INTERVAL;
	
	this->cap_smooth_speed();
	
	if (_data.smooth_accel == 0.0f)
		_smooth_base = Vector(1.0f, 1.0f, 1.0f);
	
	Vector smooth_vec = _smooth_base;
	float special_len = sqrtf(((smooth_vec.x*smooth_vec.x) + (smooth_vec.y*smooth_vec.y) + (smooth_vec.z*smooth_vec.z)) / 3.0f);
	
	smooth_vec.x = fabsf(smooth_vec.x) * smooth_mul;
	smooth_vec.y = fabsf(smooth_vec.y) * smooth_mul;
	smooth_vec.z = fabsf(smooth_vec.z) * smooth_mul;

	if (_data.smooth_fix > 0.0f && smooth > 0.0f) {
		float temp = smooth_mul * special_len;
		if (temp < 1.e-5f)
			temp = 1.e-5f;
		
		float t = 1.0f / temp;
		t *= _data.smooth_fix;
		
		int ticks = (int)(t + 0.5f) - 1;
		
		Vector delta = Vector(0.0f, 0.0f, 0.0f);
		//delta += _aim_obj->velocity * (t * OW_TICK_INTERVAL);
		g_anim->calculate_delta(_aim_obj, ticks, 0.0f, &delta);
		
		Vector newpos = _aim_pos + delta;
		
		Vector local_delta = Vector(0.0f, 0.0f, 0.0f);
		g_anim->calculate_delta(&(local->object), ticks, 0.0f, &local_delta);
		
		Vector neworg = local->vieworigin + local_delta;
		
		Vector normal = (newpos - neworg).normal();
		
		dt = normal;
	}
	
	if (silent) {
		Vector aim_delta = (dt - g_ow->_cmd.silent);
		_silent_delta += (aim_delta - _silent_delta) * smooth_vec;
	} else {
		Vector smooth_dt = (dt - forward) * smooth_vec;
		_aim_dir = forward + smooth_dt;
		_aim_dir = _aim_dir.normal();
	}
}

bool c_aim::set_aim() {
	Vector viewforward = g_ow->_cmd.forward;
	
	if (_data.type == AIM_TYPE_RAGE) {
		if (local->fire_time != 0.0f && local->object.hero != HERO_BAPTISTE) {
			int hero = local->object.hero;
			
			float attack_recovery = g_hero_data[hero].attack_recovery;
			if (hero == HERO_WIDOW)
				attack_recovery = 0.5f + 0.9f;
			else if (hero == HERO_HANZO && local->ability_limit == 0x5a1)
				attack_recovery = 0.25f;
			
			if (local->fire_time < attack_recovery)
				return false;
		}
	}
	
	if (_aim_obj->type != OBJECT_PLAYER)
		_data.smooth_fix = 1.0f;
	
	if (this->pre_aim() == false) {
		if (_data.auto_fire == 2)
			_autofire_time = 0.0f;
		
		return false;
	}
	
	if (_data.type != AIM_TYPE_RAGE) {
		{
			Vector test = Vector(90.0f, 0.0f, 0.0f);
			Vector test_forward = test.forward().normal();
			
			if (_aim_dir.dot(test_forward) > 0.93969f)
				return false;
		}
		
		{
			Vector test = Vector(-90.0f, 0.0f, 0.0f);
			Vector test_forward = test.forward().normal();
			
			if (_aim_dir.dot(test_forward) > 0.93969f)
				return false;
		}
	}
	
	if (_data.silent == false) {
		this->apply_smooth(viewforward, false);
		
		pseudo_cmd* change_cmd = &(g_ow->_cmd);
		
		bool rotate = true;
		
		if (rotate) {
			Vector new_dir = _aim_dir.normal();
			
			change_cmd->silent = new_dir;
			
			change_cmd->bits |= UCMD_FORWARD;
			change_cmd->forward = new_dir;
		}
		
		if (_data.type == AIM_TYPE_RAGE)
			change_cmd->bits &= ~UCMD_FORWARD;
	}
	
	_aiming = true;
	
	_aim_time += OW_TICK_INTERVAL;
	
	_backward_tick = _aim_obj->predicted_backward;
	_last_aimed = g_state->_curtime;
	
	return true;
}

void c_aim::autofire_think() {
	pseudo_cmd* change_cmd = &(g_ow->_cmd);
	bool auto_fire = false;
	
	if (_data.autofire_time > 0.0f && _autofire_time >= _data.autofire_time)
		g_trig->_first_fired = true;
	
	if (_data.auto_fire != 0) {
		if (_data.auto_fire == 2 && _aiming)
			auto_fire = true;
		else if (_data.auto_fire == 1)
			auto_fire = true;
	}
	
	if (_data.autofire_time > 0.0f && _autofire_time < _data.autofire_time)
		auto_fire = false;
	
	if (local->object.hero == HERO_HANZO && _data.type != AIM_TYPE_RAGE)
		auto_fire = false;
	
	if (g_trig->_data.type != TRIG_TYPE_NONE && g_trig->_first_fired == false)
		auto_fire = false;
	
	if (_data.type == AIM_TYPE_ABILITY2 || _data.type == AIM_TYPE_HEAL)
		auto_fire = false;
	
	if (_data.type == AIM_TYPE_RAGE) {
		if (_aiming)
			auto_fire = true;
		else
			auto_fire = false;
		
		if (local->object.hero == HERO_HANZO && local->ability_limit != 0x5a1) {
			if (_aiming && local->sniper_charge > 0.99f)
				auto_fire = false;
			else
				auto_fire = true;
		}
	}
	
	if (_data.type == AIM_TYPE_HEAL && _aiming) {
		Vector delta = _aim_obj->predicted_delta;
		g_anim->_intersect_mult = 0.9f;
		
		Vector new_forward = g_ow->_cmd.silent;
		
		if (g_anim->find_capsule_intersection(_aim_obj, new_forward, delta) != -1)
			auto_fire = true;
	}
	
	if (auto_fire) {
		if (_data.type == AIM_TYPE_HOOK)
			change_cmd->buttons |= IN_ABILITY1;
		else if (_data.type == AIM_TYPE_PRIMARY || _data.type == AIM_TYPE_RAGE || _data.type == AIM_TYPE_HEAL)
			change_cmd->buttons |= IN_ATTACK;
		else if (_data.type == AIM_TYPE_SECONDARY )//|| _data.type == AIM_TYPE_ZEN)
			change_cmd->buttons |= IN_ATTACK2;
	}
}

void c_aim::update_silent_aim() {
	if (_data.type == AIM_TYPE_RAGE) {
		_silent_delta = Vector(0.0f, 0.0f, 0.0f);
		return;
	}
	
	pseudo_cmd* change_cmd = &(g_ow->_cmd);
	
	Vector viewforward = change_cmd->silent;
	
	if (_aiming == false && _silent_delta.length() < 1.e-3f) {
		_silent_delta = Vector(0.0f, 0.0f, 0.0f);
		return;
	}
	
	Vector view_delta = Vector(0.0f, 0.0f, 0.0f);
	
	if (_data.silent == true && _aiming)
		this->apply_smooth(viewforward + _silent_delta, true);
	else if (_silent_delta.length() > 1.e-3f)
		_silent_delta += (view_delta - _silent_delta) / std::max((float)(g_cvars->aim_silent_detach.value), 1.0f);
	else
		_silent_delta = Vector(0.0f, 0.0f, 0.0f);
	
	Vector new_dir = viewforward + _silent_delta;
	
	change_cmd->silent = new_dir.normal();
}

void c_aim::force_tick() {
	if (g_cvars->history_lowest.value == 0)
		return;
	
	if ((g_state->_curtime - _last_aimed) > 1.0f)
		return;
	
	g_anim->_tick_force_ms = (int)((OW_TICK_INTERVAL * (float)(_backward_tick)) * 1000.0f);
	if (g_anim->_tick_force_ms + g_anim->_tick_limit_ms < 0)
		g_anim->_tick_force_ms = g_anim->_tick_limit_ms * -1;
}

void c_aim::get_smooth_scale(Vector viewforward, Vector dir, float* smooth) {
	float base = _data.smooth;
	float min = _data.scale_min;
	float max = _data.scale_max;
	
	if (min > max || (min == 0.0f && max == 0.0f))
		return;
	
	float dot = dir.dot(viewforward);
	float diff = RAD2DEG(acosf(dot));
	
	float dist = diff / base;
	if (min > 0.0f) {
		float new_diff = diff - min;
		if (new_diff < 0.0f)
			new_diff = 0.0f;
		
		dist = new_diff / base;
	}
	
	bool modify = true;
	if (max > 0.0f && diff > max)
		modify = false;
	
	if (modify)
		*smooth = base + ((*smooth - base) * dist);
	
	*smooth = std::max(*smooth, 1.0f);
}

void c_aim::apply_smooth_friction() {	
	if (_data.smooth_accel == 0.0f)
		return;
	
	float drop = (_data.smooth_accel * (1.0f / 3.0f)) * OW_TICK_INTERVAL;
	float speed = _smooth_base.length();
	
	float newspeed = speed - drop;
	if (newspeed < 0.0f)
		newspeed = 0.0f;
	
	if (speed > 1.e-5f) {
		newspeed /= speed;
		_smooth_base *= newspeed;
	}
}

void c_aim::cap_smooth_speed() {
	float max_speed = _data.smooth_accel_cap;
	if (max_speed < 1.e-5f)
		max_speed = 1.0f;
	
	for (int i = 0; i < 3; ++i) {
		if (_smooth_base[i] < -max_speed)
			_smooth_base[i] = -max_speed;
		
		if (_smooth_base[i] > max_speed)
			_smooth_base[i] = max_speed;
	}
}