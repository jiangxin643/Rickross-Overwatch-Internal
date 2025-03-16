#pragma once

struct DX9_Vertex {
	float x, y, z;
	DWORD color;
};

class c_dx9_render {
public:
	bool init();
	
	void start_render();
	void end_render();
	
	void draw_rect_xxyy(float x0, float y0, float x1, float y1, int r, int g, int b);
	void draw_text_point(int i, float x, float y);
	
	void set_dimensions(float w, float h) {
		_width = w;
		_height = h;
		if (_width > 0.0f)
			_width_inv = 1.0f / _width;
		
		if (_height > 0.0f)
			_height_inv = 1.0f / _height;
	}
	
	bool _init;
	bool _failed;
	
	float _width;
	float _height;
	float _width_inv;
	float _height_inv;
	
	DX9_Vertex* _vertices;
	int _vertex_count;
	
	DX9_Vertex* _points;
	int _point_count;
	
	int _text_r;
	int _text_g;
	int _text_b;
	
	class IDirect3DDevice9* _device;
	
	class IDirect3DVertexBuffer9* _vertex_buffer;
	class IDirect3DVertexBuffer9* _text_buffer;
};

extern c_dx9_render* g_dx9;