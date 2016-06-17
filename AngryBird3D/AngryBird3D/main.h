#ifndef _main_h
#define _main_h

#include <string>
#include <iostream>
#include "WindowsRelated.h"
#include "Application.h"

#define RENDER_STATE_WIREFRAME      1		// ��Ⱦ�߿�
#define RENDER_STATE_TEXTURE        2		// ��Ⱦ����
#define RENDER_STATE_COLOR          4		// ��Ⱦ��ɫ

extern int screen_w, screen_h;					//��Ļ���
extern int screen_exit;					//������Ļ�˳�

typedef unsigned int IUINT32;

typedef struct { float m[4][4];
} matrix_t;
typedef struct { float x, y, z, w; } vector_t;
typedef vector_t point_t;

typedef struct { float r, g, b; } color_t;
typedef struct { float u, v; } texcoord_t;
typedef struct { point_t pos; texcoord_t tc; color_t color; float rhw; } vertex_t;

typedef struct { vertex_t v, v1, v2; } edge_t;	//v1:����ĵ㣬v2������ĵ㣬v����ֵ�ĵ㣬��ʱʹ��
typedef struct { float top, bottom; edge_t left, right; } trapezoid_t;	//y��Сֵ��y���ֵ����ߵıߣ��ұߵı�
typedef struct { vertex_t v, step; int x, y, w; } scanline_t;	//��y��,��ʼλ��x������w, ���Ϊ����ߵĶ���v��֮�����

typedef struct { 
	matrix_t world;         // ��������任
	matrix_t view;          // ��Ӱ������任
	matrix_t projection;    // ͶӰ�任
	matrix_t transform;     // transform = world * view * projection
	float w, h;             // ��Ļ��С
}	transform_t;

using namespace std;

#endif