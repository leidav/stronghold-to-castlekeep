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

#include <stdio.h>
#include <stdlib.h>

#include "image.h"
#include "tgx.h"

int tgxCreateFromFile(struct Tgx *tgx, const char *file)
{
	FILE *fp = fopen(file, "rb");

	if (fp == NULL) {
		return -1;
	}

	if (fseek(fp, 0, SEEK_END)) {
		fclose(fp);
		return -1;
	}

	unsigned int file_size = ftell(fp);

	if (file_size == 0) {
		fclose(fp);
		return -1;
	}
	fseek(fp, 0, SEEK_SET);

	if (fread(&tgx->width, sizeof(tgx->width), 1, fp) < 1 ||
	    fread(&tgx->height, sizeof(tgx->height), 1, fp) < 1) {
		fclose(fp);
		tgxDelete(tgx);
		return -1;
	}
	tgx->size = file_size - ftell(fp);

	tgx->data = malloc(sizeof(*(tgx->data)) * tgx->size);
	if (tgx->data == NULL) {
		fclose(fp);
		tgxDelete(tgx);
		return -1;
	}

	if (fread(tgx->data, sizeof(*(tgx->data)), tgx->size, fp) < tgx->size) {
		fclose(fp);
		tgxDelete(tgx);
		return -1;
	}
	return 0;
}

void tgxDelete(struct Tgx *tgx)
{
	if (tgx != NULL) {
		free(tgx->data);
	}
	free(tgx);
}

int tgxDecode(struct Image *image, struct Rect *rect, uint8_t *data, int size,
              const uint16_t *palette)
{
	int left = rect->x;
	int right = rect->x + rect->width;
	int top = rect->y;
	int bottom = rect->y + rect->height;
	int x = left;
	int y = top;
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
				for (int j = x; j < right; j++) {
					image->pixel[y * image->width + j].a = 0x00;
					image->pixel[y * image->width + j].r = 0x00;
					image->pixel[y * image->width + j].g = 0x00;
					image->pixel[y * image->width + j].b = 0x00;
				}
				if (y < bottom - 1) {
					y++;
					x = left;
				} else {
					return 0;
				}
				break;
			case TGX_TOKEN_PIXEL_STREAM:
				for (int j = 0; j < length; j++) {
					if (palette == NULL) {
						color = data[i];
						i++;
						color |= (data[i] << 8);
					} else {
						color = palette[data[i]];
					}
					i++;
					image->pixel[y * image->width + x].a = 0xFF;
					image->pixel[y * image->width + x].r =
					    COLOR_CONVERT_RED(color);
					image->pixel[y * image->width + x].g =
					    COLOR_CONVERT_GREEN(color);
					image->pixel[y * image->width + x].b =
					    COLOR_CONVERT_BLUE(color);
					if (x < right) {
						x++;
					} else {
						return -1;
					}
				}
				break;
			case TGX_TOKEN_REPEATING_PIXEL:
				if (palette == NULL) {
					color = data[i];
					i++;
					color |= (data[i] << 8);
				} else {
					color = palette[data[i]];
				}
				i++;
				for (int j = 0; j < length; j++) {
					image->pixel[y * image->width + x].a = 0xFF;
					image->pixel[y * image->width + x].r =
					    COLOR_CONVERT_RED(color);
					image->pixel[y * image->width + x].g =
					    COLOR_CONVERT_GREEN(color);
					image->pixel[y * image->width + x].b =
					    COLOR_CONVERT_BLUE(color);
					if (x < right) {
						x++;
					} else {
						return -1;
					}
				}
				break;
			case TGX_TOKEN_TRANSPARENT_PIXEL_STRING:
				for (int j = 0; j < length; j++) {
					image->pixel[y * image->width + x].a = 0x00;
					image->pixel[y * image->width + x].r = 0x00;
					image->pixel[y * image->width + x].g = 0x00;
					image->pixel[y * image->width + x].b = 0x00;
					if (x < right) {
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
                   int size, const uint16_t *palette)
{
	struct Rect rect = {0, 0, width, height};
	if (imageCreate(image, NULL, width, height)) {
		return -1;
	}
	return tgxDecode(image, &rect, data, size, palette);
}
