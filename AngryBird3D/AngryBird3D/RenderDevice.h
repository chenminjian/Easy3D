#ifndef _render_device_h
#define _render_device_h

#include "Math3D.h"
#include "main.h"

class RenderDevice
{
private:
	transform_t transform;      // ����任��
	int width;                  // ���ڿ��
	int height;                 // ���ڸ߶�
	IUINT32 **framebuffer;      // ���ػ��棺framebuffer[y] ����� y��
	float **zbuffer;            // ��Ȼ��棺zbuffer[y] Ϊ�� y��ָ��

	//�������
	IUINT32 **texture;          // ����ͬ����ÿ������
	int tex_width;              // ������
	int tex_height;             // ����߶�
	float max_u;                // ��������ȣ�tex_width - 1
	float max_v;                // �������߶ȣ�tex_height - 1

	int render_state;           // ��Ⱦ״̬
	IUINT32 foreground;         // �߿���ɫ

public:
	void device_init(int width, int height, void *fb);
	void device_pixel(int x, int y, IUINT32 color);
	void device_draw_line(int x1, int y1, int x2, int y2, IUINT32 c);
	//by me----------------------
	transform_t& GetTransform();
	int GetRenderState();
	void SetRenderState(int state);
	void SetForeground(IUINT32 color);
	IUINT32 GetForeground();
	//texture--------------------
	void device_set_texture(void *bits, long pitch, int w, int h);
	void device_draw_scanline(scanline_t *scanline);
	void device_render_trap(trapezoid_t *trap);
	IUINT32 device_texture_read(float u, float v);

};

#endif