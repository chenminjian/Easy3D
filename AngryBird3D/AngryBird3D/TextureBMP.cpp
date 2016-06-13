#include "TextureBMP.h"
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <string>

unsigned int TextureBMP::Width()
{
	return width;
}
unsigned int TextureBMP::Height()
{
	return height;
}

unsigned int TextureBMP::ImageSize()
{
	return height*width*3;
}

unsigned int TextureBMP::PixelColor(int y,int x)
{
	unsigned char r,g,b;
	unsigned int result = 0;

	unsigned int position = (y * width + x)*3;

	b = data[position];				//��BGR˳��洢
	g = data[position + 1];
	r = data[position + 2];

	
	result = (int)r;
	result = result<<8 +(int)g;
	result = result<<8 +(int)b;
	

	int result2 = ((int)r)<<16|((int)g)<<8|((int)b);

	return result2;
}

TextureBMP::TextureBMP(const char* imagePath)
{

	unsigned char header[54];	//ͷ����Ϣ
	unsigned int dataPos;		//ʵ�����ݿ�ʼ��λ��
	unsigned int imageSize;		//width*height*3


	FILE* file = fopen(imagePath,"rb");
	if (!file)
	{
		printf("can't find image in %s",imagePath);
	}

	if (fread(header,1,54,file)!=54)
	{
		printf("Not a correct BMP file,�ﲻ���ײ��ĳ���\n");

	}

	if (header[0]!= 'B'||header[1]!='M')
	{
		printf("Not a correct BMP file����BM\n");
	}

	//ȷ������һ��24bpp file
	if (*(int*)&(header[0x1E])!= 0) 
	{
		printf("Not a correct BMP file����24bpp\n");
	}
	if (*(int*)&(header[0x1C])!= 24)	//�ֽ�Խ����Խ��18 00 00 00ָ����0x18
	{
		printf("Not a correct BMP file����24bpp����%d\n",*(int*)&(header[0x1E]));
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	int num = *(int*)&(header[0x0a]);


	if (imageSize == 0)
	{
		imageSize = width * height *3;
	}
	if (dataPos == 0)
	{
		dataPos = 54;
	}

	data = new unsigned char[imageSize];

	fread(data,1,imageSize,file);

	fclose(file);
}



TextureBMP::~TextureBMP()
{
	delete []data;
}
