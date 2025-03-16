#pragma once

#define MAX_INPUTS 8

#define MAX_USER_CMDS 30

#define MAX_MOUSE_KEYS 3

static int mouse_flags[MAX_MOUSE_KEYS] = {
	(1<<0),
	(1<<1),
	(1<<2)
};

static int mouse_down[MAX_MOUSE_KEYS] = {
	MOUSEEVENTF_LEFTDOWN,
	MOUSEEVENTF_RIGHTDOWN,
	MOUSEEVENTF_MIDDLEDOWN
};

static int mouse_up[MAX_MOUSE_KEYS] = {
	MOUSEEVENTF_LEFTUP,
	MOUSEEVENTF_RIGHTUP,
	MOUSEEVENTF_MIDDLEUP
};

struct user_cmd {
	int index;
	int mouse_dx;
	int mouse_dy;
	int mouse_keys;
	int keys[MAX_INPUTS];
};

struct mouse_state {
	int cursor_x;
	int cursor_y;
	int last_x;
	int last_y;
};

#define MAX_KEY_STATES 223

struct key_state {
	bool down;
	bool toggle;
	short last_vk_state;
	int inactive_time;
};

class c_input {
public:
	void pre_think();
	void post_think();
	user_cmd* get_user_cmd();
	
	void process_input();
	void handle_mouse_move(int x, int y);
	void handle_key_press(int key, int time);
	void handle_key_release(int key, int time);
	key_state* find_key(int vkey);
	
	int _width;
	int _height;
	
	LARGE_INTEGER _process_time;
	uint32_t _sample_time;
	
	user_cmd _cmds[MAX_USER_CMDS];
	int _cmd_index;
	
	mouse_state _mouse_state;
	int _center_x;
	int _center_y;
	
	key_state _key_states[MAX_KEY_STATES];
	int _last_key_time;
	
	int _last_key_pressed;
};

extern c_input* g_input;

#define MAX_CAPTURE_KEYS 88

static uint8_t s_vkey_capture[MAX_CAPTURE_KEYS] = {
	1,
	2,
	4,
	5,
	6,
	9,
	19,
	20,
	27,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	45,
	46,
	48,
	49,
	50,
	51,
	52,
	53,
	54,
	55,
	56,
	57,
	65,
	66,
	67,
	68,
	69,
	70,
	71,
	72,
	73,
	74,
	75,
	76,
	77,
	78,
	79,
	80,
	81,
	82,
	83,
	84,
	85,
	86,
	87,
	88,
	89,
	90,
	93,
	112,
	113,
	114,
	115,
	116,
	117,
	118,
	119,
	120,
	120,
	122,
	123,
	145,
	160,
	161,
	162,
	163,
	164,
	165,
	186,
	187,
	188,
	189,
	190,
	191,
	192,
	219,
	220,
	221,
	222,
};

static const char* s_vkey_names[MAX_CAPTURE_KEYS] = {
	"mouse1",
	"mouse2",
	"mouse3",
	"mouse4",
	"mouse5",
	"tab",
	"break",
	"caps_lock",
	"escape",
	"spacebar",
	"pageup",
	"pagedown",
	"end",
	"home",
	"left",
	"up",
	"right",
	"down",
	"insert",
	"delete",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",
	"option",
	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	"f9",
	"f9",
	"f11",
	"f12",
	"scroll_lock",
	"lshift",
	"rshift",
	"lctrl",
	"rctrl",
	"lalt",
	"ralt",
	"semicolon",
	"equals",
	"comma",
	"minus",
	"period",
	"fwdslash",
	"tilde",
	"[",
	"backslash",
	"]",
	"colon",
};

// dumped from MapVirtualKeyEx
#define MAX_SCAN_KEYS 136

static uint8_t s_vk_to_scan[MAX_SCAN_KEYS] = {
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0xe,
	0xf,
	0x0,
	0x0,
	0x4c,
	0x1c,
	0x0,
	0x0,
	0x2a,
	0x1d,
	0x38,
	0x0,
	0x3a,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x1,
	0x0,
	0x0,
	0x0,
	0x0,
	0x39,
	0x49,
	0x51,
	0x4f,
	0x47,
	0x4b,
	0x48,
	0x4d,
	0x50,
	0x0,
	0x0,
	0x0,
	0x54,
	0x52,
	0x53,
	0x5b,
	0xb,
	0x2,
	0x3,
	0x4,
	0x5,
	0x6,
	0x7,
	0x8,
	0x9,
	0xa,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x1e,
	0x30,
	0x2e,
	0x20,
	0x12,
	0x21,
	0x22,
	0x23,
	0x17,
	0x24,
	0x25,
	0x26,
	0x32,
	0x31,
	0x18,
	0x19,
	0x10,
	0x13,
	0x1f,
	0x14,
	0x16,
	0x2f,
	0x11,
	0x2d,
	0x15,
	0x2c,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x0,
	0x37,
	0x4e,
	0x0,
	0x4a,
	0x0,
	0x0,
	0x3b,
	0x3c,
	0x3d,
	0x3e,
	0x3f,
	0x40,
	0x41,
	0x42,
	0x43,
	0x44,
	0x56,
	0x57,
	0x5c,
	0x5d,
	0x5e,
	0x5f,
	0x60,
	0x61,
	0x62,
	0x63,
	0x64,
	0x65,
	0x66,
	0x6c,
};