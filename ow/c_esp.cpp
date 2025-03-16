#include "base.h"

c_esp esp_instance;
c_esp* g_esp = &esp_instance;

void c_esp::draw_line(Vector start, Vector end, int r, int g, int b) {
	float sx = 0.0f;
	float sy = 0.0f;
	float ex = 0.0f;
	float ey = 0.0f;
	
	if (!this->world_to_screen(start, &sx, &sy))
		return;
	
	if (!this->world_to_screen(end, &ex, &ey))
		return;
	
	g_dx11->draw_line(sx, sy, ex, ey, r, g, b);
}

void c_esp::draw_capsule(Vector capsule_min, Vector capsule_max, float radius, int r, int g, int b) {
	Vector min = capsule_min;
	Vector max = capsule_max;
	
	Vector mind = min;
	mind[F_Z] += -radius;
	
	Vector maxu = max;
	maxu[F_Z] += radius;
	
	Vector minl = min;
	minl[F_X] += radius;
	
	Vector minr = min;
	minr[F_X] += -radius;
	
	Vector minf = min;
	minf[F_Y] += radius;
	
	Vector minb = min;
	minb[F_Y] += -radius;
	
	Vector maxl = max;
	maxl[F_X] += radius;
	
	Vector maxr = max;
	maxr[F_X] += -radius;
	
	Vector maxf = max;
	maxf[F_Y] += radius;
	
	Vector maxb = max;
	maxb[F_Y] += -radius;
	
	Vector dl = min;
	dl[F_X] += radius * 0.707f;
	dl[F_Z] += -radius * 0.707f;
	
	Vector dr = min;
	dr[F_X] += -radius * 0.707f;
	dr[F_Z] += -radius * 0.707f;
	
	Vector df = min;
	df[F_Y] += radius * 0.707f;
	df[F_Z] += -radius * 0.707f;
	
	Vector db = min;
	db[F_Y] += -radius * 0.707f;
	db[F_Z] += -radius * 0.707f;
	
	Vector ul = max;
	ul[F_X] += radius * 0.707f;
	ul[F_Z] += radius * 0.707f;
	
	Vector ur = max;
	ur[F_X] += -radius * 0.707f;
	ur[F_Z] += radius * 0.707f;
	
	Vector uf = max;
	uf[F_Y] += radius * 0.707f;
	uf[F_Z] += radius * 0.707f;
	
	Vector ub = max;
	ub[F_Y] += -radius * 0.707f;
	ub[F_Z] += radius * 0.707f;
	
	this->draw_line(mind, df, r, g, b);
	this->draw_line(df, minf, r, g, b);
	
	this->draw_line(mind, db, r, g, b);
	this->draw_line(db, minb, r, g, b);
	
	this->draw_line(mind, dl, r, g, b);
	this->draw_line(dl, minl, r, g, b);
	
	this->draw_line(mind, dr, r, g, b);
	this->draw_line(dr, minr, r, g, b);
	
	this->draw_line(maxu, uf, r, g, b);
	this->draw_line(uf, maxf, r, g, b);
	
	this->draw_line(maxu, ub, r, g, b);
	this->draw_line(ub, maxb, r, g, b);
	
	this->draw_line(maxu, ul, r, g, b);
	this->draw_line(ul, maxl, r, g, b);
	
	this->draw_line(maxu, ur, r, g, b);
	this->draw_line(ur, maxr, r, g, b);
	
	this->draw_line(minl, maxl, r, g, b);
	this->draw_line(minr, maxr, r, g, b);
	this->draw_line(minf, maxf, r, g, b);
	this->draw_line(minb, maxb, r, g, b);
}

/*
	body offset = 0.0f, 1.3f, 0.0f
	head offset = 0.0f, 2.0f, 0.0f
*/

int BoxFaceIndices[6][4] =
{
	{ 0, 4, 6, 2 }, // -x
	{ 5, 1, 3, 7 }, // +x
	{ 0, 1, 5, 4 }, // -y
	{ 2, 6, 7, 3 }, // +y
	{ 0, 2, 3, 1 },	// -z
	{ 4, 5, 7, 6 }	// +z
};

