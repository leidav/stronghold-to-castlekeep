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

#ifndef TGX_H
#define TGX_H

#include <stdint.h>

#define TGX_TOKEN_PIXEL_STREAM 0x00
#define TGX_TOKEN_TRANSPARENT_PIXEL_STRING 0x20
#define TGX_TOKEN_REPEATING_PIXEL 0x40
#define TGX_TOKEN_NEW_LINE 0x80

#define TGX_TOKEN_MASK_TYPE 0xE0
#define TGX_TOKEN_MASK_VALUE 0x1F

#define TGX_GET_TOKEN_TYPE(t) ((int)(t) & TGX_TOKEN_MASK_TYPE)
#define TGX_GET_TOKEN_VALUE(t) ((int)((t)&TGX_TOKEN_MASK_VALUE))

struct Image;
struct Color;
struct Rect;

struct Tgx
{
	uint32_t width;
	uint32_t height;
	int size;
	uint8_t *data;
};

struct Tgx *tgxCreateFromFile(const char *file);

int tgxDecode(struct Image *image, struct Rect rect, uint8_t *data, int size,
              uint16_t *palette);

int tgxCreateImage(struct Image *image, int width, int height, uint8_t *data,
                   int size, uint16_t *palette);

void tgxDelete(struct Tgx *tgx);
#endif  // TGX_H
