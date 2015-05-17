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

uint16_t imageGetColor16Bit(uint8_t *data)
{
	return (*data) | (*(data + 1) << 8);
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

int imageCreateList(struct ImageList *image_list, int count, int type)
{
	image_list->image_count = count;
	image_list->type = type;
	image_list->images = malloc(sizeof(*image_list->images) * count);

	if (image_list->images == NULL) {
		return -1;
	}

	if (type == IMAGE_TYPE_TILE) {
		image_list->data = malloc(sizeof(struct TileObjectList));
		if (image_list->data == NULL) {
			free(image_list->images);
			return -1;
		}
		if (tileObjectCreateList(image_list->data, count)) {
			free(image_list->data);
			free(image_list->images);
			return -1;
		}
	} else {
		image_list->data = NULL;
	}

	for (int i = 0; i < count; i++) {
		image_list->images[i].pixel = NULL;
	}
	return 0;
}

void imageDeleteList(struct ImageList *image_list)
{
	if (image_list != NULL) {
		for (int i = 0; i < image_list->image_count; i++) {
			free(image_list->images[i].pixel);
		}
		free(image_list->images);
		if (image_list->data != NULL) {
			if (image_list->type == IMAGE_TYPE_TILE) {
				tileObjectDeleteList(image_list->data);
			}
			free(image_list->data);
		}
	}
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

void tileObjectDelete(struct TileObject *object)
{
	if (object != NULL) {
		free(object->parts);
	}
}

int tileObjectCreateList(struct TileObjectList *objects_list, int count)
{
	objects_list->object_count = count;
	objects_list->objects = malloc(sizeof(*objects_list->objects) * count);

	if (objects_list->objects == NULL) {
		return -1;
	}

	for (int i = 0; i < count; i++) {
		objects_list->objects[i].parts = NULL;
	}
	return 0;
}

static void writeTileParts(FILE *fp, struct TilePart *parts, int count)
{
	for (int i = 0; i < count; i++) {
		fprintf(fp,
		        "        {\n"
		        "          \"id\": %d,\n"
		        "          \"x\": %d,\n"
		        "          \"y\": %d,\n"
		        "          \"rect\": {\n"
		        "            \"x\": %d,\n"
		        "            \"y\": %d,\n"
		        "            \"width\": %d,\n"
		        "            \"height\": %d\n"
		        "          }\n",
		        parts[i].id, parts[i].x, parts[i].y, parts[i].rect.x,
		        parts[i].rect.y, parts[i].rect.width, parts[i].rect.height);
		if (i < count - 1) {
			fputs("        },\n", fp);
		} else {
			fputs("        }\n", fp);
		}
	}
}

static void writeObjects(FILE *fp, struct ImageList *image_list)
{
	struct TileObjectList *object_list =
	    (struct TileObjectList *)image_list->data;
	for (int i = 0; i < image_list->image_count; i++) {
		fprintf(fp,
		        "    {\n"
		        "      \"id\": %d,\n"
		        "      \"width\": %d,\n"
		        "      \"heigth\": %d,\n"
		        "      \"part_count\": %d,\n"
		        "      \"parts\": [\n",
		        i, image_list->images[i].width, image_list->images[i].height,
		        object_list->objects[i].part_count);
		writeTileParts(fp, object_list->objects[i].parts,
		               object_list->objects[i].part_count);
		fputs("      ]\n", fp);
		if (i < object_list->object_count - 1) {
			fputs("    },\n", fp);
		} else {
			fputs("    }\n", fp);
		}
	}
}

int tileObjectSaveData(struct ImageList *image_list, const char *file)
{
	if (image_list->type != IMAGE_TYPE_TILE) {
		return -1;
	}

	FILE *fp = fopen(file, "w");
	if (fp == NULL) {
		return -1;
	}

	fprintf(fp,
	        "{\n"
	        "  \"object_count\": %d,\n"
	        "  \"objects\": [\n",
	        image_list->image_count);
	writeObjects(fp, image_list);
	fputs(
	    "  ]\n"
	    "}\n",
	    fp);

	fclose(fp);

	return 0;
}

void tileObjectDeleteList(struct TileObjectList *object_list)
{
	if (object_list != NULL) {
		for (int i = 0; i < object_list->object_count; i++) {
			free(object_list->objects[i].parts);
		}
		free(object_list->objects);
	}
}
