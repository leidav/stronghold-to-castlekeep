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
#include <math.h>

#include "gm1.h"
#include "tgx.h"
#include "image.h"
int gm1CreateFromFile(struct Gm1 *gm1, const char *file)
{
	FILE *fp = fopen(file, "rb");
	if (fp == NULL) {
		return -1;
	}
	if (fseek(fp, 0, SEEK_END)) {
		fclose(fp);
		return -1;
	}
	int file_size = ftell(fp);
	if (file_size == 0) {
		fclose(fp);
		return -1;
	}
	fseek(fp, 0, SEEK_SET);

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

	    fread(&gm1->header.width, sizeof(gm1->header.width), 1, fp) < 1 ||

	    fread(&gm1->header.height, sizeof(gm1->header.height), 1, fp) < 1 ||

	    fread(&gm1->header.unknown12, sizeof(gm1->header.unknown12), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown13, sizeof(gm1->header.unknown13), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown14, sizeof(gm1->header.unknown14), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown15, sizeof(gm1->header.unknown15), 1, fp) <
	        1 ||

	    fread(&gm1->header.center_x, sizeof(gm1->header.center_x), 1, fp) < 1 ||

	    fread(&gm1->header.center_y, sizeof(gm1->header.center_y), 1, fp) < 1 ||

	    fread(&gm1->header.data_size, sizeof(gm1->header.data_size), 1, fp) <
	        1 ||

	    fread(&gm1->header.unknown18, sizeof(gm1->header.unknown18), 1, fp) <
	        1) {
		gm1Delete(gm1);
		fclose(fp);
		return -1;
	}

	/* read palette*/
	gm1->palette =
	    malloc(sizeof(*gm1->palette) * GM1_PALETTE_SIZE * GM1_PALETTE_COUNT);
	if (gm1->palette == NULL) {
		gm1Delete(gm1);
		fclose(fp);
		return -1;
	}
	fread(gm1->palette, sizeof(*gm1->palette),
	      GM1_PALETTE_SIZE * GM1_PALETTE_COUNT, fp);

	gm1->image_offset_list =
	    malloc(sizeof(*(gm1->image_offset_list)) * gm1->header.image_count);
	if (gm1->image_offset_list == NULL) {
		gm1Delete(gm1);
		fclose(fp);
		return -1;
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
		return -1;
	}
	for (unsigned int i = 0; i < gm1->header.image_count; i++) {
		fread(&gm1->image_size_list[i], sizeof(*(gm1->image_size_list)), 1, fp);
	}

	gm1->image_headers =
	    malloc(sizeof(*(gm1->image_headers)) * gm1->header.image_count);
	if (gm1->image_headers == NULL) {
		gm1Delete(gm1);
		return -1;
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
			return -1;
		}
	}

	int file_position = ftell(fp);
	gm1->image_data = (uint8_t *)malloc(file_size - file_position);
	if (gm1->image_data == NULL) {
		gm1Delete(gm1);
		fclose(fp);
		return -1;
	}

	if (file_size - file_position < gm1->header.data_size ||
	    fread(gm1->image_data, sizeof(*gm1->image_data),
	          file_size - file_position, fp) < file_size - file_position) {
		gm1Delete(gm1);
		fclose(fp);
		return -1;
	}

	return 0;
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
}

