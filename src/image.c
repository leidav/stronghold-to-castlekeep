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
	} else if (type == IMAGE_TYPE_ANIMATION) {
		image_list->data = malloc(sizeof(struct Animation));
		if (image_list->data == NULL) {
			free(image_list->images);
			return -1;
		}
		if (animationCreate(image_list->data, count)) {
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
			imageDelete(&image_list->images[i]);
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

void tileObjectDeleteList(struct TileObjectList *object_list)
{
	if (object_list != NULL) {
		for (int i = 0; i < object_list->object_count; i++) {
			tileObjectDelete(&object_list->objects[i]);
		}
		free(object_list->objects);
	}
}

int animationCreate(struct Animation *animation, int frame_count)
{
	animation->frame_count = frame_count;
	animation->frames = malloc(sizeof(*animation->frames) * frame_count);

	if (animation->frames == NULL) {
		return -1;
	}

	return 0;
}

void animationDelete(struct Animation *animation)
{
	if (animation != NULL) {
		free(animation->frames);
	}
}

static void writeTileObject(FILE *fp, struct TileObject *object)
{
	fprintf(fp,
	        "    {\n"
	        "      \"id\": %d,\n"
	        "      \"part_count\": %d,\n"
	        "      \"parts\": [\n",
	        object->id, object->part_count);

	for (int i = 0; i < object->part_count; i++) {
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
		        object->parts[i].id, object->parts[i].x, object->parts[i].y,
		        object->parts[i].rect.x, object->parts[i].rect.y,
		        object->parts[i].rect.width, object->parts[i].rect.height);
		if (i < object->part_count - 1) {
			fputs("        },\n", fp);
		} else {
			fputs("        }\n", fp);
		}
	}
	fputs("      ]\n", fp);
}

static void writeAnimationFrame(FILE *fp, struct AnimationFrame *frame)
{
	fprintf(fp,
	        "    {\n"
	        "      \"id\": %d,\n"
	        "      \"center\": {\n"
	        "            \"x\": %d,\n"
	        "            \"y\": %d\n"
	        "      }\n",
	        frame->id, frame->center.x, frame->center.y);
}

static void writeImages(FILE *fp, struct ImageList *image_list)
{
	fputs("  \"images\": [\n", fp);

	for (int i = 0; i < image_list->image_count; i++) {
		fprintf(fp,
		        "    {\n"
		        "      \"id\": %d,\n"
		        "      \"width\": %d,\n"
		        "      \"heigth\": %d\n",
		        i, image_list->images[i].width, image_list->images[i].height);
		if (i < image_list->image_count - 1) {
			fputs("    },\n", fp);
		} else {
			fputs("    }\n", fp);
		}
	}
	fputs("  ]", fp);
}
static int writeData(FILE *fp, struct ImageList *image_list)
{
	struct TileObjectList *object_list =
	    (struct TileObjectList *)image_list->data;
	struct Animation *animation = (struct Animation *)image_list->data;

	fputs("  \"data\": [\n", fp);

	if (image_list->type != IMAGE_TYPE_OTHER && image_list->data != NULL) {
		for (int i = 0; i < image_list->image_count; i++) {
			if (image_list->type == IMAGE_TYPE_TILE) {
				writeTileObject(fp, &object_list->objects[i]);
			} else if (image_list->type == IMAGE_TYPE_ANIMATION) {
				writeAnimationFrame(fp, &animation->frames[i]);
			} else {
				return -1;
			}

			if (i < image_list->image_count - 1) {
				fputs("    },\n", fp);
			} else {
				fputs("    }\n", fp);
			}
		}
	}
	fputs("  ]", fp);
	return 0;
}

int imageSaveData(struct ImageList *image_list, const char *file)
{
	FILE *fp = fopen(file, "w");
	if (fp == NULL) {
		return -1;
	}

	const static char *type_lookup[] = {"anim", "tile", "other"};

	fprintf(fp,
	        "{\n"
	        "  \"count\": %d,\n"
	        "  \"type\": \"%s\",\n",
	        image_list->image_count, type_lookup[image_list->type]);
	writeImages(fp, image_list);
	fputs(",\n", fp);
	writeData(fp, image_list);
	fputs("\n", fp);
	fputs("}\n", fp);

	fclose(fp);

	return 0;
}
