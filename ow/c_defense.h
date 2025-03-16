#pragma once

class c_defense {
public:
	void cree_think();
	void cree_auto_noon();
	
	bool _noon_started;
	float _noon_time;
	float _noon_damage;
};

extern c_defense* g_defense;