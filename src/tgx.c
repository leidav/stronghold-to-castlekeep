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
#include <stdio.h>

#include "tgx.h"
#include "image.h"

struct Tgx *tgxCreateFromFile(const char *file)
{
	FILE *fp = fopen(file, "rb");

	if (fp == NULL) {
		return NULL;
	}

	if (fseek(fp, 0, SEEK_END)) {
		fclose(fp);
		return NULL;
	}

	unsigned int file_size = ftell(fp);

	if (file_size == 0) {
		fclose(fp);
		return NULL;
	}
	fseek(fp, 0, SEEK_SET);

	struct Tgx *tgx = malloc(sizeof(*tgx));

	if (tgx == NULL) {
		fclose(fp);
		return NULL;
	}

	if (fread(&tgx->width, sizeof(tgx->width), 1, fp) < 1 ||
	    fread(&tgx->height, sizeof(tgx->height), 1, fp) < 1) {
		fclose(fp);
		tgxDelete(tgx);
		return NULL;
	}
	tgx->size = file_size - ftell(fp);

	tgx->data = malloc(sizeof(*(tgx->data)) * tgx->size);
	if (tgx->data == NULL) {
		fclose(fp);
		tgxDelete(tgx);
		return NULL;
	}

	if (fread(tgx->data, sizeof(*(tgx->data)), tgx->size, fp) < tgx->size) {
		fclose(fp);
		tgxDelete(tgx);
		return NULL;
	}
	return tgx;
}

void tgxDelete(struct Tgx *tgx)
{
	if (tgx != NULL) {
		free(tgx->data);
	}
	free(tgx);
}

int tgxDecode(struct Color *pixel, int width, int height, int horizontal_offset,
              uint8_t *data, int size, uint16_t *palette, int use_palette)
{
	int x = 0;
	int y = 0;
	int i = 0;
	int type = 0;
	int length = 0;
	uint16_t color = 0;

	while (i < size) {
		type = TGX_GET_TOKEN_TYPE(data[i]);
		length = TGX_GET_TOKEN_VALUE(data[i]) + 1;
		i++;
		switch (type) {
			case TGX_TOKEN_NEW_LINE:
				for (int j = x; j < width; j++) {
					pixel[y * width + horizontal_offset + j].a = 0x00;
					pixel[y * width + horizontal_offset + j].r = 0x00;
					pixel[y * width + horizontal_offset + j].g = 0x00;
					pixel[y * width + horizontal_offset + j].b = 0x00;
				}
				if (y < height - 1) {
					y++;
					x = 0;
				} else {
					fprintf(stderr, "y to large\n");
					return 0;
				}
				break;
			case TGX_TOKEN_PIXEL_STREAM:
				for (int j = 0; j < length; j++) {
					if (use_palette == 0) {
						color = data[i];
						i++;
						color |= (data[i] << 8);
					} else {
						color = palette[data[i]];
					}
					i++;
					pixel[y * width + horizontal_offset + x].a = 0xFF;
					pixel[y * width + horizontal_offset + x].r =
					    COLOR_CONVERT_RED(color);
					pixel[y * width + horizontal_offset + x].g =
					    COLOR_CONVERT_GREEN(color);
					pixel[y * width + horizontal_offset + x].b =
					    COLOR_CONVERT_BLUE(color);
					if (x < width) {
						x++;
					} else {
						return -1;
					}
				}
				break;
			case TGX_TOKEN_REPEATING_PIXEL:
				if (use_palette == 0) {
					color = data[i];
					i++;
					color |= (data[i] << 8);
				} else {
					color = palette[data[i]];
				}
				i++;
				for (int j = 0; j < length; j++) {
					pixel[y * width + horizontal_offset + x].a = 0xFF;
					pixel[y * width + horizontal_offset + x].r =
					    COLOR_CONVERT_RED(color);
					pixel[y * width + horizontal_offset + x].g =
					    COLOR_CONVERT_GREEN(color);
					pixel[y * width + horizontal_offset + x].b =
					    COLOR_CONVERT_BLUE(color);
					if (x < width) {
						x++;
					} else {
						return -1;
					}
				}
				break;
			case TGX_TOKEN_TRANSPARENT_PIXEL_STRING:
				for (int j = 0; j < length; j++) {
					pixel[y * width + horizontal_offset + x].a = 0x00;
					pixel[y * width + horizontal_offset + x].r = 0x00;
					pixel[y * width + horizontal_offset + x].g = 0x00;
					pixel[y * width + horizontal_offset + x].b = 0x00;
					if (x < width) {
						x++;
					} else {
						return -1;
					}
				}
				break;
			default:
				return -1;
				break;
		}
	}
	return 0;
}

int tgxCreateImage(struct Image *image, int width, int height, uint8_t *data,
                   int size, uint16_t *palette, int use_palette)
{
	image->width = width;
	image->height = height;
	image->pixel = malloc(sizeof(*image->pixel) * width * height);

	if (image->pixel == NULL) {
		return -1;
	}
	return tgxDecode(image->pixel, width, height, 0, data, size, palette,
	                 use_palette);
}