void c_esp::draw_box(ow_object* obj, int r, int g, int b) {
	Vector out[8];
	
	float dt = obj->model_max.y - obj->model_min.y;
	
	float sz = 0.45f;
	Vector org = obj->position_root;
	Vector min = Vector(-sz, -dt, -sz);
	Vector max = Vector(sz, 0.0f, sz);
	
	float yaw = obj->forward.yaw(); //+ 54.0f; // ?? weird offset
	NormalizeAngle(&yaw);
	
	for (int n = 0; n < 8; ++n) {
		Vector pos;

		pos.x = (n & 0x1) ? max.x : min.x;
		pos.y = (n & 0x2) ? max.y : min.y;
		pos.z = (n & 0x4) ? max.z : min.z;

		matrix3x4 matrix;
		matrix.init(org, Vector(yaw, 0.0f, 0.0f)); // x = yaw

		out[n] = matrix.transform(pos);
	}
	
	for (int n = 0; n < 6; ++n) {
		int* index = BoxFaceIndices[n];
		
		for (int j = 0; j < 4; ++j) {
			Vector start = out[index[j]];
			Vector end = out[index[(j == 3) ? 0 : j + 1]];
			
			this->draw_line(start, end, r, g, b);
		}
	}
}

void c_esp::debug_esp(ow_object* obj) {
	char str[64];
	
	{
		float x = 0.0f;
		float y = 0.0f;
		
		if (this->world_to_screen(obj->position, &x, &y)) {
			ow_condition* shift = g_state->lookup_condition(obj, HASH("usage_shift"));
			
			if (shift) {
				uint32_t used = shift->data.u32;
				I(wsprintfA)(str, "shift %i", used);
				g_draw->draw_text_a(str, x, y, used?0:255, used?255:0, 0);
			}
			
			ow_condition* e = g_state->lookup_condition(obj, HASH("usage_e"));
			
			if (e) {
				uint32_t used = e->data.u32;
				I(wsprintfA)(str, "e %i", e->data.u32);
				g_draw->draw_text_a(str, x, y + 15.0f, used?0:255, used?255:0, 0);
			}
			
			ow_condition* m2 = g_state->lookup_condition(obj, HASH("usage_m2"));
			
			if (m2) {
				bool ac = m2->data.u32 ? 1 : 0;
				
				I(wsprintfA)(str, "m2 %i", m2->data.u32);
				g_draw->draw_text_a(str, x, y + 30.0f, ac?0:255, ac?255:0, 0);
			}
		}
		
		//I(wsprintfA)(str, "test %i", (int)((obj->position_root - local->vieworigin).length()));
	//	g_draw->draw_text_a(str, x, y + 15.0f, 0, 255, 0);
	}
	
	
	return;
	
	if (obj->hero == HERO_TRAININGBOT1 || obj->hero == HERO_TRAININGBOT4)
		return;
	
	matrix_t maty;
	OW_InitY(obj->rotation.x, maty);
	
	for (int i = 0; i < ENGINE_HITBOX_MAX; ++i) {
		Vector offset = obj->hitbox_offset[i];
		Vector base = {};
		
		OW_Transform(offset, maty, &base);
		base += obj->position_root;
		
		Vector trans = base;
		if (obj->hitbox_id[i] == 0)
			continue;
			
	//	this->draw_capsule(trans, trans, 0.180f, 0, 255, 0);
		//if (this->world_to_screen(trans, &x, &y)) {
			Vector vieworg = _local->vieworigin;
			Vector viewforward = _viewforward;
			float size = obj->hitbox_radius[i] * g_trig->_data.hitbox_scale;
			bool test = RayIntersectSphere(vieworg, trans, viewforward, size * size, nullptr);
			
			int r = 255;
			int g = 0;
			int b = 0;
			if (test) {
				r = 0;
				g = 255;
				b = 0;
			}
				
			this->draw_capsule(trans, trans, size, r, g, b);
			
			float x = 0.0f;
			float y = 0.0f;
			
			if (!this->world_to_screen(trans, &x, &y))
				continue;
			
			Quat q = obj->hitbox_test[i];
			matrix3x4 m = {};
			
			QuaternionMatrix(q, &m);
			
			Vector v = m.get_column(0);
			
			I(wsprintfA)(str, "%i %i %i %i", (int)(q.x * 1000.0f), (int)(q.y * 1000.0f), (int)(q.z * 1000.0f), (int)(q.w * 1000.0f));
			g_draw->draw_text_a(str, x, y, 0, 255, 0);
			//g_draw->draw_text_a("*", x, y, r, g, b);
		//}
	}
	
	{
		Vector forward = obj->rotation;
		Vector angles = Vector(forward.pitch(), forward.yaw(), 0.0f);
		Vector new_forward = (angles).forward();
		new_forward = new_forward.normal();
		
		Vector ray_start = obj->position_root;
		Vector ray_end = ray_start + (forward * 10.0f);
		
		this->draw_line(ray_start, ray_end, 0, 255, 0);
	}
}

