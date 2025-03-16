#pragma once

#define MAX_DEBUG_MSGS 64
#define MAX_TEXT_SIZE 128

enum {
	CONCLR_GREEN,
	CONCLR_RED
};

struct debug_msg {
	char text[MAX_TEXT_SIZE];
	float time;
	int color;
};

class c_console {
public:
	void paint();
	
	void print(char* text, float time);
	void error(char* text, float time);
	void print_ex(char* text, float time, int color);
	
	debug_msg _msgs[MAX_DEBUG_MSGS];
};

extern c_console* g_console;