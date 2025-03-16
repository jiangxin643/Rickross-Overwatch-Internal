#include "../main_win.h"
#include "c_draw.h"
#include "../dx9/c_dx9.h"
#include "../dx11/c_dx11.h"

#include "FreeMono9pt7b.h"

c_draw draw_instance;
c_draw* g_draw = &draw_instance;

void c_draw::draw_char_fancy(char character, float x, float y) {
	int index = -1;
	
	if (character >= 0x20 && character <= 0x7D)
		index = character - 0x20;
	
	if (index == -1)
		index = 31; // ?
	
	GFXglyph* glyph = &(FreeMono9pt7bGlyphs[index]);
	
	int bo = glyph->bitmapOffset;
	int w = glyph->width;
	int h = glyph->height;
	
	int xo = glyph->xOffset;
	int yo = glyph->yOffset;
	
	int bits = 0;
	int bit = 0;
	
	float x0 = x + (float)(xo + MONO9PT_X);
	float y0 = y + (float)(yo + MONO9PT_Y);
	
	for (int yy = 0; yy < h; ++yy) {
		for (int xx = 0; xx < w; ++xx) {
			if (!(bit++ & 7))
				bits = FreeMono9pt7bBitmaps[bo++];
			
			if (bits & 0x80) {
				float x1 = (float)(xx);
				float y1 = (float)(yy);
				
				int old_r = g_dx11->_text_r;
				int old_g = g_dx11->_text_g;
				int old_b = g_dx11->_text_b;
				
				g_dx11->draw_text_point(x0 + x1, y0 + y1);
			}
			
			bits <<= 1;
		}
	}
}

/*
Unicode range
	

    U+1100–U+11FF
    U+3130–U+318F
    U+A960–U+A97F
    U+D7B0–U+D7FF
*/

void c_draw::draw_korean(wchar_t dat, float x, float y) {
	
}

void c_draw::draw_char(char character, float x, float y) {
	this->draw_char_fancy(character, x, y);
}

float c_draw::draw_test(const wchar_t* text, float x, float y, int r, int g, int b) {
	g_dx11->_text_r = r;
	g_dx11->_text_g = g;
	g_dx11->_text_b = b;
	
	//int len = strlen(text);
	int len = 3;
	//for (int i = 0; i < len; ++i)
		this->draw_korean(text[0], x + ((float)(0) * 60.0f), y);
	
	return ((float)(len) * 60.0f);
}

float c_draw::draw_text_a(const char* text, float x, float y, int r, int g, int b) {
	//g_dx9->_text_r = r;
	//g_dx9->_text_g = g;
	//g_dx9->_text_b = b;
	
	g_dx11->_text_r = r;
	g_dx11->_text_g = g;
	g_dx11->_text_b = b;
	
	int len = strlen(text);
	for (int i = 0; i < len; ++i)
		this->draw_char(text[i], x + ((float)(i) * CHAR_SPACE_X), y);
	
	return ((float)(len) * CHAR_SPACE_X);
}

void c_draw::draw_centered_text_a(const char* text, float x, float y, int r, int g, int b) {
	int len = strlen(text);
	float x_offset = ((float)(len) * CHAR_SPACE_X) / 2.0f;
	
	this->draw_text_a(text, x - x_offset, y, r, g, b);
}

void c_draw::draw_left_text_a(const char* text, float x, float y, int r, int g, int b) {
	int len = strlen(text);
	float x_offset = ((float)(len) * CHAR_SPACE_X);
	
	this->draw_text_a(text, x - x_offset, y, r, g, b);
}

void c_draw::draw_rect_xxyy(float x0, float y0, float x1, float y1, int r, int g, int b) {
	//g_dx9->draw_rect_xxyy(x0, y0, x1, y1, r, g, b);
	g_dx11->draw_rect_xxyy(x0, y0, x1, y1, r, g, b);
}

void c_draw::draw_rect(float x, float y, float w, float h, int r, int g, int b) {
	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y + h;
	
	this->draw_rect_xxyy(x0, y0, x1, y1, r, g, b);
}

void c_draw::draw_outline_rect(float x, float y, float w, float h, float size, int r, int g, int b) {
	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y + h;
	
	this->draw_rect_xxyy(x0, y0, x1, y0 + size, r, g, b);
	this->draw_rect_xxyy(x0, y1 - size, x1, y1, r, g, b);
	this->draw_rect_xxyy(x0, y0 + size, x0 + size, y1 - size, r, g, b);
	this->draw_rect_xxyy(x1 - size, y0 + size, x1, y1 - size, r, g, b);
}

