#ifndef _texture_bmp_h
#define _texture_bmp_h

class TextureBMP
{
private:
	unsigned char* data;	//ʵ������
	unsigned int width;		//ͼƬ���
	unsigned int height;	//ͼƬ�߶�

public:
	TextureBMP(const char* imagePath);
	~TextureBMP();
	unsigned int Width();
	unsigned int Height();
	unsigned int ImageSize();
	unsigned int PixelColor(int y,int x);
};

#endif