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

#define IMAGE_TYPE_ANIMATION
#define IMAGE_TYPE_TILE
#define IMAGE_TYPE_OTHER

struct Pos {
	int x;
	int y;
};

struct Rect {
	int x;
	int y;
	int width;
	int height;
};

struct Color {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
};

struct Image {
	int width;
	int height;
	struct Color *pixel;
};

struct ImageList {
	int type;
	int image_count;
	struct Image *images;
};

struct TilePart {
	int id;
	int x;
	int y;
	struct Rect rect;
};

struct TileObject {
	int part_count;
	struct TilePart *parts;
};

struct TileObjectList {
	int object_count;
	struct ImageList image_list;
	struct TileObject *objects;
};

struct AnimationFrame {
	int num;
	struct Pos center;
};

struct Animation {
	int frame_count;
	int fps;
	struct AnimationFrame *frames;
	struct Image *images;
};

uint16_t imageGetColor16Bit(uint8_t *data);

int imageCreate(struct Image *image, int width, int height);

int imageSave(struct Image *image, const char *file);

void imageClear(struct Image *image, uint32_t color);

void imageDelete(struct Image *image);

int imageCreateList(struct ImageList *image_list, int count);

void imageDeleteList(struct ImageList *image_list);

int tileObjectCreate(struct TileObject *object, int part_count);

void tileObjectDelete(struct TileObject *object);

int tileObjectCreateList(struct TileObjectList *objects_list, int count);

void tileObjectDeleteList(struct TileObjectList *object_list);

#endif  // IMAGE_H
