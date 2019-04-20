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

#include <png.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"

uint16_t imageGetColor16Bit(uint8_t *data)
{
	return (*data) | (*(data + 1) << 8);
}

int imageCreate(struct Image *image, struct ImageList *image_list, int width,
                int height)
{
	image->x = 0;
	image->y = 0;
	image->width = width;
	image->height = height;
	image->pitch = width;
	image->pixel = NULL;
	if (image_list == NULL) {
		image->pixel = malloc(sizeof(*image->pixel) * width * height);
	} else {
		int allocation_size = sizeof(*image->pixel) * width * height;
		if ((image_list->free + allocation_size) <
		    (image_list->pixel_buffer + image_list->pixel_buffer_size)) {
			image->pixel = (struct Color *)image_list->free;
			image_list->free += allocation_size;
		} else {
			return -1;
		}
	}
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
	struct Color c;
	memcpy(&c, &color, sizeof(c));
	for (int i = 0; i < image->width * image->height; i++) {
		image->pixel[i] = c;
	}
}

void imageDelete(struct Image *image, struct ImageList *image_list)
{
	if ((image_list == NULL) && (image != NULL)) {
		free(image->pixel);
	}
}

int imageCreateList(struct ImageList *image_list, int pixel_buffer_size,
                    int count, int object_count, int tile_count, int type)
{
	image_list->image_count = count;
	image_list->type = type;
	image_list->pixel_buffer_size = pixel_buffer_size;
	image_list->images = malloc(sizeof(*image_list->images) * count);
	image_list->pixel_buffer = malloc(pixel_buffer_size);
	image_list->free = image_list->pixel_buffer;

	if (image_list->images == NULL) {
		return -1;
	}
	if (image_list->pixel_buffer == NULL) {
		return -1;
	}

	if (type == IMAGE_TYPE_TILE) {
		image_list->data = malloc(sizeof(struct TileObjectList));
		if (image_list->data == NULL) {
			free(image_list->images);
			return -1;
		}
		if (tileObjectCreateList(image_list->data, object_count, tile_count)) {
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
		if (animationCreate(image_list->data, object_count)) {
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
			imageDelete(&image_list->images[i], image_list);
		}
		free(image_list->images);
		free(image_list->pixel_buffer);
		if (image_list->data != NULL) {
			if (image_list->type == IMAGE_TYPE_TILE) {
				tileObjectDeleteList(image_list->data);
			}
			free(image_list->data);
		}
	}
}

int tileObjectCreate(struct TileObject *object, int part_count, int start_index)
{
	object->part_count = part_count;
	object->tile_start = start_index;
	return 0;
}

void tileObjectDelete(struct TileObject *object) {}

int tileObjectCreateList(struct TileObjectList *objects_list, int object_count,
                         int tile_count)
{
	objects_list->object_count = object_count;
	objects_list->tile_count = tile_count;
	objects_list->objects =
	    malloc(sizeof(*objects_list->objects) * object_count);
	objects_list->tiles = malloc(sizeof(*objects_list->tiles) * tile_count);

	if (objects_list->objects == NULL) {
		return -1;
	}
	if (objects_list->tiles == NULL) {
		return -1;
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
		free(object_list->tiles);
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

int imageWriteData(struct ImageList *image_list, const char *file)
{
	FILE *fp = fopen(file, "w");
	if (fp == NULL) {
		return -1;
	}

	int type = image_list->type;
	if (type == IMAGE_TYPE_TILE) {
		fprintf(fp, "!tile\n");
	} else if (type == IMAGE_TYPE_ANIMATION) {
		fprintf(fp, "!anim\n");
	} else {
		fprintf(fp, "!other\n");
	}

	fprintf(fp, "[images,%d,4,i,i,i,i]\n", image_list->image_count);
	fprintf(fp, "#posx,posy,width,height\n");
	for (int i = 0; i < image_list->image_count; i++) {
		fprintf(fp, "%d,%d,%d,%d\n", image_list->images[i].x,
		        image_list->images[i].y, image_list->images[i].width,
		        image_list->images[i].height);
	}
	if (type == IMAGE_TYPE_TILE) {
		struct TileObjectList *objects =
		    (struct TileObjectList *)image_list->data;
		fprintf(fp, "[objects,%d,2,i,i]\n", objects->object_count);
		fprintf(fp, "#tile_start,tiles\n");
		int num_tiles = 0;
		for (int i = 0; i < objects->object_count; i++) {
			fprintf(fp, "%d,%d\n", num_tiles, objects->objects[i].part_count);
			num_tiles += objects->objects[i].part_count;
		}
		fprintf(fp, "[tiles,%d,6,i,i,i,i,i,i]\n", num_tiles);
		fprintf(fp, "#x,y,posx,posy,width,height\n");
		for (int i = 0; i < objects->tile_count; i++) {
			fprintf(fp, "%d,%d,%d,%d,%d,%d\n", objects->tiles[i].x,
			        objects->tiles[i].y, objects->tiles[i].rect.x,
			        objects->tiles[i].rect.y, objects->tiles[i].rect.width,
			        objects->tiles[i].rect.height);
		}
	} else if (type == IMAGE_TYPE_ANIMATION) {
		struct Animation *animation = (struct Animation *)image_list->data;
		fprintf(fp, "[animation,%d,2,i,i]\n", animation->frame_count);
		for (int i = 0; i < animation->frame_count; i++) {
			fprintf(fp, "%d,%d\n", animation->frames[i].center.x,
			        animation->frames[i].center.y);
		}
	}
	return 0;
}

static void boundingBox(struct Rect *bbox, struct Image *image)
{
	int minx = image->width;
	int miny = image->height;
	int maxx = 0;
	int maxy = 0;
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			if (image->pixel[y * image->width + x].a == 0xFF) {
				if (minx >= x) {
					minx = x;
				}
				if (miny >= y) {
					miny = y;
				}
				if (maxx <= x) {
					maxx = x;
				}
				if (maxy <= y) {
					maxy = y;
				}
			}
		}
	}
	bbox->x = minx;
	bbox->y = miny;
	bbox->width = maxx - minx;
	bbox->height = maxy - miny;
}

struct CmpVal {
	uint16_t id;
	uint16_t height;
};
struct Offset {
	int16_t x;
	int16_t y;
};

static int heightCmp(const void *a, const void *b)
{
	const struct CmpVal *img_a = a;
	const struct CmpVal *img_b = b;
	return img_a->height - img_b->height;
}

void shrinkAnimationImages(struct ImageList *image_list,
                           struct Offset *source_offsets)
{
	struct Animation *animation = (struct Animation *)image_list->data;
	for (int i = 0; i < image_list->image_count; i++) {
		struct Rect bbox;
		boundingBox(&bbox, &image_list->images[i]);
		image_list->images[i].width = bbox.width;
		image_list->images[i].height = bbox.height;
		animation->frames[i].center.x -= bbox.x;
		animation->frames[i].center.y -= bbox.y;
		source_offsets[i].x = bbox.x;
		source_offsets[i].y = bbox.y;
	}
}

static int layout(struct ImageList *image_list, struct Rect *atlas_size,
                  struct Offset *image_offsets, int width, int sort,
                  int assembled)
{
	if (image_list->type == IMAGE_TYPE_ANIMATION) {
		shrinkAnimationImages(image_list, image_offsets);
	}

	struct CmpVal *vals = malloc(sizeof(*vals) * image_list->image_count);
	if (vals == NULL) {
		return -1;
	}

	for (int i = 0; i < image_list->image_count; i++) {
		vals[i].id = (uint16_t)i;
		vals[i].height = (uint16_t)image_list->images[i].height;
	}
	if (sort && (image_list->type != IMAGE_TYPE_ANIMATION)) {
		qsort(vals, image_list->image_count, sizeof(*vals), heightCmp);
	}

	int x = 0;
	int posx = 0;
	int posy = 0;
	int maxy = 0;
	int maxx = 0;
	for (int i = 0; i < image_list->image_count; i++) {
		struct Image *image = &image_list->images[vals[i].id];
		if ((posx + image->width + 1) > width) {
			if (x == 0) {
				free(vals);
				return -1;
			}
			x = 0;
			posx = 0;
			posy = maxy + 1;
		} else {
			x++;
		}
		image->x = posx;
		image->y = posy;

		if (assembled && (image_list->type == IMAGE_TYPE_TILE)) {
			struct TileObjectList *tile_objects = image_list->data;
			struct TileObject *object = &tile_objects->objects[vals[i].id];
			for (int j = object->tile_start;
			     j < object->tile_start + object->part_count; j++) {
				tile_objects->tiles[j].rect.x += posx;
				tile_objects->tiles[j].rect.y += posy;
			}
		}

		posx = posx + image->width + 1;
		if (posx > maxx) {
			maxx = posx;
		}
		if ((posy + image->height) > maxy) {
			maxy = posy + image->height;
		}
	}
	if ((!assembled) && (image_list->type == IMAGE_TYPE_TILE)) {
		struct TileObjectList *tile_objects = image_list->data;
		for (int i = 0; i < tile_objects->tile_count; i++) {
			tile_objects->tiles[i].rect.x += image_list->images[i].x;
			tile_objects->tiles[i].rect.y += image_list->images[i].y;
		}
	}
	atlas_size->x = 0;
	atlas_size->y = 0;
	atlas_size->width = maxx;
	atlas_size->height = maxy;

	free(vals);
	return 0;
}

static void placeImage(struct Image *atlas, struct Offset offset,
                       struct Image *image)
{
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			int posx = image->x + x;
			int posy = image->y + y;
			atlas->pixel[posy * atlas->width + posx] =
			    image->pixel[(y + offset.y) * (image->pitch) + x + offset.x];
		}
	}
}

int imagecreateAtlas(struct Image *atlas, struct ImageList *image_list,
                     int width, int sort, int assembled)
{
	struct Rect atlas_size;
	struct Offset *image_offsets = NULL;
	if (image_list->type == IMAGE_TYPE_ANIMATION) {
		image_offsets =
		    malloc(sizeof(*image_offsets) * image_list->image_count);
	}
	if (layout(image_list, &atlas_size, image_offsets, width, sort,
	           assembled) != 0) {
		return -1;
	}

	if (atlas_size.width <= (width / 2)) {
		width = width / 2;
	}
	imageCreate(atlas, NULL, width, atlas_size.height);
	imageClear(atlas, 0x00);
	for (int i = 0; i < image_list->image_count; i++) {
		struct Offset offset = {0, 0};
		if (image_list->type == IMAGE_TYPE_ANIMATION) {
			offset = image_offsets[i];
		}
		placeImage(atlas, offset, &image_list->images[i]);
	}
	free(image_offsets);
	return 0;
}
