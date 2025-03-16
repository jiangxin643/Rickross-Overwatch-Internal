#include "base.h"

c_tank tank_instance;
c_tank* g_tank = &tank_instance;

void c_tank::think() {
	this->block_think();
	
	if (local->object.hero == HERO_REINHARDT)
		this->rein_think(); 
}

void c_tank::block_think() {
	bool block = false;
	bool ignore_zen = false;
	
	// create time limit for shielding junk mine
	
	ow_object* best_obj = nullptr;
	bool eat = local->object.hero == HERO_DVA || local->object.hero == HERO_SIGMA;
	bool ult_eat_available = false;
	
	int best_cd = 0;
	
	for (int i = 0; i < g_state->_object_count; ++i) {
		ow_object* obj = &(g_state->_objects[i]);
		if (obj->type != OBJECT_PLAYER)
			continue;
		
		if (!g_state->is_alive(obj))
			continue;
		
		if (g_state->get_team(local->object.filter_bits) == g_state->get_team(obj->filter_bits))
			continue;
		
		if (obj->hero == HERO_ZEN) {
			ow_condition* orb = g_state->lookup_condition(obj, HASH("zen_orb"));
			if (orb == nullptr)
				continue;
			
			float orb_charge = orb->data.fl;
			if (orb_charge > 0.8f)
				block = true;
			
			if (orb_charge > 0.4f) {
				_zen_m2 = g_state->_curtime + 0.5f;
				ignore_zen = true;
			}
			
			continue;
		}
		
		if (local->object.hero == HERO_REINHARDT) {
			if (obj->hero == HERO_ORISA)
				continue;
		}
		
		ow_condition* shift = g_state->lookup_condition(obj, HASH("usage_shift"));
		ow_condition* e = g_state->lookup_condition(obj, HASH("usage_e"));
		ow_condition* m2 = g_state->lookup_condition(obj, HASH("usage_m2"));
		
		int shift_type = g_hero->get_shift_type(obj->hero);
		int e_type = g_hero->get_e_type(obj->hero);
		int m2_type = g_hero->get_m2_type(obj->hero);
		int ult_type = g_hero->get_ult_type(obj->hero);
		
		bool junk_time = false;
		bool sombra_time = false;
		
		int cd = 0;
		
		bool block_shift = false;
		bool block_e = false;
		bool block_m2 = false;
		
		if (shift_type != 0) {
			block_shift = true;
			if (eat && shift_type < CDTYPE_STUN_EAT)
				block_shift = false;
		}
		
		if (e_type != 0) {
			block_e = true;
			if (eat && e_type < CDTYPE_STUN_EAT)
				block_e = false;
		}
		
		if (m2_type != 0) {
			block_m2 = true;
			if (eat && m2_type < CDTYPE_STUN_EAT)
				block_m2 = false;
		}
		
		if (block_shift) {
			if (shift) {
				if (shift->data.u32 != 0) {
					cd = 1;
					junk_time = true;
				}
			}
		}
		
		if (block_e) {
			if (e) {
				if (e->data.u32 != 0) {
					cd = 2;
					sombra_time = true;
				}
			}
		}
		
		if (block_m2) {
			if (m2) {
				if (m2->data.u32 != 0)
					cd = 3;
			}
		}
		
		if (eat && ult_type != 0) {
			if (obj->ult_charge == 0.0f) {
				cd = 4;
			} else if (obj->ult_charge > 0.9f) {
				best_obj = nullptr;
				ult_eat_available = true;
			}
		}
		
		if (obj->hero == HERO_JUNK) {
			if (junk_time)
				_junk_shift += OW_TICK_INTERVAL;
			else
				_junk_shift = 0.0f;
		}
		
		if (obj->hero == HERO_SOMBRA) {
			if (sombra_time)
				_sombra_e += OW_TICK_INTERVAL;
			else
				_sombra_e = 0.0f;
		}
		
		if (local->object.hero == HERO_DVA) {
			if (obj->hero == HERO_ANA) {
				if (cd == 2) {
					ow_condition* cd_shift = g_state->lookup_condition(obj, HASH("cooldown_shift"));
					if (cd_shift) {
						float shift_time = cd_shift->data.fl;
						if (shift_time < 2.0f)
							continue;
					}
				}
			}
			
			if (obj->hero == HERO_ASHE) {
				if (cd == 2) {
					ow_condition* cd_shift = g_state->lookup_condition(obj, HASH("cooldown_shift"));
					if (cd_shift) {
						float shift_time = cd_shift->data.fl;
						if (shift_time < 2.0f)
							continue;
					}
				}
			}
		}
		
		if (ult_eat_available && cd != 4)
			cd = 0;
		
		{
			float len = (obj->model_center - local->vieworigin).length();
			if (local->object.hero == HERO_DVA)
				len -= g_hero_data[local->object.hero].attack2_range;
			else if (local->object.hero == HERO_SIGMA)
				len -= g_hero_data[local->object.hero].ability1_range;
			
			if (cd == 1) {
				float dist = g_hero_data[obj->hero].ability1_range + g_hero_data[obj->hero].ability2_splash_radius;
				if (len > dist)
					cd = 0;
			} else if (cd == 2) {
				float dist = g_hero_data[obj->hero].ability2_range + g_hero_data[obj->hero].ability2_splash_radius;
				if (len > dist)
					cd = 0;
			} else if (cd == 3) {
				float dist = g_hero_data[obj->hero].attack2_range + g_hero_data[obj->hero].attack2_splash_radius;
				if (len > dist)
					cd = 0;
			} else if (cd == 4) {
				float dist = g_hero_data[obj->hero].ult_range + g_hero_data[obj->hero].ult_splash_radius;
				if (len > dist)
					cd = 0;
			}
		}
		
		if (eat == true) {
			/*Vector fwd = local->viewforward;
			Vector dt = (obj->model_center - local->vieworigin).normal();
			float dist = fwd.dot(dt);
			
			if (dist < 0.342f)
				cd = 0;*/
		} else {
			Vector fwd = obj->forward;
			Vector dt = (local->vieworigin - obj->model_center).normal();
			float dist = fwd.dot(dt);
			
			if (dist < 0.0f)
				cd = 0;
		}
		
		if (cd != 0) {
			best_obj = obj;
			best_cd = cd;
			
			if (cd == 4)
				break;
		}
	}
	
	// sigma crouch to eat stuff pointed at ground
	
	if (best_obj == nullptr)
		return;
	
	if (g_cvars->hero_block.value == 0)
		return;
	
	if (best_cd != 4 && g_cvars->hero_block_key.value != -1) {
		if (g_hero->get_aim_key(g_cvars->hero_block_key.value, 0) == -1)
			return;
	}
	
	block = true;
	
	if (best_obj->hero == HERO_JUNK) {
		if (_junk_shift > 1.0f)
			block = false;
	}
	
	if (best_obj->hero == HERO_SOMBRA) {
		if (_sombra_e > 1.0f)
			block = false;
	}
	
	if (ignore_zen == false) {
		if (_zen_m2 - g_state->_curtime > 0.0f)
			block = true;
	}
	
	if (block) {
		pseudo_cmd* ucmd = (pseudo_cmd*)&(g_ow->_cmd);
		
		if (local->object.hero == HERO_SIGMA) {
			ucmd->buttons |= IN_ABILITY1;
		} else {
			ucmd->buttons &= ~IN_ATTACK;
			ucmd->buttons |= IN_ATTACK2;
		}
	}
}

