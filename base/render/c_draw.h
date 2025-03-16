#pragma once

struct GFXglyph {
	uint16_t bitmapOffset;     ///< Pointer into GFXfont->bitmap
	uint8_t  width;            ///< Bitmap dimensions in pixels
    uint8_t  height;           ///< Bitmap dimensions in pixels
	uint8_t  xAdvance;         ///< Distance to advance cursor (x axis)
	char   xOffset;          ///< X dist from cursor pos to UL corner
    char   yOffset;          ///< Y dist from cursor pos to UL corner
};

#define PROGMEM

#define MONO9PT_X 0
#define MONO9PT_Y 10

#define CHAR_X 3.0f
#define CHAR_Y 5.0f
#define CHAR_SPACE_X 7.0f
#define CHAR_SPACE_Y 2.0f
#define CHAR_SIZE 2.0f

enum {
	RENDERER_NONE = 0,
	RENDERER_DX9,
	RENDERER_DX11,
};

#define ESP_BOX_WIDTH 3.0f
#define ESP_BOX_HEIGHT 3.0f
#define ESP_BAR_WIDTH 4.0f
#define ESP_TEXT_X ESP_BOX_WIDTH + ESP_BAR_WIDTH + 1.0f
#define ESP_TEXT_Y (CHAR_Y * CHAR_SIZE) + CHAR_SPACE_Y

class c_draw {
public:
	void draw_char_fancy(char character, float x, float y);
	void draw_korean(wchar_t dat, float x, float y);
	void draw_char(char character, float x, float y);
	
	float draw_test(const wchar_t* text, float x, float y, int r, int g, int b);
	
	float draw_text_a(const char* text, float x, float y, int r, int g, int b);
	void draw_centered_text_a(const char* text, float x, float y, int r, int g, int b);
	void draw_left_text_a(const char* text, float x, float y, int r, int g, int b);
	
	void draw_rect_xxyy(float x0, float y0, float x1, float y1, int r, int g, int b);
	void draw_rect(float x, float y, float w, float h, int r, int g, int b);
	void draw_outline_rect(float x, float y, float w, float h, float size, int r, int g, int b);
	
	void hue_2_rgb(int hue, int* r, int* g, int* b);
	
	void format_int_1(char* out, const char* text, int num1);
	void format_int_2(char* out, const char* text, int num1, int num2);
	void format_int_3(char* out, const char* text, int num1, int num2, int num3);
	
	void draw_crosshair(bool green);
	
	void draw_esp_text(int type, char* text, int r, int g, int b);
	void draw_esp_box(int r, int g, int b);
	void draw_esp_box_corners(int r, int g, int b, float s);
	void draw_esp_bar(int type, float frac, int value, int max_value);
		
	inline float get_width() {
		return _width;
	}
	inline float get_height() {
		return _height;
	}
	inline void reset_text() {
		_text_min_x = 0.0f;
		_text_min_y = 0.0f;
		_text_max_x = 0.0f;
		_text_max_y = 0.0f;
		_text_center_x = 0.0f;
		_text_center_y = 0.0f;
		memset(_text_offsets_y, 0, sizeof(_text_offsets_y));
	}
	
	int _renderer;
	float _width;
	float _height;
	
	float _text_min_x;
	float _text_min_y;
	float _text_max_x;
	float _text_max_y;
	float _text_center_x;
	float _text_center_y;
	float _text_offsets_y[5];
};

extern c_draw* g_draw;