#pragma once
#pragma once
#include <iostream>
#include <Gl/glut.h>
#include <windows.h>
#include <tchar.h>
#define MAX_CHAR       128
//#include ""
using namespace std;

#define BMP_Header_Length 54

int power_of_two(int nn)
{
	if (nn <= 0)
		return 0;
	return (nn & (nn - 1)) == 0;
}

GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	// ���ļ������ʧ�ܣ�����  
	errno_t err;
	FILE* pFile;
	err = fopen_s(&pFile, file_name, "rb");
	if (err != 0) {
		cout << file_name << " not open" << endl;
		return 0;
	}


	// ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶�  
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ���  
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}

	// �����������ֽ��������ڴ�  
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// ��ȡ��������  
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// �Ծ;ɰ汾�ļ��ݣ����ͼ��Ŀ�Ⱥ͸߶Ȳ��ǵ������η�������Ҫ��������  
	// ��ͼ���߳�����OpenGL�涨�����ֵ��Ҳ����  
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width)
			|| !power_of_two(height)
			|| width > max
			|| height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // �涨���ź��µĴ�СΪ�߳���������  
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// ����ÿ����Ҫ���ֽ��������ֽ���  
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			// �����ڴ�  
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// ������������  
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// �ͷ�ԭ�����������ݣ���pixelsָ���µ��������ݣ�����������width��height  
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// ����һ���µ�������  
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// ���µ������������������������  
	// �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ�  
	GLint lastTextureID = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, lastTextureID);  //�ָ�֮ǰ�������  
	free(pixels);
	return texture_ID;
}

//�洢������Ϣ
struct Pixel {
	GLubyte red, green, blue;
	int x, y;
	bool jun;
} pixel;
//��ȡ���� ��ɫ����1,��ɫ����0
bool readPixel(int x, int y) {
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);

	if ((int)pixel.red <255 || (int)pixel.blue <255 || (int)pixel.green <255) {
		//������ɫ
		pixel.jun = 1;
		//cout << (int)pixel.red << " " << (int)pixel.green << " " << (int)pixel.blue << endl;
	}
	else {
		pixel.jun = 0;
	}
	//cout << pixel.jun << endl;
	return pixel.jun;
}
//��ȡ���� ָ����ɫ����1,���򷴻�0
bool readPixelRGB(int x, int y, int red, int green, int blue) {
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &pixel);

	if ((int)pixel.red == red && (int)pixel.blue == blue && (int)pixel.green == green) {
		//������ɫ
		pixel.jun = 1;
		//cout << (int)pixel.red << " " << (int)pixel.green << " " << (int)pixel.blue << endl;
	}
	else {
		pixel.jun = 0;
	}
	//cout << pixel.jun << endl;
	return pixel.jun;
}

//void read_png(char *file_name) {
//	png_structp png_ptr;
//	png_infop info_ptr;
//	png_uint_32 width, height;
//	int bit_depth, color_type, interlace_type;
//	FILE *fp;
//	if ((fp = fopen(file_name, "rb")) == NULL) {
//		return (ERROR);
//	}
//
//	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
//		0, 0, 0);
//	if (png_ptr == NULL) {
//		fclose(fp);
//		return (ERROR);
//	}
//	info_ptr = png_create_info_struct(png_ptr);
//	if (info_ptr == NULL) {
//		fclose(fp);
//		png_destroy_read_struct(&png_ptr, NULL, NULL);
//		return (ERROR);
//	}
//	if (setjmp(png_jmpbuf(png_ptr))) {
//		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
//		fclose(fp);
//		return (ERROR);
//	}
//	png_init_io(png_ptr, fp);
//	png_read_info(png_ptr, info_ptr);
//	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
//		&color_type, &interlace_type, NULL, NULL);
//	if (color_type == PNG_COLOR_TYPE_PALETTE) {
//		png_set_palette_to_rgb(png_ptr);
//	}
//	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
//		png_set_expand_gray_1_2_4_to_8(png_ptr);
//	}
//	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) != 0) {
//		png_set_tRNS_to_alpha(png_ptr);
//	}
//	png_read_update_info(png_ptr, info_ptr);
//	png_bytep row_pointers[height];
//	int row, col;
//	for (row = 0; row < height; row++) {
//		row_pointers[row] = NULL;
//	}
//	for (row = 0; row < height; row++) {
//		row_pointers[row] = png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
//	}
//	png_read_image(png_ptr, row_pointers);
//	GLubyte * rgba = (GLubyte *)malloc(width * height * 4);
//	int pos = (width * height * 4) - (4 * width);
//	for (row = 0; row < height; row++) {
//		for (col = 0; col < (4 * width); col += 4) {
//			rgba[pos++] = row_pointers[row][col];
//			rgba[pos++] = row_pointers[row][col + 1];
//			rgba[pos++] = row_pointers[row][col + 2];
//			rgba[pos++] = row_pointers[row][col + 3];
//		}
//		pos = (pos - (width * 4) * 2);
//	}
//	//glEnable(GL_TEXTURE_2D);
//	glGenTextures(1, &textureID);
//	printf("%d\n", textureID);
//	//glPixeStorei(GL_UNPACK_ALIGNMENT, 1);
//	glBindTexture(GL_TEXTURE_2D, textureID);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0
//		, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	free(rgba);
//	fclose(fp);
//}