void c_esp::calc_health_rgb(ow_object* obj, int* r, int* g, int* b) {
	float max_hp = std::max(obj->max_health, 1.0f);
	
	float mul = (float)(obj->health) / (float)(max_hp);
	if (mul > 1.0f)
		mul = 1.0f;
	
	*r = (int)(0.0f + (510.0f * (1.0f - mul)));
	if (*r > 255)
		*r = 255;
	
	*g = (int)(510.0f - (510.0f * (1.0f - mul)));
	if (*g > 255)
		*g = 255;
	
	if (obj->max_armor > 0.0f && obj->armor > 0.0f) {
		*r = 255;
		*g = 180;
		*b = 0;
	}
	
	if (obj->max_shields > 0.0f && obj->shields > 0.0f) {
		*r = 36;
		*g = 192;
		*b = 255;
	}
}

void c_esp::draw_osd(ow_object* obj) {
	char str[64];
	
	float x = 15.0f;
	bool alive = g_state->is_alive(obj);
	
	int r = 16;
	int g = 16;
	int b = 16;
	float x_offset = 0.0f;
	
	{
		if (alive) {
			uint32_t color = g_hero_data[obj->hero].color;
			r = (color >> 16) & 0xFF;
			g = (color >> 8) & 0xFF;
			b = color & 0xFF;
		}
		
		I(wsprintfA)(str, "%s", g_hero_data[obj->hero].name);
		x += g_draw->draw_text_a(str, (5.0f * x_offset) + x, g_draw->_height * 0.25f + _osd_offset_y, r, g, b);
		x_offset += 1.0f;
	}
	
	/*{
		if (alive)
			this->calc_health_rgb(obj, &r, &g, &b);
		
		I(wsprintfA)(str, "%i", (int)(obj->total_hp));
		x += g_draw->draw_text_a(str, (5.0f * x_offset) + x, g_draw->_height * 0.25f + _osd_offset_y, r, g, b);
		x_offset += 1.0f;
	}
	
	{
		ow_condition* cond = g_state->lookup_condition(obj, HASH("cooldown_shift"));
		if (cond) {
			float shift_time = cond->data.fl;
			
			int r = 128;
			int g = 128;
			int b = 128;
			
			if (shift_time == 0.0f) {
				r = 36;
				g = 255;
				b = 255;
			}
			
			if (shift_time > 0.0f)
				I(wsprintfA)(str, "shift %i", (int)(shift_time + 1.0f));
			else
				I(wsprintfA)(str, "shift");
			
			x += g_draw->draw_text_a(str, (5.0f * x_offset) + x, g_draw->_height * 0.25f + _osd_offset_y, r, g, b);
			x_offset += 1.0f;
		}
		
		cond = g_state->lookup_condition(obj, HASH("cooldown_e"));
		if (cond) {
			float e_time = cond->data.fl;
			
			int r = 128;
			int g = 128;
			int b = 128;
			
			if (e_time == 0.0f) {
				r = 36;
				g = 255;
				b = 255;
			}
			
			if (e_time > 0.0f)
				I(wsprintfA)(str, "e %i", (int)(e_time + 1.0f));
			else
				I(wsprintfA)(str, "e");
			
			x += g_draw->draw_text_a(str, (5.0f * x_offset) + x, g_draw->_height * 0.25f + _osd_offset_y, r, g, b);
			x_offset += 1.0f;
		}
	}*/
	
	{
		if (alive) {
			r = 128;
			g = 128;
			b = 128;
		}
		
		if (obj->ult_charge == 100.0f) {
			r = 36;
			g = 255;
			b = 255;
		}
		
		I(wsprintfA)(str, "%i%%", (int)(obj->ult_charge));
		x += g_draw->draw_text_a(str, (5.0f * x_offset) + x, g_draw->_height * 0.25f + _osd_offset_y, r, g, b);
		x_offset += 1.0f;
	}
	
	_osd_offset_y += 20.0f;
}

