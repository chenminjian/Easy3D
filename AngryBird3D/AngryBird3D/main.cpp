#include "WindowsRelated.h"
#include "main.h"
#include "Math3D.h"
#include "RenderDevice.h"
#include "TextureBMP.h"

int screen_w, screen_h;					//��Ļ���
int screen_exit = 0;					//������Ļ�˳�

vertex_t mesh[8] = {
	{ {  1, -1,  1, 1 }, { 0, 0 }, { 1.0f, 0.2f, 0.2f } , 1 },
	{ { -1, -1,  1, 1 }, { 0, 1 }, { 0.2f, 1.0f, 0.2f } , 1 },
	{ { -1,  1,  1, 1 }, { 1, 1 }, { 0.2f, 0.2f, 1.0f } , 1 },
	{ {  1,  1,  1, 1 }, { 1, 0 }, { 1.0f, 0.2f, 1.0f } , 1 },
	{ {  1, -1, -1, 1 }, { 0, 0 }, { 1.0f, 1.0f, 0.2f } , 1 },
	{ { -1, -1, -1, 1 }, { 0, 1 }, { 0.2f, 1.0f, 1.0f } , 1 },
	{ { -1,  1, -1, 1 }, { 1, 1 }, { 1.0f, 0.3f, 0.3f } , 1 },
	{ {  1,  1, -1, 1 }, { 1, 0 }, { 0.2f, 1.0f, 0.3f } , 1 },
};

void camera_at_zero(RenderDevice *device, float x, float y, float z) {
	point_t eye = { x, y, z, 1 }, at = { 0, 0, 0, 1 }, up = { 0, 0, 1, 1 };
	Math3D::matrix_set_lookat(&(device->GetTransform().view), &eye, &at, &up);
	Math3D::transform_update(&device->GetTransform());
}

// ��ʸ�� x ���� project 
void transform_apply(vector_t *y, const vector_t *x, const transform_t *ts) {
	Math3D::matrix_apply(y, x, &ts->transform);
}

// ����������ͬ cvv �ı߽�������׶�ü�
int transform_check_cvv(const vector_t *v) {
	float w = v->w;
	int check = 0;
	if (v->z < 0.0f) check |= 1;
	if (v->z >  w) check |= 2;
	if (v->x < -w) check |= 4;
	if (v->x >  w) check |= 8;
	if (v->y < -w) check |= 16;
	if (v->y >  w) check |= 32;
	return check;
}

// ��һ�����õ���Ļ����
void transform_homogenize(vector_t *y, const vector_t *x,const transform_t *ts) {
	float rhw = 1.0f / x->w;
	y->x = (x->x * rhw + 1.0f) * ts->w * 0.5f;
	y->y = (1.0f - x->y * rhw) * ts->h * 0.5f;
	y->z = x->z * rhw;
	y->w = 1.0f;
}

// ���� render_state ����ԭʼ������
void device_draw_primitive(RenderDevice *device, const vertex_t *v1, 
	const vertex_t *v2, const vertex_t *v3) {
		point_t p1, p2, p3, c1, c2, c3;
		int render_state = device->GetRenderState();

		// ���� Transform �仯
		transform_apply(&c1, &v1->pos,&device->GetTransform());
		transform_apply(&c2, &v2->pos,&device->GetTransform());
		transform_apply(&c3, &v3->pos,&device->GetTransform());

		// �ü���ע��˴���������Ϊ�����жϼ������� cvv���Լ�ͬcvv�ཻƽ����������
		// ���н�һ����ϸ�ü�����һ���ֽ�Ϊ������ȫ���� cvv�ڵ�������
		if (transform_check_cvv(&c1) != 0) return;
		if (transform_check_cvv(&c2) != 0) return;
		if (transform_check_cvv(&c3) != 0) return;

		// ��һ��
		transform_homogenize(&p1, &c1,&device->GetTransform());
		transform_homogenize(&p2, &c2,&device->GetTransform());
		transform_homogenize(&p3, &c3,&device->GetTransform());

		// �������ɫ�ʻ���
		if (render_state & (RENDER_STATE_TEXTURE | RENDER_STATE_COLOR)) {
			vertex_t t1 = *v1, t2 = *v2, t3 = *v3;		//ԭʼ�����е�������Ϣ�Ƚ����õ���ֻ�������λ����Ϣ
			trapezoid_t traps[2];
			int n;

			t1.pos = p1; 
			t2.pos = p2;
			t3.pos = p3;
			t1.pos.w = c1.w;   //w��ֵ�Ƕ��������������Zֵ��Ϊ�˷�����㣬��������
			t2.pos.w = c2.w;
			t3.pos.w = c3.w;
			
			Math3D::vertex_rhw_init(&t1);	// ��ʼ�� w
			Math3D::vertex_rhw_init(&t2);	// ��ʼ�� w
			Math3D::vertex_rhw_init(&t3);	// ��ʼ�� w
			
			// ���������Ϊ0-2�����Σ����ҷ��ؿ�����������
			n = Math3D::trapezoid_init_triangle(traps, &t1, &t2, &t3);
			
			if (n >= 1) device->device_render_trap(&traps[0]);
			if (n >= 2) device->device_render_trap(&traps[1]);
			
		}
		if (device->GetRenderState() & RENDER_STATE_WIREFRAME) {		// �߿����
			device->device_draw_line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, device->GetForeground());
			device->device_draw_line((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, device->GetForeground());
			device->device_draw_line((int)p3.x, (int)p3.y, (int)p2.x, (int)p2.y, device->GetForeground());
		}
}

