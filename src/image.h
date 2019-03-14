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

#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

#define COLOR_MASK_BLUE 0x001F
#define COLOR_MASK_GREEN 0x03E0
#define COLOR_MASK_RED 0x7C00

#define COLOR_CONVERT_BLUE(c) ((uint8_t)((COLOR_MASK_BLUE & (c)) << 3))
#define COLOR_CONVERT_GREEN(c) ((uint8_t)((COLOR_MASK_GREEN & (c)) >> 2))
#define COLOR_CONVERT_RED(c) ((uint8_t)((COLOR_MASK_RED & (c)) >> 7))

#define IMAGE_TYPE_ANIMATION 0x0
#define IMAGE_TYPE_TILE 0x1
#define IMAGE_TYPE_OTHER 0x2

struct Pos {
	int16_t x;
	int16_t y;
};

struct Rect {
	int16_t x;
	int16_t y;
	int16_t width;
	int16_t height;
};

struct Color {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
};

struct Image {
	int16_t x;
	int16_t y;
	int16_t width;
	int16_t height;
	int16_t pitch;
	struct Color *pixel;
};

struct ImageList {
	int type;
	int image_count;
	int pixel_buffer_size;
	struct Image *images;
	void *data;
	uint8_t *free;
	uint8_t *pixel_buffer;
};

struct TilePart {
	uint16_t id;
	int16_t x;
	int16_t y;
	struct Rect rect;
};

struct TileObject {
	uint16_t id;
	int16_t part_count;
	int16_t tile_start;
};

struct TileObjectList {
	int object_count;
	int tile_count;
	struct TileObject *objects;
	struct TilePart *tiles;
};

struct AnimationFrame {
	uint16_t id;
	struct Pos center;
};

struct Animation {
	int frame_count;
	struct AnimationFrame *frames;
};

uint16_t imageGetColor16Bit(uint8_t *data);

int imageCreate(struct Image *image, struct ImageList *image_list, int width,
                int height);

int imageSave(struct Image *image, const char *file);

void imageClear(struct Image *image, uint32_t color);

void imageDelete(struct Image *image, struct ImageList *image_list);

int imageCreateList(struct ImageList *image_list, int pixel_buffer_size,
                    int count, int object_count, int tile_count, int type);

void imageDeleteList(struct ImageList *image_list);

int tileObjectCreate(struct TileObject *object, int part_count,
                     int start_index);

void tileObjectDelete(struct TileObject *object);

int tileObjectCreateList(struct TileObjectList *object_list, int object_count,
                         int tile_count);

void tileObjectDeleteList(struct TileObjectList *object_list);

int animationCreate(struct Animation *animation, int frame_count);

void animationDelete(struct Animation *animation);

int imageWriteData(struct ImageList *image_list, const char *file);

int imagecreateAtlas(struct Image *atlas, struct ImageList *image_list,
                     int width, int sort, int assembled);

#endif  // IMAGE_H