static int decodeTile(struct Image *image, uint8_t *data, struct Pos *offset)
{
	int i = 0;
	int j = 2;
	int x = 0;
	int y = offset->y;
	uint16_t color = 0x0;

	while (y < offset->y + GM1_TILE_HEIGHT / 2) {
		x = offset->x + GM1_TILE_WIDTH / 2 - j / 2;
		while (x < offset->x + GM1_TILE_WIDTH / 2 + j / 2) {
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
	while (y < offset->y + GM1_TILE_HEIGHT) {
		x = offset->x + GM1_TILE_WIDTH / 2 - j / 2;
		while (x < offset->x + GM1_TILE_WIDTH / 2 + j / 2) {
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

static int decodeTgxAndTile(struct Image *image, struct Rect *rect,
                            struct Gm1ImageHeader *header, uint8_t *data,
                            int size)
{
	if (size < 512) {
		return -1;
	}
	if (size > 512) {
		struct Rect rect2 = {rect->x + header->horizontal_offset, rect->y,
		                     rect->width - header->horizontal_offset,
		                     rect->height};
		if (tgxDecode(image, &rect2, data + 512, size - 512, NULL) == -1) {
			return -1;
		}
	}

	struct Pos offset = {rect->x, rect->y + header->tile_position_y};
	return decodeTile(image, data, &offset);
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

int gm1CreateTileObjectList(struct ImageList *image_list, struct Gm1 *gm1)
{
	int object_count = 0;
	struct TileObjectList *object_list = NULL;
	for (int i = 0; i < gm1->header.image_count; i++) {
		if (gm1->image_headers[i].part == gm1->image_headers[i].parts - 1) {
			object_count++;
		}
	}

	if (imageCreateList(image_list, object_count, IMAGE_TYPE_TILE) == -1) {
		return -1;
	}

	object_list = (struct TileObjectList *)image_list->data;

	int i = 0;
	int j = 0;
	while (i < gm1->header.image_count) {
		int part_count = gm1->image_headers[i].parts;
		int part = 0;
		int max_length = sqrt(part_count);
		int current_length = 1;
		int xtile = 0;
		int ytile = 0;

		int height = 0;
		int image_width = max_length * (GM1_TILE_WIDTH + 2) - 2;
		int image_height = 0;
		int x = 0;
		int y = 0;

		if (tileObjectCreate(&object_list->objects[j], part_count) == -1) {
			tileObjectDeleteList(object_list);
			return -1;
		}

		while (part < part_count) {
			if (xtile < current_length) {
				y = ytile * GM1_TILE_HEIGHT / 2;
				x = (image_width / 2) -
				    current_length * (GM1_TILE_WIDTH + 2) / 2 +
				    xtile * (GM1_TILE_WIDTH + 2) + 1;
				object_list->objects[j].parts[part].id = part;
				object_list->objects[j].parts[part].x = xtile;
				object_list->objects[j].parts[part].y = ytile;
				object_list->objects[j].parts[part].rect.width = GM1_TILE_WIDTH;
				object_list->objects[j].parts[part].rect.height =
				    gm1->image_headers[i].image_height;
				object_list->objects[j].parts[part].rect.x = x;
				object_list->objects[j].parts[part].rect.y = y;
				height = y + gm1->image_headers[i].image_height;
				if (height > image_height) {
					image_height = height;
				}
				xtile++;
				i++;
				part++;
			} else {
				xtile = 0;
				ytile++;
				if (ytile < max_length) {
					current_length++;
				} else {
					current_length--;
				}
			}
		}

		if (imageCreate(&image_list->images[j], image_width, image_height) ==
		    -1) {
			tileObjectDeleteList(object_list);
			return -1;
		}
		j++;
	}

	int k = 0;
	for (j = 0; j < object_list->object_count; j++) {
		imageClear(&image_list->images[j], 0x00);
		for (i = 0; i < object_list->objects[j].part_count; i++) {
			object_list->objects[j].parts[i].rect.y =
			    (image_list->images[j].height -
			     (object_list->objects[j].parts[i].rect.y +
			      object_list->objects[j].parts[i].rect.height));

			if (decodeTgxAndTile(&image_list->images[j],
			                     &object_list->objects[j].parts[i].rect,
			                     &gm1->image_headers[k],
			                     gm1->image_data + gm1->image_offset_list[k],
			                     gm1->image_size_list[k]) == -1) {
				tileObjectDeleteList(object_list);
				return -1;
			}
			k++;
		}
	}

	return 0;
}

int gm1CreateImageList(struct ImageList *image_list, struct Gm1 *gm1,
                       int palette)
{
	switch (gm1->header.data_type) {
		case GM1_DATA_TGX_AND_TILE:
			if (gm1CreateTileObjectList(image_list, gm1) == -1) {
				return -1;
			}
			break;
		case GM1_DATA_TGX:
		case GM1_DATA_TGX_FONT:
		case GM1_DATA_TGX_CONST_SIZE:
			if (imageCreateList(image_list, gm1->header.image_count,
			                    IMAGE_TYPE_OTHER)) {
				return -1;
			}
			for (int i = 0; i < image_list->image_count; i++) {
				if (tgxCreateImage(&(image_list->images[i]),
				                   gm1->image_headers[i].image_width,
				                   gm1->image_headers[i].image_height,
				                   gm1->image_data + gm1->image_offset_list[i],
				                   gm1->image_size_list[i], NULL) == -1) {
					imageDeleteList(image_list);
					return -1;
				}
			}
			break;
		case GM1_DATA_ANIMATION:
			if (imageCreateList(image_list, gm1->header.image_count,
			                    IMAGE_TYPE_ANIMATION)) {
				return -1;
			}
			for (int i = 0; i < image_list->image_count; i++) {
				if (tgxCreateImage(&(image_list->images[i]),
				                   gm1->image_headers[i].image_width,
				                   gm1->image_headers[i].image_height,
				                   gm1->image_data + gm1->image_offset_list[i],
				                   gm1->image_size_list[i],
				                   gm1->palette + palette * GM1_PALETTE_SIZE) ==
				    -1) {
					imageDeleteList(image_list);
					return -1;
				}
			}
			break;
		case GM1_DATA_BITMAP:
		case GM1_DATA_BITMAP_OTHER:
			if (imageCreateList(image_list, gm1->header.image_count,
			                    IMAGE_TYPE_OTHER)) {
				return -1;
			}
			for (int i = 0; i < image_list->image_count; i++) {
				if (decodeBitmap(&(image_list->images[i]),
				                 gm1->image_headers[i].image_width,
				                 gm1->image_headers[i].image_height,
				                 gm1->image_data + gm1->image_offset_list[i],
				                 gm1->image_size_list[i]) == -1) {
					imageDeleteList(image_list);
					return -1;
				}
			}
			break;
		default:
			return -1;
			break;
	}
	return 0;
}

int gm1SaveHeader(struct Gm1 *gm1, const char *file)
{
	FILE *fp = fopen(file, "w");
	if (fp == NULL) {
		return -1;
	}
	const static char *data_type_lookup[] = {"TGX", "ANIMATION", "TGX_AND_TILE",
	                                         "TGX_FONT", "BITMAP",
	                                         "TGX_CONST_SIZE", "BITMAP_OTHER"};
	const static char *size_type_lookup[] = {
	    "UNDEFINDED", "30x30",   "55x55",   "75x75",
	    "UNKNOWN_4",  "100x100", "110x110", "130x130",
	    "UNKNOWN_8",  "185x185", "250x250", "180x180"};
	fprintf(fp,
	        "{\n\"unknown1\": %d,\n"
	        "  \"uint32_unknown2\": %d,\n"
	        "  \"uint32_unknown3\": %d,\n"
	        "  \"image_count\": %d,\n"
	        "  \"uint32_unknown4\": %d,\n"
	        "  \"data_type\": \"%s\",\n"
	        "  \"uint32_unknown5\": %d,\n"
	        "  \"uint32_unknown6\": %d,\n"
	        "  \"size_type\": \"%s\",\n"
	        "  \"uint32_unknown7\": %d,\n"
	        "  \"uint32_unknown8\": %d,\n"
	        "  \"uint32_unknown9\": %d,\n"
	        "  \"width\": %d,\n"
	        "  \"height\": %d,\n"
	        "  \"uint32_unknown12\": %d,\n"
	        "  \"uint32_unknown13\": %d,\n"
	        "  \"uint32_unknown14\": %d,\n"
	        "  \"uint32_unknown15\": %d,\n"
	        "  \"center_x\": %d,\n"
	        "  \"center_y\": %d,\n"
	        "  \"data_size\": %d,\n"
	        "  \"uint32_unknown18\": %d\n}\n",
	        gm1->header.unknown1, gm1->header.unknown2, gm1->header.unknown3,
	        gm1->header.image_count, gm1->header.unknown4,
	        data_type_lookup[gm1->header.data_type - 1], gm1->header.unknown5,
	        gm1->header.unknown6, size_type_lookup[gm1->header.size_type],
	        gm1->header.unknown7, gm1->header.unknown8, gm1->header.unknown9,
	        gm1->header.width, gm1->header.height, gm1->header.unknown12,
	        gm1->header.unknown13, gm1->header.unknown14, gm1->header.unknown15,
	        gm1->header.center_x, gm1->header.center_y, gm1->header.data_size,
	        gm1->header.unknown18);
	fclose(fp);
	return 0;
}

int gm1IsTileObject(struct Gm1 *gm1)
{
	return gm1->header.data_type == GM1_DATA_TGX_AND_TILE;
}

int gm1IsAnimation(struct Gm1 *gm1)
{
	return gm1->header.data_type == GM1_DATA_ANIMATION ||
	       gm1->header.data_type == GM1_DATA_TGX_CONST_SIZE;
}

int gm1SavePalette(struct Gm1 *gm1, const char *file)
{
	FILE *fp = fopen(file, "w");
	if (fp == NULL) {
		return -1;
	}
	fwrite(gm1->palette, GM1_PALETTE_SIZE, GM1_PALETTE_COUNT, fp);
	fclose(fp);
	return 0;
}

int gm1CreatePaletteImage(struct Image *image, const uint16_t *palette,
                          int size)
{
	const int linewidth = 256;
	const int linecount = (GM1_PALETTE_COUNT * GM1_PALETTE_SIZE) / linewidth;

	if (imageCreate(image, size * linewidth, size * linecount) == -1) {
		return -1;
	}
	int j = 0;
	int k = 0;
	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			image->pixel[y * image->width + x].a = 0xFF;
			image->pixel[y * image->width + x].r =
			    COLOR_CONVERT_RED(palette[j * linewidth + k]);
			image->pixel[y * image->width + x].g =
			    COLOR_CONVERT_GREEN(palette[j * linewidth + k]);
			image->pixel[y * image->width + x].b =
			    COLOR_CONVERT_BLUE(palette[j * linewidth + k]);
			if (x % size == 0) {
				k++;
			}
		}
		k = 0;
		if (y % size == 0) {
			j++;
		}
	}
	return 0;
}