//��Ӧ���ĸ�����
void draw_plane(RenderDevice *device, int a, int b, int c, int d) {
	vertex_t p1 = mesh[a], p2 = mesh[b], p3 = mesh[c], p4 = mesh[d];
	p1.tc.u = 0, p1.tc.v = 0, p2.tc.u = 0, p2.tc.v = 1;
	p3.tc.u = 1, p3.tc.v = 1, p4.tc.u = 1, p4.tc.v = 0;
	device_draw_primitive(device, &p1, &p2, &p3);
	device_draw_primitive(device, &p3, &p4, &p1);
}

void draw_box(RenderDevice *device, float theta) {
	matrix_t m;
	//matrix_set_rotate(&m, -1, -0.5, 1, theta);
	Math3D::matrix_set_identity(&m);
	Math3D::matrix_set_translate(&m,0,2,0);
	//device->GetTransform().world = m;
	Math3D::matrix_set_equal(&(device->GetTransform().world),&m);
	Math3D::transform_update(&device->GetTransform());
	draw_plane(device, 0, 1, 2, 3);
	draw_plane(device, 4, 5, 6, 7);
	draw_plane(device, 0, 4, 5, 1);
	draw_plane(device, 1, 5, 6, 2);
	draw_plane(device, 2, 6, 7, 3);
	draw_plane(device, 3, 7, 4, 0);
}

void init_texture(RenderDevice *device) {

	wchar_t last[1000] = L"\\AngryBird3D\\Resources\\yin.bmp";		//ͼƬ�ڹ�����λ��
	wchar_t buf[1000];												//ͼƬ��Ӳ�̵�λ��
	GetModuleFileName(NULL,buf,sizeof(buf));
	//wcout<<"Texture Location��"<<buf<<endl;
	for (int i=wcslen(buf)-1,k=2;i>=0&&k>0;i--)
	{
		if (buf[i]==L'\\')
		{
			buf[i] = L'\0';
			k--;
		}
	}
	//wcout<<"Texture Location��"<<buf<<endl;
	for (int i = 0,j = wcslen(buf);i<wcslen(last);i++,j++)
	{
		buf[j] = last[i];
		if (i == wcslen(last) - 1)
		{
			buf[j+1] = L'\0';
		}
	}
	wcout<<"Texture Location��"<<buf<<endl;

	//TextureBMP * _bmp = new TextureBMP("C:\\Users\\hzchenminjian\\Desktop\\MyCode\\AngryBird3D\\AngryBird3D\\Resources\\yin.bmp");
	TextureBMP * _bmp = new TextureBMP(buf);
	printf("Width:%d,Height:%d",_bmp->Width(),_bmp->Height());


	static IUINT32 texture[256][256];
	/*
	for (int j = 0; j < 256; j++) {
		for (int i = 0; i < 256; i++) {
			int x = i / 32, y = j / 32;
			texture[j][i] = ((x + y) & 1)? 0xffffff : 0x3fbcef;
		}
	}
	*/
	
	/*
	for (int j = 0; j < 256; j++) {
		for (int i = 0; i < 256; i++) {
			int x = i / 85;
			texture[j][i] = (x & 1)? 0xffffff : 0x3fbcef;
		}
	}
	*/
	
	
	for (int j = 0; j < 256; j++) {
		for (int i = 0; i < 256; i++) {
			if(j<_bmp->Height()&&i<_bmp->Width())
				texture[j][i] = _bmp->PixelColor(256-j,i);	//��Ϊ�����µߵ���
			else
				texture[j][i] = 0x00ffffff;
		}
	}
	

	device->device_set_texture(texture, 256 * 4, 256, 256);
}

int main()
{
	int states[] = { RENDER_STATE_TEXTURE, RENDER_STATE_COLOR, RENDER_STATE_WIREFRAME };
	WindowsRelated windowsRelated;
	RenderDevice renderDevice;

	TCHAR *title = _T("Mini3d (software render tutorial) - ")
		_T("Left/Right: rotation, Up/Down: forward/backward, Space: switch state");

	//����ȫ�ֱ���
	screen_w = 800;
	screen_h = 600;

	if (windowsRelated.screen_init(800, 600, title)) 
		return -1;

	renderDevice.device_init(800, 600, windowsRelated.getScreenPtr());
	camera_at_zero(&renderDevice, 4, 0, 0);

	init_texture(&renderDevice);
	renderDevice.SetRenderState(RENDER_STATE_TEXTURE);

	while(screen_exit == 0)
	{
		windowsRelated.screen_dispatch();







		draw_box(&renderDevice,0);

		windowsRelated.screen_update();
	}

	return 0;
}