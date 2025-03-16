#include "base.h"

c_console console_instance;
c_console* g_console = &console_instance;

void c_console::paint() {
	for (int i = 0; i < MAX_DEBUG_MSGS; ++i) {
		float width = 5.0f + (450.0f * (float)(i / 30));
		float height = 15.0f + (float)(i % 30) * 15.0f;
		
		debug_msg* msg = &_msgs[i];
		if (msg->time == 0.0f)
			continue;
		
		int r = msg->color == CONCLR_RED ? 255 : 0;
		int g = msg->color == CONCLR_GREEN ? 255 : 0;
		int b = 0;
		
		g_draw->draw_text_a(msg->text, width, height, r, g, b);
		
		if (msg->time > 0.0f && g_input->_sample_time < 500) {
			msg->time -= (float)(g_input->_sample_time) / 1000.0f;
			if (msg->time < 0.0f)
				msg->time = 0.0f;
		}
	}
}

void c_console::print(char* text, float time) {
	this->print_ex(text, time, CONCLR_GREEN);
}

void c_console::error(char* text, float time) {
	this->print_ex(text, time, CONCLR_RED);
}

void c_console::print_ex(char* text, float time, int color) {
	if (strlen(text) >= MAX_TEXT_SIZE)
		return;
	
	for (int i = 0; i < MAX_DEBUG_MSGS; ++i) {
		debug_msg* msg = &_msgs[i];
		if (msg->time != 0.0f)
			continue;
		
		strcpy(msg->text, text);
		
		msg->time = time;
		
		msg->color = color;
		
		break;
	}
}