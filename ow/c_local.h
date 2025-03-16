struct ow_local {
	ow_object object;
	
	uint64_t ray_cmp;
	
	Vector vieworigin;
	Vector viewforward;
	Vector viewangles;
	
	Vector velocity;
	
	Vector last_viewforward;
	
	int team;
	
	float ammo_percent;
	float move_speed_mult;
	float fire_time;
	float sniper_charge;
	float genji_primary;
	float cur_weapon;
	float healing_beam;
	float hog_hook;
	Vector hog_hook_pos;
	float cross_fade;
	float zen_orbs;
	
	bool reloading;
	bool firing;
	bool fire_ready;
	bool ability_locked;
	uint16_t ability_limit;
	float projectile_speed;
	bool melee;
	float melee_range;
	bool shotgun;
	
	bool throwing_hook;
	
	pseudo_ray_t view_ray;
};

class c_local {
public:
	void accelerate(Vector wishdir, float wishspeed, float accel);
	bool update();
	uint64_t lookup_component(uint64_t id);
	
	ow_local _local_data;
	float _hook_attach_time;
	float _last_sniper_charge;
};

extern c_local* g_local;
extern ow_local* local;