void c_draw::hue_2_rgb(int hue, int* r, int* g, int* b) {
	uint8_t h = (uint8_t)(hue);
	uint8_t region = h / 43;
	uint8_t remainder = (h - (region * 43)) * 6;
	
	int q = (int)((255 * (255 - (255 * (remainder >> 8)))) >> 8);
	int t = (int)((255 * (255 - ((255 * (255 - remainder)) >> 8))) >> 8);

    switch (region) {
    	case 0: {
			*r = 255; *g = t; *b = 0;
			break;
        }
        case 1: {
			*r = q; *g = 255; *b = 0;
			break;
        }
		case 2: {
			*r = 0; *g = 255; *b = t;
			break;
		}
		case 3: {
			*r = 0; *g = q; *b = 255;
			break;
		}
		case 4: {
			*r = t; *g = 0; *b = 255;
			break;
		}
        default: {
			*r = 255; *g = 0; *b = q;
			break;
		}
    }
}

void c_draw::format_int_1(char* out, const char* text, int num1) {
	I(wsprintfA)(out, text, num1);
}

void c_draw::format_int_2(char* out, const char* text, int num1, int num2) {
	I(wsprintfA)(out, text, num1, num2);
}

void c_draw::format_int_3(char* out, const char* text, int num1, int num2, int num3) {
	I(wsprintfA)(out, text, num1, num2, num3);
}

void c_draw::draw_crosshair(bool green) {
	float x = this->get_width() * 0.5f - 1;
	float y = this->get_height() * 0.5f - 1;
	
	int r = green ? 0 : 255;
	int g = green ? 255 : 0;
	int b = 0;
	
	this->draw_rect(x, y - 4, 2, 2, r, g, b);
	this->draw_rect(x, y + 4, 2, 2, r, g, b);
	this->draw_rect(x - 4, y, 2, 2, r, g, b);
	this->draw_rect(x + 4, y, 2, 2, r, g, b);
}

void c_draw::draw_esp_text(int type, char* text, int r, int g, int b) {
	switch (type) {
		case 1: {
			float x = _text_max_x + ((_text_min_y - _text_max_y) * 0.25f);
			float y = _text_max_y + _text_offsets_y[0];
			this->draw_text_a(text, x + ESP_TEXT_X, y, r, g, b);
			
			_text_offsets_y[0] += ESP_TEXT_Y;
			
			break;
		}
		case 2: {
			float x = _text_max_x - ((_text_min_y - _text_max_y) * 0.25f);
			float y = _text_max_y + _text_offsets_y[1];
			this->draw_left_text_a(text, x - ESP_TEXT_X - 4.0f, y, r, g, b);
			
			_text_offsets_y[1] += ESP_TEXT_Y;
			
			break;
		}
		case 3: {
			float x = _text_max_x;
			float y = _text_max_y + _text_offsets_y[2];
			this->draw_centered_text_a(text, x, y - 18.0f, r, g, b);
			
			_text_offsets_y[2] += ESP_TEXT_Y;
			
			break;
		}
		case 4: {
			float x = _text_min_x;
			float y = _text_min_y + _text_offsets_y[3];
			this->draw_centered_text_a(text, x, y, r, g, b);
			
			_text_offsets_y[3] += ESP_TEXT_Y;
			
			break;
		}
		case 5: {
			float x = _text_center_x;
			float y = _text_center_y + _text_offsets_y[4];
			this->draw_text_a(text, x, y, r, g, b);
			
			_text_offsets_y[4] += ESP_TEXT_Y;
			
			break;
		}
	}
}

void c_draw::draw_esp_box(int r, int g, int b) {
	float min_x = _text_min_x;
	float min_y = _text_min_y;
	float max_x = _text_max_x;
	float max_y = _text_max_y;
	
	float x = max_x - ((min_y - max_y) * 0.25f) + 2.0f;
	float y = max_y + 2.0f;
	float w = (min_y - max_y) * 0.5f - 2.0f;
	float h = min_y - max_y - 2.0f;
	
	this->draw_outline_rect(x, y, w, h, 1.0f, r, g, b);
	this->draw_outline_rect(x - 1.0f, y - 1.0f, w + 2.0f, h + 2.0f, 1.0f, 0, 0, 0);
	this->draw_outline_rect(x + 1.0f, y + 1.0f, w - 2.0f, h - 2.0f, 1.0f, 0, 0, 0);
}

