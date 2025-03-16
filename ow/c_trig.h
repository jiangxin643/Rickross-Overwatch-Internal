enum {
	TRIG_TYPE_NONE = 0,
	TRIG_TYPE_PRIMARY = 1,
	TRIG_TYPE_SECONDARY = 2,
	TRIG_TYPE_ZEN = 3,
};

struct trig_data {
	int type;
	
	int hitbox;
	
	float fire_delay;
	float wait_time;
	
	bool head_only;
	float hitbox_scale;
	float min_accuracy;
	
	bool pressing;
	
	bool history;
	bool history_pred;
	
	bool objects;
};

class c_trig {
public:
	bool is_trigger_target(ow_object* obj);
	bool get_target();
	
	void think();
	
	bool update();
	
	trig_data _data;
	
	float _press_time;
	float _press_time_diff;
	
	bool _first_fired;
};

extern c_trig* g_trig;