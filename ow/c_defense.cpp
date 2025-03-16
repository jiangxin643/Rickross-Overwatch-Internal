#include "base.h"

c_defense defense_instance;
c_defense* g_defense = &defense_instance;

void c_defense::cree_think() {
	pseudo_cmd* ucmd = (pseudo_cmd*)&(g_ow->_cmd);
}

void c_defense::cree_auto_noon() {
	if (!_noon_started) {
		_noon_time = g_state->_curtime + 0.09f + 0.2f;
		_noon_started = true;
	}
	
	float damage = 0.0f;
	
	{
		float delta = g_state->_curtime - _noon_time;
		
		float delta1 = delta;
		if (delta1 < 0.0f)
			delta1 = 0.0f;
		else if (delta1 > 0.8f)
			delta1 = 0.8f;
		
		float delta2 = delta - 0.8f;
		if (delta2 < 0.0f)
			delta2 = 0.0f;
		else if (delta2 > 0.5f)
			delta2 = 0.5f;
		
		float delta3 = delta - 1.3f;
		if (delta3 < 0.0f)
			delta3 = 0.0f;
		else if (delta3 > 4.7f)
			delta3 = 4.7f;
		
		damage = (delta1 * 100.0f) + (delta2 * 275.0f) + (delta3 * 550.0f);
		_noon_damage = damage;
	}
	
	Vector fwd = local->viewforward;
	
	for (int i = 0; i < g_state->_object_count; ++i) {
		ow_object* obj = &(g_state->_objects[i]);
		
		if (obj->health < 1.0f)
			continue;
		
		Vector org = obj->position;
		Vector dt = (org - local->vieworigin).normal();
		
		float diff = fwd.dot(dt);
		if (diff < 0.707f)
			continue;
		
		if (damage >= obj->health) {
			g_ow->_cmd.buttons |= IN_ATTACK;
			break;
		}
	}
}