void c_esp::draw_player(ow_object* obj) {
	if (_local->team != 3) {
		if (_local->team == g_state->get_team(obj->filter_bits))
			return;
	}
	
	if (g_cvars_misc->esp_osd.value)
		this->draw_osd(obj);
	
	if (g_cvars_misc->esp_active.value == 0)
		return;
	
	if (!g_state->is_alive(obj))
		return;
	
	Vector min = obj->model_min;
	Vector max = obj->model_max;
	Vector center = obj->model_center;
	
	if (!this->world_to_screen(min, &(g_draw->_text_min_x), &(g_draw->_text_min_y))
		|| !this->world_to_screen(max, &(g_draw->_text_max_x), &(g_draw->_text_max_y))
		|| !this->world_to_screen(center, &(g_draw->_text_center_x), &(g_draw->_text_center_y))) {
		return;
	}
	
	if (g_cvars_misc->esp_box.value) {
		
		int r = 0;
		int g = 255;
		int b = 0;
		
		if (obj->is_visible == false) {
			r = 0;
			g = 117;
			b = 225;
		}
		
		//this->calc_health_rgb(obj, &r, &g, &b);
		
		if (g_cvars_misc->esp_box.value == 1)
			g_draw->draw_esp_box(r, g, b);
		else if (g_cvars_misc->esp_box.value == 2)
			g_draw->draw_esp_box_corners(r, g, b, 2.0f);
		else
			this->draw_box(obj, r, g, b);
		
		Vector start = obj->model_max;
		Vector end = start + (obj->forward * 10.0f);
		
		//this->draw_line(start, end, 0, 255, 0);
	}
	
	if (g_cvars_misc->esp_health_bar.value)
		g_draw->draw_esp_bar(g_cvars_misc->esp_health_bar.value, (float)(g_cvars_misc->esp_health_bar_frac.value), (int)(obj->total_hp), (int)(obj->total_max_hp));
	
	char str[128];
	
	if (g_cvars_misc->esp_class.value) {
		//uint32_t color = g_hero_data[obj->hero].color;
		//int r = (color >> 16) & 0xFF;
		//int g = (color >> 8) & 0xFF;
		//int b = color & 0xFF;
		
		int r = 0;
		int g = 255;
		int b = 0;
		
		if (obj->is_visible == false) {
			r = 255;
			g = 0;
			b = 0;
		}
		
		I(wsprintfA)(str, "%s", g_hero_data[obj->hero].name);
		g_draw->draw_esp_text(g_cvars_misc->esp_class.value, str, r, g, b);
	}
	
	if (g_cvars_misc->esp_health.value) {
		int r = 0;
		int g = 0;
		int b = 0;
		
		this->calc_health_rgb(obj, &r, &g, &b);
		
		I(wsprintfA)(str, "%i HP", (int)(obj->total_hp));
		g_draw->draw_esp_text(g_cvars_misc->esp_health.value, str, r, g, b);
	}
	
	if (g_cvars_misc->esp_cd.value) {
		ow_condition* cond = g_state->lookup_condition(obj, HASH("cooldown_shift"));
		if (cond) {
			float shift_time = cond->data.fl;
			
			int r = 128;
			int g = 128;
			int b = 128;
			
			if (shift_time == 0.0f) {
				r = 36;
				g = 255;
				b = 255;
			}
			
			if (shift_time > 0.0f)
				I(wsprintfA)(str, "shift %i", (int)(shift_time + 1.0f));
			else
				I(wsprintfA)(str, "shift");
			
			g_draw->draw_esp_text(g_cvars_misc->esp_cd.value, str, r, g, b);
		}
		
		cond = g_state->lookup_condition(obj, HASH("cooldown_e"));
		if (cond) {
			float e_time = cond->data.fl;
			
			int r = 128;
			int g = 128;
			int b = 128;
			
			if (e_time == 0.0f) {
				r = 36;
				g = 255;
				b = 255;
			}
			
			if (e_time > 0.0f)
				I(wsprintfA)(str, "e %i", (int)(e_time + 1.0f));
			else
				I(wsprintfA)(str, "e");
			
			g_draw->draw_esp_text(g_cvars_misc->esp_cd.value, str, r, g, b);
		}
	}
	
	if (g_cvars_misc->esp_ult.value) {
		int r = 128;
		int g = 128;
		int b = 128;
		
		if (obj->ult_charge == 100.0f) {
			r = 36;
			g = 255;
			b = 255;
		}
		
		I(wsprintfA)(str, "%i%% ULT", (int)(obj->ult_charge));
		g_draw->draw_esp_text(g_cvars_misc->esp_ult.value, str, r, g, b);
	}
	
	
	
	/*float attack_speed = g_hero_data[_local->object.hero].attack_speed;
	
	if (attack_speed > 0.0f) {
		Vector aimspot = obj->bones[HITBOX_HEAD];
		
		float x = 0.0f;
		float y = 0.0f;
		
		float pred_dist = (aimspot - _local->vieworigin).length();
		int ticks = (int)(((pred_dist / attack_speed) / OW_TICK_INTERVAL) + 0.5f);
		
		Vector delta = Vector(0.0f, 0.0f, 0.0f);
		g_anim->calculate_delta(obj, ticks, &delta);
		
		if (this->world_to_screen(aimspot + delta, &x, &y))
			g_draw->draw_text_a("*", x, y, 255, 255, 0);
	}*/
	
	/*if (g_menu->_drawing && g_cvars->aim_fov.value > 0) {
		float rad = 0.4f * ((float)(g_cvars->aim_fov.value) / 100.0f);
		Vector min = obj->model_min + Vector(0.0f, 0.4f, 0.0f);
		Vector max = obj->model_max - Vector(0.0f, 0.4f, 0.0f);
		
		this->draw_capsule(min, max, rad, 0, 255, 0);
	}*/
	
	if (g_cvars_misc->esp_debug.value) {
		this->debug_esp(obj);
	}
	
	if (g_cvars_misc->esp_draw_fov.value) { //esp_draw_fov
		
		float rad = 0.4f * ((float)(g_cvars->aim_fov.value) / 100.0f);
		Vector min = obj->model_min + Vector(0.0f, 0.4f, 0.0f);
		Vector max = obj->model_max - Vector(0.0f, 0.4f, 0.0f);
		
		this->draw_capsule(min, max, rad, 255, 255, 255);
		
	}
}

