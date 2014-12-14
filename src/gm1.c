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

#include "gm1.h"
#include "tgx.h"
#include "image.h"

struct Gm1 *gm1CreateFromFile(const char *file)
{
	FILE *fp = fopen(file, "rb");
	if (fp == NULL) {
		return NULL;
	}
	if (fseek(fp, 0, SEEK_END)) {
		fclose(fp);
		return NULL;
	}
	int file_size = ftell(fp);
	if (file_size == 0) {
		fclose(fp);
		return NULL;
	}
	fseek(fp, 0, SEEK_SET);

	struct Gm1 *gm1 = malloc(sizeof(*gm1));

	/*read gm1 file header*/
	if (fread(&gm1->header.unknown1, sizeof(gm1->header.unknown1), 1, fp) < 1 ||
	    fread(&gm1->header.unknown2, sizeof(gm1->header.unknown2), 1, fp) < 1 ||
	    fread(&gm1->header.unknown3, sizeof(gm1->header.unknown3), 1, fp) < 1 ||

	    fread(&gm1->header.image_count, sizeof(gm1->header.image_count), 1,
	          fp) < 1 ||

	    fread(&gm1->header.unknown4, sizeof(gm1->header.unknown4), 1, fp) < 1 ||

	    fread(&gm1->header.data_type, sizeof(gm1->header.data_type), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown5, sizeof(gm1->header.unknown5), 1, fp) < 1 ||

	    fread(&gm1->header.unknown6, sizeof(gm1->header.unknown6), 1, fp) < 1 ||

	    fread(&gm1->header.size_type, sizeof(gm1->header.size_type), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown7, sizeof(gm1->header.unknown7), 1, fp) < 1 ||

	    fread(&gm1->header.unknown8, sizeof(gm1->header.unknown8), 1, fp) < 1 ||

	    fread(&gm1->header.unknown9, sizeof(gm1->header.unknown9), 1, fp) < 1 ||

	    fread(&gm1->header.unknown10, sizeof(gm1->header.unknown10), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown11, sizeof(gm1->header.unknown11), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown12, sizeof(gm1->header.unknown12), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown13, sizeof(gm1->header.unknown13), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown14, sizeof(gm1->header.unknown14), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown15, sizeof(gm1->header.unknown15), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown16, sizeof(gm1->header.unknown16), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown17, sizeof(gm1->header.unknown17), 1, fp) <
	        1 ||

	    fread(&gm1->header.data_size, sizeof(gm1->header.data_size), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown18, sizeof(gm1->header.unknown18), 1, fp) <
	        1) {
		gm1Delete(gm1);
		fclose(fp);
		return NULL;
	}

	/* read palette*/
	gm1->palette =
	    malloc(sizeof(*gm1->palette) * GM1_PALETTE_SIZE * GM1_PALETTE_COUNT);
	if (gm1->palette == NULL) {
		gm1Delete(gm1);
		fclose(fp);
		return NULL;
	}
	fread(gm1->palette, sizeof(*gm1->palette),
	      GM1_PALETTE_SIZE * GM1_PALETTE_COUNT, fp);

	gm1->image_offset_list =
	    malloc(sizeof(*(gm1->image_offset_list)) * gm1->header.image_count);
	if (gm1->image_offset_list == NULL) {
		gm1Delete(gm1);
		fclose(fp);
		return NULL;
	}

	for (unsigned int i = 0; i < gm1->header.image_count; i++) {
		fread(&gm1->image_offset_list[i], sizeof(*(gm1->image_offset_list)), 1,
		      fp);
	}

	gm1->image_size_list =
	    malloc(sizeof(*(gm1->image_size_list)) * gm1->header.image_count);
	if (gm1->image_size_list == NULL) {
		gm1Delete(gm1);
		fclose(fp);
		return NULL;
	}
	for (unsigned int i = 0; i < gm1->header.image_count; i++) {
		fread(&gm1->image_size_list[i], sizeof(*(gm1->image_size_list)), 1, fp);
	}

	gm1->image_headers =
	    malloc(sizeof(*(gm1->image_headers)) * gm1->header.image_count);
	if (gm1->image_headers == NULL) {
		gm1Delete(gm1);
		return NULL;
	}