void c_draw::draw_esp_box_corners(int r, int g, int b, float s) {
	float min_x = _text_min_x;
	float min_y = _text_min_y;
	float max_x = _text_max_x;
	float max_y = _text_max_y;
	
	float x = max_x - ((min_y - max_y) * 0.25f);
	float y = max_y;
	float w = (min_y - max_y) * 0.5f;
	float h = min_y - max_y;
	
	float x2 = x + w;
	float y2 = y + h;
	
	this->draw_rect_xxyy(x, y, x + 4.0f, y + 1.0f, r, g, b);
	this->draw_rect_xxyy(x, y, x + 1.0f, y + 4.0f, r, g, b);
	
	this->draw_rect_xxyy(x2, y, x2 - 4.0f, y + 1.0f, r, g, b);
	this->draw_rect_xxyy(x2 - 1.0f, y, x2, y + 4.0f, r, g, b);
	
	this->draw_rect_xxyy(x, y2 - 1.0f, x + 4.0f, y2, r, g, b);
	this->draw_rect_xxyy(x, y2, x + 1.0f, y2 - 4.0f, r, g, b);
	
	this->draw_rect_xxyy(x2, y2 - 1.0f, x2 - 4.0f, y2, r, g, b);
	this->draw_rect_xxyy(x2, y2, x2 - 1.0f, y2 - 4.0f, r, g, b);
}

void c_draw::draw_esp_bar(int type, float frac, int value, int max_value) {
	float min_x = _text_min_x;
	float min_y = _text_min_y;
	float max_x = _text_max_x;
	float max_y = _text_max_y;
	
	float mul = (float)(value) / (float)(max_value);
	if (mul > 1.0f)
		mul = 1.0f;
	
	float hp_mul = mul;
	float step = 0.0f;
	
	if (frac > 1.0f) {
		step = 1.0f / frac;
		int int_mul = (int)((frac * mul) + 0.5f);
		if (int_mul < 1)
			int_mul = 1;
		
		hp_mul = (float)(int_mul) / frac;
	}
	
	int r = (int)(0.0f + (510.0f * (1.0f - mul)));
	if (r > 255)
		r = 255;
	
	int g = (int)(510.0f - (510.0f * (1.0f - mul)));
	if (g > 255)
		g = 255;
	
	switch (type) {
		case 1:
		case 2: {
			float sign = 1.0f;
			if (type == 2)
				sign = -1.0f;
			
			float x = max_x + (((min_y - max_y) * 0.25f) * sign) + (ESP_BOX_WIDTH * sign);
			float y = max_y;
			float dt = (min_y - max_y);
			
			float len = dt * hp_mul;
			
			this->draw_rect(x, y, 2.0f, dt, 0, 0, 0);
			this->draw_rect(x, y + (dt - len), 2.0f, len, r, g, 0);
			
			if (frac > 1.0f) {
				for (float f = 0.0f; f <= 1.0f; f += step) {
					float len2 = dt * f;
					this->draw_rect(x, y + (dt - len2), 2.0f, 1.0f, 0, 0, 0);
				}
			}
			
			this->draw_outline_rect(x - 1.0f, y, 4.0f, dt + 1.0f, 1.0f, 0, 0, 0);
			
			break;
		}
		case 3:
		case 4:
		case 5: {
			float sign = 1.0f;
			if (type == 3)
				sign = -1.0f;
			
			float x = 0.0f;
			float y = 0.0f;
			float dt = ((min_y - max_y) * 0.25f) * 2.0f;
			
			if (type == 3) {
				x = _text_max_x - ((min_y - max_y) * 0.25f);
				y = _text_max_y - _text_offsets_y[2] + (ESP_BOX_HEIGHT * sign);
			} else if (type == 4) {
				x = _text_max_x - ((min_y - max_y) * 0.25f);
				y = _text_min_y + _text_offsets_y[3] + (ESP_BOX_HEIGHT * sign);
			} else if (type == 5) {
				x = _text_center_x;
				y = _text_center_y + _text_offsets_y[4] + 1.0f;
			}
			
			float len = dt * hp_mul;
			
			this->draw_rect(x, y, dt, 2.0f, 0, 0, 0);
			this->draw_rect(x, y, len, 2.0f, r, g, 0);
			
			if (frac > 1.0f) {
				for (float f = 0.0f; f <= 1.0f; f += step) {
					float len2 = dt * f;
					this->draw_rect(x + (dt - len2), y, 1.0f, 2.0f, 0, 0, 0);
				}
			}
			
			this->draw_outline_rect(x, y - 1.0f, dt + 1.0f, 4.0f, 1.0f, 0, 0, 0);
			
			break;
		}
	}
}