void c_tank::rein_think() {
	ow_object* best_obj = nullptr;
	
	for (int i = 0; i < g_state->_object_count; ++i) {
		ow_object* obj = &(g_state->_objects[i]);
		if (obj->type != OBJECT_PLAYER)
			continue;
		
		if (!g_state->is_alive(obj))
			continue;
		
		if (g_state->get_team(local->object.filter_bits) == g_state->get_team(obj->filter_bits))
			continue;
		
		if (obj->hero != HERO_REINHARDT)
			continue;
		
		best_obj = obj;
		break;
	}
	
	if (best_obj == nullptr)
		return;
	
	bool block = false;
	bool pin = false;
	
	if (best_obj->ult_charge == 0.0f) {
		if (_rein_ult_cast == false)
			_rein_ult = g_state->_curtime + 1.0f;
		
		_rein_ult_cast = true;
	} else {
		_rein_ult_cast = false;
	}
	
	if (_rein_ult > g_state->_curtime)
		block = true;
	
	float dist = (best_obj->model_center - local->vieworigin).length();
	if (dist < 15.0f) {
		ow_condition* shift = g_state->lookup_condition(best_obj, HASH("usage_shift"));
		if (shift) {
			if (shift->data.u32 != 0)
				pin = true;
		}
	}
	
	pseudo_cmd* ucmd = (pseudo_cmd*)&(g_ow->_cmd);
	
	if (block) {
		ucmd->buttons &= ~IN_ATTACK;
		ucmd->buttons |= IN_ATTACK2;
		return;
	}
	
	if (pin)
		ucmd->buttons |= IN_ABILITY1;
}