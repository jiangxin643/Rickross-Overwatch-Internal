#include "draw.h"
#include "base.h"

Draw draw;

int Draw::TextWidth(string Text)
{
	//RECT rect = {0, 0, 0, 0};
	//draw.font->DrawText(NULL, Text.c_str(), -1, &rect, DT_CALCRECT, NULL);
//	return rect.right - rect.left;
	return 0;
}

void Draw::Text(int x, int y, char* text, D3DCOLOR color, bool isBordered, TextAlignment eAlignment)
{
	RECT rect;

	/*if (isBordered)
	{
		struct ShadowStruct { int x; int y; };
		ShadowStruct shadowOffset[] = {{-1, 0}, {+1, 0}, {0, -1}, {0, +1}};

		for (int i = 0; i < GetSizeOf(shadowOffset); i++)
		{
			SetRect(&rect, x + shadowOffset[i].x, y + shadowOffset[i].y, x + shadowOffset[i].x, y + shadowOffset[i].y);
			this->font->DrawTextA(NULL, text.c_str(), -1, &rect, eAlignment | DT_NOCLIP, BlackColor(100));
		}
	}
	else
	{
		SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
		this->font->DrawTextA(NULL, text.c_str(), -1, &rect, eAlignment | DT_NOCLIP, BlackColor(50));
	}*/
//	SetRect(&rect, x, y, x, y);
//	this->font->DrawTextA(NULL, text.c_str(), -1, &rect, eAlignment | DT_NOCLIP, color);;
	int r = (color >> 16) & 0xFF;
	int g = (color >> 8) & 0xFF;
	int b = color & 0xFF;
	
	g_draw->draw_text_a(text, x, y, r, g, b);
}

void Draw::Line(float x, float y, float x2, float y2, D3DCOLOR color)
{
/*	Vertex v[2] =
	{
		{x,		y,	0.0f, 1.0f, color},
		{x2,	y2,	0.0f, 1.0f, color},
	};
	this->device->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(Vertex));*/
	g_dx11->draw_line(x, y, x2, y2, 0, 255, 0);
}

void Draw::Border(float x, float y, float w, float h, D3DCOLOR color)
{
	/*Vertex vertices[6] =
	{
		x + w, y,		0.0f, 1.0f, color, 0.0f, 0.0f,
		x + w, y + h,	0.0f, 1.0f, color, 0.0f, 0.0f,
		x, y + h,		0.0f, 1.0f, color, 0.0f, 0.0f,

		x, y + h,		0.0f, 1.0f, color, 0.0f, 0.0f,
		x, y,			0.0f, 1.0f, color, 0.0f, 0.0f,
		x + w, y,		0.0f, 1.0f, color, 0.0f, 0.0f,
	};

	this->device->DrawPrimitiveUP(D3DPT_LINESTRIP, 5, vertices, sizeof(Vertex));*/
	//?
}

void Draw::Rectangle(float x, float y, float w, float h, D3DCOLOR startColor, D3DCOLOR endColor, D3DCOLOR borderColor)
{
	/*if (endColor == NULL)
		endColor = startColor;

	Vertex vertices[4] =
	{
		x,  y,			0.0f, 1.0f, startColor, 0.0f, 0.0f,
		x + w, y,		0.0f, 1.0f, startColor, 1.0f, 0.0f,

		x + w, y + h,	0.0f, 1.0f, endColor, 1.0f, 1.0f,
		x,  y + h,		0.0f, 1.0f, endColor, 0.0f, 1.0f,
	};

	this->device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));

	if (borderColor != NULL)
		Border(x - 1, y - 1, w + 1, h + 1, borderColor);*/
	if (borderColor != NULL) {
		int r = (borderColor >> 16) & 0xFF;
		int g = (borderColor >> 8) & 0xFF;
		int b = borderColor & 0xFF;
		
		g_draw->draw_rect(x - 1, y - 1, w + 1, h + 1, r, g, b);
	}
	
	int r = (startColor >> 16) & 0xFF;
	int g = (startColor >> 8) & 0xFF;
	int b = startColor & 0xFF;
	
	g_draw->draw_rect(x, y, w, h, r, g, b);
}