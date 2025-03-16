#pragma once

struct DX11_Vertex {
	float pos[2];
	float color[3];
};

typedef RECT D3D11_RECT_EX;

struct D3D11_VIEWPORT_EX {
    FLOAT TopLeftX;
    FLOAT TopLeftY;
    FLOAT Width;
    FLOAT Height;
    FLOAT MinDepth;
    FLOAT MaxDepth;
};

class c_dx11_render {
public:
	bool init();
	bool ready();
	
	HWND get_window();
	
	void store_context();
	void load_context();
	void start_render();
	void end_render();
	
	void draw_rect_xxyy(float x0, float y0, float x1, float y1, int r, int g, int b);
	void draw_line(float x0, float y0, float x1, float y1, int r, int g, int b);
	void draw_text_point(float x, float y);
	
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
	
	bool _rendering_ok;
	
	bool _draw_text_outline;
	
	DX11_Vertex* _vertices;
	int _vertex_count;
	
	DX11_Vertex* _lines;
	int _line_count;
	
	DX11_Vertex* _points;
	int _point_count;
	
	int _text_r;
	int _text_g;
	int _text_b;
	
	class IDXGISwapChain* _swapchain;
	class ID3D11Device* _device;
	class ID3D11DeviceContext* _context;
	void* _render_target;
	void* _input;
	
	void* _vertex_buffer;
	void* _lines_buffer;
	void* _text_buffer;
	
	void* _rasterizer_state;
	void* _pixel_shader;
	void* _vertex_shader;
	
	// state
	// 16 = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE
	void* _old_input;
	void* _old_vertex_buffer;
	UINT _old_stride;
	UINT _old_offset;
	UINT _old_topology;
	
	void* _old_rasterizer_state;
	D3D11_RECT_EX _old_scissor_rects[16];
	UINT _old_scissor_rects_count;
	D3D11_VIEWPORT_EX _old_viewports[16];
	UINT _old_viewport_count;
	
	void* _old_pixel_shader;
	void* _old_vertex_shader;
	void* _old_geometry_shader;
	void* _old_hull_shader;
	void* _old_domain_shader;
	void* _old_compute_shader;
	
	// 8 = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT
	void* _old_blend_state;
	float _old_blend_factor[4];
	UINT _old_sample_mask;
};

extern c_dx11_render* g_dx11;

// 1/255
#define CLR_MUL 0.003921568627451