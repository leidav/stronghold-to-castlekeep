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
}

void imageDeleteList(struct ImageList *image_list)
{
	if (image_list != NULL) {
		for (int i = 0; i < image_list->image_count; i++) {
			free(image_list->images[i].pixel);
		}
		free(image_list->images);
	}
}

void tileObjectDelete(struct TileObject *object)
{
	if (object != NULL) {
		free(object->parts);
	}
}

void tileObjectDeleteList(struct TileObjectList *object_list)
{
	if (object_list != NULL) {
		for (int i = 0; i < object_list->object_count; i++) {
			free(object_list->objects[i].parts);
		}
		free(object_list->objects);
		imageDeleteList(&object_list->image_list);
	}
}

int imageCreate(struct Image *image, int width, int height)
{
	image->width = width;
	image->height = height;
	image->pixel = malloc(sizeof(*image->pixel) * width * height);

	if (image->pixel == NULL) {
		return -1;
	}
	return 0;
}

int imageCreateList(struct ImageList *image_list, int count)
{
	image_list->image_count = count;
	image_list->images = malloc(sizeof(*image_list->images) * count);

	if (image_list->images == NULL) {
		return -1;
	}
	for (int i = 0; i < count; i++) {
		image_list->images[i].pixel = NULL;
	}
	return 0;
}

int tileObjectCreate(struct TileObject *object, int part_count)
{
	object->part_count = part_count;
	object->parts = malloc(sizeof(*object->parts) * part_count);

	if (object->parts == NULL) {
		return -1;
	}
	return 0;
}

int tileObjectCreateList(struct TileObjectList *objects_list, int count)
{
	objects_list->object_count = count;

	if (imageCreateList(&objects_list->image_list, count)) {
		return -1;
	}

	objects_list->objects = malloc(sizeof(*objects_list->objects) * count);

	if (objects_list->objects == NULL) {
		imageDeleteList(&objects_list->image_list);
		return -1;
	}

	for (int i = 0; i < count; i++) {
		objects_list->objects[i].parts = NULL;
	}
	return 0;
}
