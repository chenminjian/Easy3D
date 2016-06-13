#include "WindowsRelated.h"
#include "main.h"
#include "Math3D.h"

/*
int screen_mx = 0, screen_my = 0, screen_mb = 0;
int screen_keys[512];	// ��ǰ���̰���״̬
long screen_pitch = 0;
*/

WindowsRelated::WindowsRelated():screen_handle(NULL),screen_dc(NULL),screen_hb(NULL),screen_ob(NULL),screen_fb(NULL)
{
	
}

int WindowsRelated::screen_init(int w,int h,TCHAR * title)
{
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, //�ṹ���С�����ߡ�1��ͼ��λ����0��w*h*�ֽ���
		w * h * 4, 0, 0, 0, 0 }  };
	RECT rect = { 0, 0, w, h };	//�洢��������
	int wx, wy, sx, sy;			//�����ߡ��������Ͻ�λ��
	HDC hDC;				//��ʱ�����������Ļ����������й�ĳ���豸�Ļ���������Ϣ�� Windows ���ݽṹ�����л��Ƶ��ö�ͨ���豸�����Ķ�����У���Щ�����װ�����ڻ�����������״���ı��� Windows API��
	LPVOID ptr;

	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,//���ڵĿͻ������ԡ��ַ��߽硱���룬��ϵͳ�������ڵ�ˮƽλ��ʱ���ͻ���������������8��������
		NULL, NULL, NULL, NULL, _T("SCREEN3.1415926") };//����
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);		//����WNDCLASS����Ϥ
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc)) return -1;

	screen_handle = CreateWindow(_T("SCREEN3.1415926"), title,	//����һ������Ĵ��ڣ���һ����������һ���߿�|�б�����|����ϵͳ�˵�|����С��
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
	if (screen_handle == NULL) return -2;

	hDC = GetDC(screen_handle);				//��ȡ��HDCֱ��������豸��ͨ
	screen_dc = CreateCompatibleDC(hDC);	//������DC���������ڴ��е�һ�����������
	ReleaseDC(screen_handle, hDC);

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);//&ptr:�ñ�������һ��ָ��DIBλ����ֵ��ָ��,�޸����ݻ�任windows����
	if (screen_hb == NULL) return -3;

	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);		//����ֵΪ�ɵľ�����������豸��������ѡ���ԭ�ȵ�����
	screen_fb = (unsigned char*)ptr;

	AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));//���Ͻǡ����ߡ�����ͻ�����������|ά�ֵ�ǰZ��|��ʾ����
	SetForegroundWindow(screen_handle);//��ָ���������õ�ǰ̨��������
	ShowWindow(screen_handle, SW_NORMAL);

	memset(screen_fb, -100000, w * h * 4);

	return 0;
}

LRESULT WindowsRelated::screen_events(HWND hWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_CLOSE: screen_exit = 1; break;
		//case WM_KEYDOWN: screen_keys[wParam & 511] = 1; break;		//���������¼�
		//case WM_KEYUP: screen_keys[wParam & 511] = 0; break;
		default: return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
}

//��ȡ������Windows��Ϣ������
void WindowsRelated::screen_dispatch(void) {
	MSG msg;
	while (1) {
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
		if (!GetMessage(&msg, NULL, 0, 0)) break;
		DispatchMessage(&msg);
	}
}

void WindowsRelated::screen_update(void) {
	HDC hDC = GetDC(screen_handle);
	BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
	ReleaseDC(screen_handle, hDC);
	screen_dispatch();
}

void* WindowsRelated::getScreenPtr()
{
	return (void *)screen_fb;
}