	for (unsigned int i = 0; i < gm1->header.image_count; i++) {
		if (fread(&gm1->image_headers[i].image_width,
		          sizeof(gm1->image_headers[i].image_width), 1, fp) < 1 ||
		    fread(&gm1->image_headers[i].image_height,
		          sizeof(gm1->image_headers[i].image_height), 1, fp) < 1 ||
		    fread(&gm1->image_headers[i].position_x,
		          sizeof(gm1->image_headers[i].position_x), 1, fp) < 1 ||
		    fread(&gm1->image_headers[i].position_y,
		          sizeof(gm1->image_headers[i].position_y), 1, fp) < 1 ||
		    fread(&gm1->image_headers[i].part,
		          sizeof(gm1->image_headers[i].part), 1, fp) < 1 ||
		    fread(&gm1->image_headers[i].parts,
		          sizeof(gm1->image_headers[i].parts), 1, fp) < 1 ||
		    fread(&gm1->image_headers[i].tile_position_y,
		          sizeof(gm1->image_headers[i].tile_position_y), 1, fp) < 1 ||
		    fread(&gm1->image_headers[i].tile_placement_alignment,
		          sizeof(gm1->image_headers[i].tile_placement_alignment), 1,
		          fp) < 1 ||
		    fread(&gm1->image_headers[i].horizontal_offset,
		          sizeof(gm1->image_headers[i].horizontal_offset), 1, fp) < 1 ||
		    fread(&gm1->image_headers[i].drawing_box_width,
		          sizeof(gm1->image_headers[i].drawing_box_width), 1, fp) < 1 ||
		    fread(&gm1->image_headers[i].performance_id,
		          sizeof(gm1->image_headers[i].performance_id), 1, fp) < 1) {
			gm1Delete(gm1);
			fclose(fp);
			return NULL;
		}
	}

	int file_position = ftell(fp);
	gm1->image_data = (uint8_t *)malloc(file_size - file_position);
	if (gm1->image_data == NULL) {
		gm1Delete(gm1);
		fclose(fp);
		return NULL;
	}

	if (file_size - file_position < gm1->header.data_size ||
	    fread(gm1->image_data, sizeof(*gm1->image_data),
	          file_size - file_position, fp) < file_size - file_position) {
		gm1Delete(gm1);
		fclose(fp);
		return NULL;
	}

	return gm1;
}

void gm1Delete(struct Gm1 *gm1)
{
	if (gm1 != NULL) {
		free(gm1->palette);
		free(gm1->image_offset_list);
		free(gm1->image_size_list);
		free(gm1->image_headers);
		free(gm1->image_data);
	}
	free(gm1);
}

static int decodeTile(struct Image *image, uint8_t *data, int y_pos)
{
	int i = 0;
	int j = 2;
	int x = 0;
	int y = y_pos;
	uint16_t color = 0x0;

	while (y < y_pos + GM1_TILE_HEIGHT / 2) {
		x = GM1_TILE_WIDTH / 2 - j / 2;
		while (x < GM1_TILE_WIDTH / 2 + j / 2) {
			color = data[i];
			i++;
			color |= (data[i] << 8);
			i++;
			image->pixel[y * image->width + x].a = 0xFF;
			image->pixel[y * image->width + x].r = COLOR_CONVERT_RED(color);
			image->pixel[y * image->width + x].g = COLOR_CONVERT_GREEN(color);
			image->pixel[y * image->width + x].b = COLOR_CONVERT_BLUE(color);
			x++;
		}
		j += 4;
		y++;
	}
	j = 30;
	x = 0;
	while (y < y_pos + GM1_TILE_HEIGHT) {
		x = GM1_TILE_WIDTH / 2 - j / 2;
		while (x < GM1_TILE_WIDTH / 2 + j / 2) {
			color = data[i];
			i++;
			color |= (data[i] << 8);
			i++;
			image->pixel[y * image->width + x].a = 0xFF;
			image->pixel[y * image->width + x].r = COLOR_CONVERT_RED(color);
			image->pixel[y * image->width + x].g = COLOR_CONVERT_GREEN(color);
			image->pixel[y * image->width + x].b = COLOR_CONVERT_BLUE(color);
			x++;
		}
		j -= 4;
		y++;
	}
	return 0;
}

