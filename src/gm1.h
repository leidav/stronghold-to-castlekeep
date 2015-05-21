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

#ifndef GM1_H
#define GM1_H

#include "image.h"

#include <stdint.h>

#define GM1_PALETTE_SIZE 256
#define GM1_PALETTE_COUNT 10

#define GM1_DATA_TGX 1
#define GM1_DATA_ANIMATION 2
#define GM1_DATA_TGX_AND_TILE 3
#define GM1_DATA_TGX_FONT 4
#define GM1_DATA_BITMAP 5
#define GM1_DATA_TGX_CONST_SIZE 6
#define GM1_DATA_BITMAP_OTHER 7

#define GM1_SIZE_UNDEFINDED 0
#define GM1_SIZE_30x30 1
#define GM1_SIZE_55x55 2
#define GM1_SIZE_75x75 3
#define GM1_SIZE_UNKNOWN_4 4
#define GM1_SIZE_100x100 5
#define GM1_SIZE_110x110 6
#define GM1_SIZE_130x130 7
#define GM1_SIZE_UNKNOWN_8 8
#define GM1_SIZE_185x185 9
#define GM1_SIZE_250x250 10
#define GM1_SIZE_180x180 11

#define GM1_TILE_WIDTH 30
#define GM1_TILE_HEIGHT 16

struct Gm1FileHeader {
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;

	/*number of total images in the file*/
	uint32_t image_count;

	uint32_t unknown4;

	uint32_t data_type;

	uint32_t unknown5;
	uint32_t unknown6;

	/*size type of the images*/
	uint32_t size_type;

	uint32_t unknown7;
	uint32_t unknown8;
	uint32_t unknown9;

	/*maybe x size of images*/
	uint32_t width;
	/*maybe y size of images*/
	uint32_t height;

	uint32_t unknown12;
	uint32_t unknown13;
	uint32_t unknown14;
	uint32_t unknown15;
	uint32_t center_x;
	uint32_t center_y;

	/*The size of file left to read in uint8_ts.*/
	uint32_t data_size;

	uint32_t unknown18;
};

struct Gm1ImageHeader {
	uint16_t image_width;
	uint16_t image_height;
	uint16_t position_x;
	uint16_t position_y;
	uint8_t part;
	uint8_t parts;
	uint16_t tile_position_y;
	uint8_t tile_placement_alignment;
	uint8_t horizontal_offset;
	uint8_t drawing_box_width;
	uint8_t performance_id;
};

struct Gm1 {
	struct Gm1FileHeader header;
	/* 10*256 colors */
	uint16_t *palette;
	uint32_t *image_offset_list;
	uint32_t *image_size_list;
	struct Gm1ImageHeader *image_headers;
	uint8_t *image_data;
};

int gm1SaveHeader(struct Gm1 *gm1, const char *file);

int gm1CreateFromFile(struct Gm1 *gm1, const char *file);

int gm1CreateImageList(struct ImageList *image_list, struct Gm1 *Gm1,
                       int palette);

int gm1CreateTileObjectList(struct ImageList *image_list, struct Gm1 *gm1);

void gm1Delete(struct Gm1 *Gm1);

int gm1IsTileObject(struct Gm1 *gm1);

int gm1IsAnimation(struct Gm1 *gm1);

int gm1SavePalette(struct Gm1 *gm1, const char *file);

int gm1CreatePaletteImage(struct Image *image, const uint16_t *palette,
                          int size);

#endif  // GM1_H
