#include "../main_win.h"
#include "c_dx9.h"
#include "d3d9.h"

c_dx9_render dx9_instance;
c_dx9_render* g_dx9 = &dx9_instance;

#define VERTEX_SIZE 8192
#define TEXT_POINTS_SIZE 32768

bool c_dx9_render::init() {
	if (_init)
		return !_failed;
	
	_init = true;
	_failed = true;
	
	{
		HRESULT result = _device->CreateVertexBuffer(VERTEX_SIZE * 16, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_XYZ|D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &_vertex_buffer, 0);
		if (result != D3D_OK) {
			return false;
		}
	}
	
	{
		HRESULT result = _device->CreateVertexBuffer(TEXT_POINTS_SIZE * 16, D3DUSAGE_WRITEONLY|D3DUSAGE_POINTS|D3DUSAGE_DYNAMIC, D3DFVF_XYZ|D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &_text_buffer, 0);
		if (result != D3D_OK) {
			return false;
		}
	}
	
	_failed = false;
	
	return true;
}

void c_dx9_render::start_render() {
	{
		float pointSize = 1.0f;
		
		_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		_device->SetRenderState(D3DRS_LIGHTING, FALSE);
		_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
		
		_device->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
		_device->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&pointSize));
		_device->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&pointSize));
		_device->SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&pointSize));
		
		_device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE);
		_device->SetPixelShader(NULL);
		_device->SetTexture((D3DRENDERSTATETYPE)(0), 0);
		
		_vertex_buffer->Lock(0, 0, (void**)(&_vertices), D3DLOCK_DISCARD);
		_text_buffer->Lock(0, 0, (void**)(&_points), D3DLOCK_DISCARD);
		
	//	memset(_vertices, 0, sizeof(_vertices) * VERTEX_SIZE);
	//	memset(_points, 0, sizeof(_points) * TEXT_POINTS_SIZE);
		
		_vertex_count = 0;
		_point_count = 0;
	}
}

void c_dx9_render::end_render() {
	{
		_vertex_buffer->Unlock();
		_text_buffer->Unlock();
	}
	
	{
		_device->SetStreamSource(0, _vertex_buffer, 0, sizeof(DX9_Vertex));
		_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, _vertex_count / 3);
		
		_device->SetStreamSource(0, _text_buffer, 0, sizeof(DX9_Vertex));
		_device->DrawPrimitive(D3DPT_POINTLIST, 0, _point_count);
	}
}

void c_dx9_render::draw_rect_xxyy(float x0, float y0, float x1, float y1, int r, int g, int b) {
	if (_vertex_count >= VERTEX_SIZE - 6 - 1)
		return;
	
	float x0x = -1.0f + (2.0f * (x0 * _width_inv));
	float y0x = 1.0f - (2.0f * (y0 * _height_inv));
	float x1x = -1.0f + (2.0f * (x1 * _width_inv));
	float y1x = 1.0f - (2.0f * (y1 * _height_inv));
	
	int i = _vertex_count;
	
	_vertices[i].x = x0x;
	_vertices[i].y = y0x;
	_vertices[i].color = D3DCOLOR_XRGB(r, g, b);
	
	i = ++_vertex_count;
	
	_vertices[i].x = x1x;
	_vertices[i].y = y0x;
	_vertices[i].color = D3DCOLOR_XRGB(r, g, b);
	
	i = ++_vertex_count;
	
	_vertices[i].x = x0x;
	_vertices[i].y = y1x;
	_vertices[i].color = D3DCOLOR_XRGB(r, g, b);
	
	i = ++_vertex_count;
	
	_vertices[i].x = x1x;
	_vertices[i].y = y1x;
	_vertices[i].color = D3DCOLOR_XRGB(r, g, b);
	
	i = ++_vertex_count;
	
	_vertices[i].x = x0x;
	_vertices[i].y = y1x;
	_vertices[i].color = D3DCOLOR_XRGB(r, g, b);
	
	i = ++_vertex_count;
	
	_vertices[i].x = x1x;
	_vertices[i].y = y0x;
	_vertices[i].color = D3DCOLOR_XRGB(r, g, b);
	
	++_vertex_count;
}

void c_dx9_render::draw_text_point(int i, float x, float y) {
	if (i > TEXT_POINTS_SIZE - 1)
		return;
	
	float x0 = -1.0f + (2.0f * (x * _width_inv));
	float y0 = 1.0f - (2.0f * (y * _height_inv));
	
	_points[i].x = x0;
	_points[i].y = y0;
	_points[i].color = D3DCOLOR_XRGB(_text_r, _text_g, _text_b);
}