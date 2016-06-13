#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <windows.h>
#include <tchar.h>
#include "Math3D.h"
#include "RenderDevice.h"
#include "main.h"

//�豸��ʼ�����ͷ�-------------------------------------------------------------

// �豸��ʼ����fbΪ�ⲿ֡���棬�� NULL �������ⲿ֡���棨ÿ�� 4�ֽڶ��룩
void RenderDevice::device_init(int width, int height, void *fb)
{
	int need = sizeof(void*) * (height * 2 + 1024) + width * height * 4;
	char *ptr = (char*)malloc(need);

	char *framebuf,*zbuf;

	this->width = width;
	this->height = height;

	//framebuffer
	assert(ptr);	//����
	this->framebuffer = (IUINT32**)ptr;		//height��void*�ռ�
	if (fb != NULL) framebuf = (char*)fb;		//��framebufָ��λͼ
	for (int i = 0; i < height; i++) {
	this->framebuffer[i] = (IUINT32*)(framebuf + width * 4 * i);	//��ָ��ָ��framebufָ�����Ƭ�ռ�
	}
	ptr += sizeof(void*) * height;

	//ָ��zbuff�����ָ�� 
	this->zbuffer = (float**)ptr;		//height��void*�ռ�
	ptr += sizeof(void*) * height;

	//zbuff���� 
	zbuf = (char*)ptr;		//width * height * 4���ֽڿռ�
	ptr += width * height * 4;
	for (int i = 0; i < height; i++) {
		this->zbuffer[i] = (float*)(zbuf + width * 4 * i);	//��ָ��ָ��framebufָ�����Ƭ�ռ�
	}
	for (int j= 0 ; j<height; j++)
	{
		for (int i=0 ; i<width ; i++)
		{
			this->zbuffer[j][i]=1;
		}
	}

	this->texture = (IUINT32**)ptr;			//1024��void*�ռ�
	ptr += sizeof(void*) * 1024;

	Math3D::transform_init(&this->transform, width, height);

	
}

//����---------------------------------------

// ����
void RenderDevice::device_pixel(int x, int y, IUINT32 color) {
	if (((IUINT32)x) < (IUINT32)this->width && ((IUINT32)y) < (IUINT32)this->height) {
		this->framebuffer[y][x] = color;
	}
}