static int decodeTgxAndTile(struct Image *image, struct Gm1ImageHeader *header,
                            uint8_t *data, int size)
{
	if (size < 512) {
		return -1;
	}
	image->width = header->image_width;
	image->height = header->image_height;
	image->pixel = malloc(sizeof(*image->pixel) * image->width * image->height);

	if (image->pixel == NULL) {
		return -1;
	}
	imageClear(image, 0x00);
	if (size > 512) {
		if (tgxDecode(image->pixel, image->width, image->height,
		              header->horizontal_offset, data + 512, size - 512, NULL,
		              0) == -1) {
			return -1;
		}
	}

	return decodeTile(image, data, header->tile_position_y);
}

static int decodeBitmap(struct Image *image, int width, int height,
                        uint8_t *data, int size)
{
	int i = 0;
	int j = 0;
	uint16_t color = 0;
	image->width = width;
	image->height = height;
	image->pixel = malloc(sizeof(*image->pixel) * width * height);

	if (image->pixel == NULL) {
		return -1;
	}

	while (i < size) {
		color = data[i];
		i++;
		color |= (data[i] << 8);
		i++;
		image->pixel[j].a = 0xFF;
		image->pixel[j].r = COLOR_CONVERT_RED(color);
		image->pixel[j].g = COLOR_CONVERT_GREEN(color);
		image->pixel[j].b = COLOR_CONVERT_BLUE(color);
		j++;
	}
	return 0;
}

struct ImageList *gm1CreateImageList(struct Gm1 *gm1)
{
	struct ImageList *image_list = NULL;

	if (gm1 == NULL) {
		return 0;
	}

	image_list = malloc(sizeof(*image_list));

	if (image_list == NULL) {
		return NULL;
	}
	image_list->image_count = gm1->header.image_count;
	image_list->images =
	    malloc(sizeof(*(image_list->images)) * gm1->header.image_count);
	if (image_list->images == NULL) {
		imageDeleteList(image_list);
		return NULL;
	}

	switch (gm1->header.data_type) {
		case GM1_DATA_TGX:
		case GM1_DATA_TGX_FONT:
		case GM1_DATA_TGX_CONST_SIZE:
			for (int i = 0; i < image_list->image_count; i++) {
				if (tgxCreateImage(&(image_list->images[i]),
				                   gm1->image_headers[i].image_width,
				                   gm1->image_headers[i].image_height,
				                   gm1->image_data + gm1->image_offset_list[i],
				                   gm1->image_size_list[i], NULL, 0) == -1) {
					return NULL;
				}
			}
			break;
		case GM1_DATA_ANIMATION:
			for (int i = 0; i < image_list->image_count; i++) {
				if (tgxCreateImage(&(image_list->images[i]),
				                   gm1->image_headers[i].image_width,
				                   gm1->image_headers[i].image_height,
				                   gm1->image_data + gm1->image_offset_list[i],
				                   gm1->image_size_list[i],
				                   gm1->palette + 1 * GM1_PALETTE_SIZE,
				                   1) == -1) {
					return NULL;
				}
			}
			break;
		case GM1_DATA_TGX_AND_TILE:
			for (int i = 0; i < image_list->image_count; i++) {
				if (decodeTgxAndTile(
				        &(image_list->images[i]), &gm1->image_headers[i],
				        gm1->image_data + gm1->image_offset_list[i],
				        gm1->image_size_list[i]) == -1) {
					return NULL;
				}
			}
			break;
		case GM1_DATA_BITMAP:
		case GM1_DATA_BITMAP_OTHER:
			for (int i = 0; i < image_list->image_count; i++) {
				if (decodeBitmap(&(image_list->images[i]),
				                 gm1->image_headers[i].image_width,
				                 gm1->image_headers[i].image_height,
				                 gm1->image_data + gm1->image_offset_list[i],
				                 gm1->image_size_list[i]) == -1) {
					return NULL;
				}
			}
			break;
		default:
			return NULL;
			break;
	}
	return image_list;
}
