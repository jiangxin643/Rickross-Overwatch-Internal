#pragma once

enum {
	CDTYPE_NONE = 0,
	CDTYPE_STUN = 1,
	CDTYPE_CC = 2,
	CDTYPE_DAMAGE = 3,
	CDTYPE_STUN_EAT = 4,
	CDTYPE_CC_EAT = 5,
	CDTYPE_DAMAGE_EAT = 6,
	CDTYPE_ULT_EAT = 7,
};

class c_hero {
public:
	void test();
	
	void think();
	
	int get_aim_key(int value, int post_time);
	void aim_setup();
	void post_aim_setup();
	
	void aim_think();
	
	void trig_think();
	
	void auto_melee();
	
	void zarya_super_jump();
	
	void fire_time_think();
	
	int get_shift_type(int hero);
	int get_e_type(int hero);
	int get_m2_type(int hero);
	int get_ult_type(int hero);
	
	bool _beasd;
	Vector _beasd_dir;
	float _beasd_dist;
	
	int _jump_time;
};

extern c_hero* g_hero;