// �����߶�
void RenderDevice::device_draw_line(int x1, int y1, int x2, int y2, IUINT32 c) {
	int x, y, rem = 0;
	if (x1 == x2 && y1 == y2) {			//һ����
		device_pixel(x1, y1, c);
	}	else if (x1 == x2) {			//����
		int inc = (y1 <= y2)? 1 : -1;
		for (y = y1; y != y2; y += inc) device_pixel(x1, y, c);
		device_pixel(x2, y2, c);
	}	else if (y1 == y2) {			//����
		int inc = (x1 <= x2)? 1 : -1;
		for (x = x1; x != x2; x += inc) device_pixel(x, y1, c);
		device_pixel(x2, y2, c);
	}	else {
		int dx = (x1 < x2)? x2 - x1 : x1 - x2;
		int dy = (y1 < y2)? y2 - y1 : y1 - y2;
		if (dx >= dy) {				//��ȸ߳�
			if (x2 < x1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;	//�������㣬ʹx1<x2
			for (x = x1, y = y1; x <= x2; x++) {
				device_pixel(x, y, c);
				rem += dy;
				if (rem >= dx) {			//�ۻ�rem��ÿ��+dy����rem>=dxʱ���໭һ�����أ�rem-dx
					rem -= dx;
					y += (y2 >= y1)? 1 : -1;
					device_pixel(x, y, c);
				}
			}
			device_pixel(x2, y2, c);
		}	else {
			if (y2 < y1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; y <= y2; y++) {
				device_pixel(x, y, c);
				rem += dx;
				if (rem >= dy) {
					rem -= dy;
					x += (x2 >= x1)? 1 : -1;
					device_pixel(x, y, c);
				}
			}
			device_pixel(x2, y2, c);
		}
	}
}

// ���õ�ǰ����
void RenderDevice::device_set_texture(void *bits, long pitch, int w, int h) {
	char *ptr = (char*)bits;
	assert(w <= 1024 && h <= 1024);
	for (int j = 0; j < h; ptr += pitch, j++) 	// ���¼���ÿ�������ָ��
		this->texture[j] = (IUINT32*)ptr;
	this->tex_width = w;
	this->tex_height = h;
	this->max_u = (float)(w - 1);
	this->max_v = (float)(h - 1);
}

// ����Ⱦ����
void RenderDevice::device_render_trap(trapezoid_t *trap) {
	scanline_t scanline;
	int top, bottom;

	top = (int)(trap->top + 0.5f);
	bottom = (int)(trap->bottom + 0.5f);
	for (int j = top; j < bottom; j++) {

		if (j >= 0 && j < this->height) {
			Math3D::trapezoid_edge_interp(trap, (float)j + 0.5f);

			Math3D::trapezoid_init_scan_line(trap, &scanline, j);

			device_draw_scanline(&scanline);
		}
		if (j >= this->height) break;
	}
}

// ����ɨ����
void RenderDevice::device_draw_scanline(scanline_t *scanline) {
	int i = scanline->x;
	int lineWidth = scanline->w;
	int screenWidth = this->width;
	int render_state = this->render_state;

	for (; lineWidth > 0; i++, lineWidth--) {
		if (i >= 0 && i <= screenWidth) {
			//float rhw = scanline->v.rhw;
			if (scanline->v.pos.z<this->zbuffer[scanline->y][i]){		//��Ϊ���������ι���ͬһ���ߣ����ֵ��ͬ���ᵼ�¿����ıߵ���ɫ��ȷ��
				this->zbuffer[scanline->y][i] = scanline->v.pos.z;
				
				if (render_state & RENDER_STATE_COLOR) {
					float r = scanline->v.color.r / scanline->v.rhw;
					float g = scanline->v.color.g / scanline->v.rhw;
					float b = scanline->v.color.b / scanline->v.rhw;
					int R = (int)(r * 255.0f);
					int G = (int)(g * 255.0f);
					int B = (int)(b * 255.0f);
					R = Math3D::CMID(R, 0, 255);
					G = Math3D::CMID(G, 0, 255);
					B = Math3D::CMID(B, 0, 255);
					device_pixel(i,scanline->y,(R << 16) | (G << 8) | (B));
				}
				
				if (render_state & RENDER_STATE_TEXTURE) {
					float u = scanline->v.tc.u / scanline->v.rhw;	//ͨ����u/z��/(1/z)����ȡu
					float v = scanline->v.tc.v / scanline->v.rhw;
					IUINT32 cc = this->device_texture_read(u, v);
					device_pixel(i,scanline->y,cc);
				}
			}
		}

		Math3D::vertex_add(&scanline->v, &scanline->step);		//ÿ����һ�����أ������޸Ķ����ڵ�����
		if (i >= screenWidth) break;
	}
}

// ���������ȡ����,��������������ֵ����ͨ����ֵ
IUINT32 RenderDevice::device_texture_read(float u, float v) {
	int x, y;
	u = u * this->max_u;
	v = v * this->max_v;
	x = (int)(u + 0.5f);
	y = (int)(v + 0.5f);
	x = Math3D::CMID(x, 0, this->tex_width - 1);
	y = Math3D::CMID(y, 0, this->tex_height - 1);
	return this->texture[y][x];
}

//get and set------------------------------------------------------

transform_t& RenderDevice::GetTransform()
{
	return transform;
}

int RenderDevice::GetRenderState()
{
	return render_state;
}

void RenderDevice::SetRenderState(int state)
{
	render_state = state;
}

void RenderDevice::SetForeground(IUINT32 color)
{
	foreground = color;
}

IUINT32 RenderDevice::GetForeground()
{
	return foreground;
}