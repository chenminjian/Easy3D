#ifndef _windows_related_h
#define _windows_related_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <windows.h>
#include <tchar.h>

class WindowsRelated
{
public:
	WindowsRelated();
	int screen_init(int w,int h,TCHAR * title);
	static LRESULT screen_events(HWND hWnd, UINT msg, 
		WPARAM wParam, LPARAM lParam);
	void screen_dispatch(void);
	void screen_update(void);

	//by me------------------
	void* getScreenPtr();
protected:
private:
		HWND screen_handle;		// ������ HWND
		HDC screen_dc;			// ���׵� HDC
		HBITMAP screen_hb;		// DIB     �����һ����ʶ����������ʶ���������Ŀ
		HBITMAP screen_ob;		// �ϵ� BITMAP
		unsigned char *screen_fb;		// frame buffer,�޸��������ݾͻ��޸�Windows����
};

#endif