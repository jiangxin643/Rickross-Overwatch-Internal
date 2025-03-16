#pragma once

class c_esp {
public:
	void draw_line(Vector start, Vector end, int r, int g, int b);
	void draw_capsule(Vector capsule_min, Vector capsule_max, float radius, int r, int g, int b);
	void draw_box(ow_object* obj, int r, int g, int b);
	
	void debug_esp(ow_object* obj);
	
	void calc_health_rgb(ow_object* obj, int* r, int* g, int* b);
	
	void draw_osd(ow_object* obj);
	void draw_player(ow_object* obj);
	void draw_dynamite(ow_object* obj);
	void draw_equipment(ow_object* obj);
	
	void paint();
	
	bool world_to_screen(Vector pos, float* x, float* y);
	
	float _fov;
	
	float _calc1;
	float _calc3;
	
	float _osd_offset_y;
	
	Vector _viewforward;
	
	ow_local* _local;
	ow_local _local_data;
	ow_object _objects[MAX_OBJECTS];
	int _object_count;
};

extern c_esp* g_esp;