#include "base.h"

c_trig trig_instance;
c_trig* g_trig = &trig_instance;

bool c_trig::is_trigger_target(ow_object* obj) {
	bool shotgun = local->shotgun;
	
	Vector forward = g_ow->_cmd.silent;
	Vector org = obj->model_center;
	
	if (obj->type != OBJECT_PLAYER)
		return false;
	
	bool team_mate = false;
	if (local->team != 3) {
		if (local->team == g_state->get_team(obj->filter_bits))
			team_mate = true;
	}
	
	if (team_mate) {
		if (obj->total_hp == obj->total_max_hp)
			return false;
	}
	
	Vector dt = (org - local->vieworigin);
	Vector dt_normal = dt.normal();
	
	float dist = dt.length_sqr();
	float diff = forward.dot(dt_normal);
	
	if (dist > 100.0f * 100.0f)
		return false;
	
	if (diff < 0.0f)
		return false; // ignore any > 90 degrees behind
	
	if (_data.hitbox != -1) {
		Vector delta = obj->predicted_delta;
		
		if (_data.head_only)
			g_anim->_intersect_flags = (1<<HITBOX_HEAD);
		else
			g_anim->_intersect_flags = ~0;
		
		g_anim->_intersect_mult = _data.hitbox_scale;
		
		g_anim->_history_pred = _data.history_pred;
		
		float dmg = 0.0f;
		
		if (_data.history)
			dmg = g_anim->find_history_intersection(obj, forward, delta, &(g_anim->_tick_force_ms));
		else if (shotgun)
			dmg = g_anim->shotgun_intersection(obj, forward, delta, nullptr);
		else
			dmg = g_anim->find_hitbox_intersection(obj, forward, delta, nullptr, nullptr) != -1 ? 1.0f : 0.0f;
		
		if (shotgun) {
			if (_data.min_accuracy > 0.0f) {
				if (dmg >= _data.min_accuracy - 0.01f)
					return true;
			} else {
				if (dmg != 0.0f)
					return true;
			}
		} else {
			if (dmg != 0.0f)
				return true;
		}
	} else {
		Vector delta = obj->predicted_delta;
		g_anim->_intersect_mult = _data.hitbox_scale;
		
		Vector new_forward = g_ow->_cmd.silent;
		
		if (g_anim->find_capsule_intersection(obj, new_forward, delta) != -1)
			return true;
	}
	
	return false;
}

bool c_trig::get_target() {
	for (int i = 0; i < g_state->_object_count; ++i) {
		ow_object* obj = &(g_state->_objects[i]);
		
		if (!g_aim->is_valid_target(obj))
			continue;
		
		if (this->is_trigger_target(obj))
			return true;
	}
	
	return false;
}

void c_trig::think() {
	pseudo_cmd* change_cmd = &(g_ow->_cmd);
	bool fire = false;
	
	if (this->update())
		fire = true;
	
	if (fire)
		_first_fired = true;
	else if (_data.type == TRIG_TYPE_NONE)
		_first_fired = false;
	
	if (_data.fire_delay != 0.0f) {
		if (_press_time_diff >= _data.fire_delay)
			fire = true;
	}
	
	if (g_aim->_aim_obj != nullptr) {
		if (g_aim->_aim_obj->type != OBJECT_PLAYER)
			fire = false;
	}
	
	if (local->object.hero == HERO_WIDOW) {
		if (local->sniper_charge < 0.01f)
			fire = false;
		
		if (_data.min_accuracy > 0.0f) {
			if (local->sniper_charge < _data.min_accuracy)
				fire = false;
		}
		
		if (local->fire_ready == false)
			fire = false;
	}
	
	if (local->object.hero == HERO_ASHE) {
		if (local->fire_ready == false)
			fire = false;
	}
	
	if (local->object.hero == HERO_HANZO && local->ability_limit != 0x5a1) {
		if (local->sniper_charge < 0.01f)
			fire = false;
		
		if (_data.min_accuracy > 0.0f) {
			if (local->sniper_charge < _data.min_accuracy)
				fire = false;
		}
	}
	
	if (_data.type == TRIG_TYPE_ZEN) {
		if (local->zen_orbs < 0.2f)
			fire = false;
		
		float wait = (float)(g_cvars_misc->zen_wait_orbs.value) * 0.2f;
		if (local->zen_orbs < wait)
			fire = false;
	}
	
	if (!g_state->is_fire_ready())
		fire = false;
	
	if (fire) {
		if (local->object.hero == HERO_HANZO && local->ability_limit != 0x5a1)
			change_cmd->buttons &= ~(IN_ATTACK);
		else if (_data.type == TRIG_TYPE_ZEN)
			change_cmd->buttons &= ~IN_ATTACK2;
		else if (_data.type == TRIG_TYPE_PRIMARY)
			change_cmd->buttons |= IN_ATTACK;
		else if (_data.type == TRIG_TYPE_SECONDARY)
			change_cmd->buttons |= IN_ATTACK2;
		
		if (_data.type == TRIG_TYPE_ZEN) {
			if (g_cvars_misc->zen_melee.value != 0 && (g_ow->_last_buttons & IN_ATTACK2) == 0)
				change_cmd->buttons |= IN_MELEE;
		}
	}
}

bool c_trig::update() {
	if (_data.type == TRIG_TYPE_NONE)
		return false;
	
	if (local->projectile_speed > 0.0f) {
		if (this->get_target())
			return true;
	} else {
		pseudo_ray_t* ray = &(local->view_ray);
		bool fire = false;
		bool head = true;
		
		{
			//if (_data.fix_accuracy) {
				if (this->get_target()) // see update_trig_rays
					return true;
			//}
			
			if (ray->fraction > 0.0f && ray->fraction < 1.0f) {
				if (_data.objects)
					fire = true;
			}
		}
		
		if (fire && head)
			return true;
	}
	
	return false;
}