void c_esp::draw_dynamite(ow_object* obj) {
	if (!g_state->is_alive(obj))
		return;
		
//	if (g_state->get_team(_local->object.filter_bits) == g_state->get_team(obj->filter_bits))
	//	return;
	
	if (g_state->is_object_destroyed(obj->filter_bits))
		return;
	
	Vector center = obj->position;
	
	if (!this->world_to_screen(center, &(g_draw->_text_center_x), &(g_draw->_text_center_y)))
		return;
	
	int r = 255;
	int g = 255;
	int b = 0;
	
	if (obj->is_visible == false) {
		r = 128;
		g = 128;
		b = 128;
	}
	
	//char str[64];
	//I(wsprintfA)(str, "test %p", obj->pool_type);
	g_draw->draw_esp_text(5, "Dynamite", r, g, b);
}

void c_esp::draw_equipment(ow_object* obj) {
	if (!g_state->is_alive(obj))
		return;
	
	if (g_state->is_object_destroyed(obj->filter_bits))
		return;
	
	if (g_state->get_team(_local->object.filter_bits) == g_state->get_team(obj->filter_bits))
		return;
	
	Vector center = obj->position;
	
	if (!this->world_to_screen(center, &(g_draw->_text_center_x), &(g_draw->_text_center_y)))
		return;
	
	switch (obj->pool_type) {
		case POOLID_TRANSLOCATOR: {
			g_draw->draw_esp_text(5, "Translocator", 255, 255, 0);
			break;
		}
		case POOLID_TPENTER:
		case POOLID_TPEXIT: {
			g_draw->draw_esp_text(5, "Teleporter", 255, 255, 0);
			break;
		}
		case POOLID_WIDOWMINE: {
			g_draw->draw_esp_text(5, "Widow Mine", 255, 255, 0);
			break;
		}
		case POOLID_LAMP: {
			g_draw->draw_esp_text(5, "Lamp", 255, 255, 0);
			break;
		}
	}
	
	if (obj->pool_type != POOLID_TRANSLOCATOR && obj->pool_type != POOLID_WIDOWMINE) {
		char str[64];
		int r = 0;
		int g = 0;
		int b = 0;
		
		this->calc_health_rgb(obj, &r, &g, &b);
		
		I(wsprintfA)(str, "%i HP", (int)(obj->total_hp));
		g_draw->draw_esp_text(5, str, r, g, b);
	}
}

