/**
 *	Copyright (C) 2014 David Leiter
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <png.h>

#include "image.h"

int imageSave(struct Image *image, const char *file)
{
	FILE *fp = fopen(file, "wb");
	if (fp == NULL) {
		return -1;
	}
	png_structp png_ptr =
	    png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return -1;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		fclose(fp);
		return -1;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		return -1;
	}

	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, image->width, image->height, 8,
	             PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
	             PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);

	png_set_bgr(png_ptr);
	for (int i = 0; i < image->height; i++) {
		png_write_row(png_ptr, (png_bytep)&image->pixel[i * image->width]);
	}

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	return 0;
}

uint16_t imageGetColor16Bit(uint8_t *data)
{
	return (*data) | (*(data + 1) << 8);
}

void imageClear(struct Image *image, uint32_t color)
{
	for (int i = 0; i < image->width * image->height; i++) {
		image->pixel[i] = *(struct Color *)&color;
	}
}

void imageDelete(struct Image *image)
{
	if (image != NULL) {
		free(image->pixel);
	}
	free(image);
}

void imageDeleteList(struct ImageList *image_list)
{
	if (image_list != NULL) {
		for (int i = 0; i < image_list->image_count; i++) {
			free(image_list->images[i].pixel);
		}
		free(image_list->images);
	}
	free(image_list);
}
