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
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "gm1.h"
#include "tgx.h"
#include "image.h"

static void printHelp(FILE *fp)
{
	fprintf(fp,
	        "Usage: sh2ck [options] input_file output_dir\n\n"
	        "Convert strongholds gm1 and tgx files to png and json,\n"
	        "as needed by castlekeep\n"
	        "options:\n"
	        "\t-h, --help\t\tThis help\n"
	        "\t-t, --tgx\t\tRead a tgx file\n");
}

static int convertTgx(const char *input_file, const char *output_dir)
{
	char string_buffer[256];
	struct Image *image = malloc(sizeof(*image));

	struct Tgx *tgx = tgxCreateFromFile(input_file);
	if (tgx == NULL) {
		fprintf(stderr, "Error on loading file\n");
		return 1;
	}

	if (tgxCreateImage(image, tgx->width, tgx->height, tgx->data, tgx->size,
	                   NULL) == -1) {
		fprintf(stderr, "Error on decoding image\n");
		return 1;
	}

	snprintf(string_buffer, 256, "%s/image0.png", output_dir);
	if (imageSave(image, string_buffer) == -1) {
		fprintf(stderr, "Error on saving images\n");
		return 1;
	}

	imageDelete(image);
	tgxDelete(tgx);

	return 0;
}

static int convertGm1(const char *input_file, const char *output_dir)
{
	char string_buffer[256];
	struct ImageList *image_list = NULL;
	struct TileObjectList *object_list = NULL;
	struct Gm1 *gm1 = gm1CreateFromFile(input_file);
	if (gm1 == NULL) {
		fprintf(stderr, "Error on loading file\n");
		return 1;
	}

	if (gm1->header.data_type == GM1_DATA_TGX_AND_TILE) {
		object_list = gm1CreateTileObjectList(gm1);
		if (object_list != NULL) {
			image_list = &object_list->image_list;
		}
	} else {
		image_list = gm1CreateImageList(gm1);
	}
	if (image_list == NULL) {
		fprintf(stderr, "Error on decoding image\n");
		return 1;
	}

	for (int i = 0; i < image_list->image_count; i++) {
		snprintf(string_buffer, 256, "%s/image%d.png", output_dir, i);
		if (imageSave(&image_list->images[i], string_buffer) == -1) {
			fprintf(stderr, "Error on saving images\n");
			return 1;
		}
	}
	if(gm1->header.data_type != GM1_DATA_TGX_AND_TILE) {
		imageDeleteList(image_list);
		free(image_list);
	}else {
		tileObjectDeleteList(object_list);
		free(object_list);
	}
	gm1Delete(gm1);

	return 0;
}

int main(int argc, char *argv[])
{
	const char *input_file;
	const char *output_dir;
	int convert_tgx = 0;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printHelp(stdout);
			return 0;
		}

		if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tgx") == 0) {
			convert_tgx = 1;
		}
	}

	if (argc < 3) {
		printHelp(stderr);
		return 1;
	}
	input_file = argv[argc - 2];
	output_dir = argv[argc - 1];

	if (mkdir(output_dir, 0775) == -1 && errno != EEXIST) {
		fprintf(stderr, "Error on creating directory\n");
		return 1;
	}

	if (convert_tgx == 1) {
		return convertTgx(input_file, output_dir);
	} else {
		return convertGm1(input_file, output_dir);
	}
}