// esp breaks at very high fps values, drawing esp before the game samples all info
// need to save a state of ow_objects for drawing
void c_esp::paint() {
	_local = &_local_data;
	if (g_ow->_gm == 0)
		return;
	
	if (g_ow->_thread_lock == false) {
		memcpy(&_local_data, &(g_local->_local_data), sizeof(_local_data));
		memcpy(_objects, g_state->_objects, sizeof(_objects));
		_object_count = g_state->_object_count;
	}
	
	if (!g_state->is_alive(&(_local->object)))
		return;
	
	{
		float calc1 = tanf(_fov * (PI / 360.0f));
		float calc2 = atan2f(g_draw->_height, g_draw->_width / calc1) * (360.0f / PI);
		float calc3 = tanf(calc2 * (PI / 360.0f));
		
		_calc1 = calc1;
		_calc3 = calc3;
	}
	
	int r = 0;//g_ow->_test_result != 0 ? 255 : 0;
	int g = 255;
	int b = 0;//g_ow->_test_result == 0 ? 255 : 0;
	
	if (g_cvars_misc->esp_debug.value != 0) {
		pseudo_ray_t* ray = &(_local->view_ray);
		
		char str[64];
		//I(wsprintfA)(str, "aim: %i %i %i", (int)(_local->object.velocity.x * 1000.0f), (int)(_local->object.velocity.y * 1000.0f), (int)(_local->object.velocity.z * 1000.0f));
		//I(wsprintfA)(str, "limit: %x %i", _local->ability_limit, (int)(_local->hog_hook)); // hanzo and echo have these set 5a3 storm arrow start, 5a1 active
		float boss = 0;
		ow_condition* cond = g_state->lookup_condition(&(_local->object), HASH("zen_ammo"));
		if (cond)
			boss = cond->data.fl;
		uint16_t test = 0;
		ow_condition* cond2 = g_state->lookup_condition(&(_local->object), HASH("usage_m2"));
		if (cond2)
			test = cond2->data.u32;
		
		//I(wsprintfA)(str, "t: %i %i", g_anim->_tick_force_ms, g_anim->_tick_limit_ms);
		//I(wsprintfA)(str, "t: %i", (int)(local->fire_time * 1000.0f));
		I(wsprintfA)(str, "t: %i", (int)(local->move_speed_mult * 1000.0f));
	//	I(wsprintfA)(str, "t: %i %i %i", (int)(g_aim->_smooth_base.x * 1000.0f), (int)(g_aim->_smooth_base.y * 1000.0f), (int)(g_aim->_smooth_base.z * 1000.0f));
		//I(wsprintfA)(str, "ray: %i %x %i %i", (int)(ray->fraction * 100.0f), ray->hitbox, ray->head_hit?1:0, (int)(ray->length));
		g_draw->draw_text_a(str, g_draw->_width / 2.0f, g_draw->_height / 2.0f, 255, 255, 0);
		
		//  Vector(42.799, 3.430, 14.501); origin
		//g_state->_black = Vector(-57.118, -0.641, -72.978);
		
		float x = 0.0f;
		float y = 0.0f;
		this->world_to_screen(g_state->_black, &x, &y);
		
		g_draw->draw_text_a("X", x, y, 255, 255, 0);
	}
	_osd_offset_y = 0.0f;
	
	if (g_cvars_misc->esp_osd.value) {
		//g_draw->draw_text_a("Hero, Ult%", 5.0f, g_draw->_height * 0.25f, 0, 255, 0);
		_osd_offset_y += 20.0f;
	}
	
	for (int i = 0; i < _object_count; ++i) {
		ow_object* obj = &(_objects[i]);
		if (obj->type == OBJECT_INVALID)
			continue;
		
		g_draw->reset_text();
		
		switch (obj->type) {
			case OBJECT_PLAYER: {
				this->draw_player(obj);
				break;
			}
			case OBJECT_DYNAMITE: {
				this->draw_dynamite(obj);
				break;
			}
			case OBJECT_EQUIPMENT: {
				this->draw_equipment(obj);
				break;
			}
		}
	}
}

bool c_esp::world_to_screen(Vector pos, float* x, float* y) {
	Vector vieworg = _local->vieworigin;
	Vector viewforward = _viewforward;
	Vector viewang = Vector(viewforward.pitch(), viewforward.yaw(), 0.0f);
	
	Vector dt = pos - vieworg;
	
	float cx = (g_draw->get_width() * 0.5f);
	float cy = (g_draw->get_height() * 0.5f);
	
	Vector view_normal = (viewang + Vector(0.0f, 180.0f, 0.0f));
	if (view_normal.y > 360.0f)
		view_normal.y -= 360.0f;
	
	Vector fwd = viewforward;
	Vector right = view_normal.right();
	Vector up = viewang.up();
	
	Vector transform = Vector(dt.dot(right), dt.dot(up), dt.dot(fwd));
	
	if (transform.z < 0.001f)
		return false;
    
	*x = cx * (1.0f - (transform.x / _calc1 / transform.z));
	*y = cy * (1.0f - (transform.y / _calc3 / transform.z));
	
	return true;
}