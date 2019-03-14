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

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "gm1.h"
#include "image.h"
#include "tgx.h"

#define PIXEL_BUFFER_SIZE 100 * 1024 * 1024

struct Options {
	unsigned int convert_tgx;
	unsigned int save_header;
	unsigned int palette;
	unsigned int assemble;
	unsigned int pack;
	unsigned int sort;
};

static void printHelp(FILE *fp)
{
	fprintf(fp,
	        "Usage: sh2ck [options] input_file output_dir name\n\n"
	        "Convert strongholds gm1 and tgx files to png and json,\n"
	        "as needed by castlekeep\n"
	        "options:\n"
	        "\t-h, --help\t\tThis help\n"
	        "\t-t, --tgx\t\tRead a tgx file\n"
	        "\t--header\t\tSave gm1 file header\n"
	        "\t-a --assemble\t\tAssemble tile objects\n"
	        "\t-P --pack\t\tPack images\n"
	        "\t-s --sort\t\tSort images by height\n");
}

static int saveImages(struct ImageList *image_list, const char *output_dir)
{
	char string_buffer[256];
	for (int i = 0; i < image_list->image_count; i++) {
		snprintf(string_buffer, 256, "%s/%d.png", output_dir, i);
		if (imageSave(&image_list->images[i], string_buffer) == -1) {
			fprintf(stderr, "Error on saving images\n");
			return 1;
		}
	}
	memset(string_buffer, 0, 256);
	snprintf(string_buffer, 256, "%s/data.data", output_dir);
	return imageWriteData(image_list, string_buffer);
}
static int saveAtlas(struct Image *atlas, struct ImageList *image_list,
                     const char *output_dir, const char *name)
{
	char string_buffer[256];
	snprintf(string_buffer, 256, "%s/%s.png", output_dir, name);
	if (imageSave(atlas, string_buffer) == -1) {
		fprintf(stderr, "Error on saving images\n");
		return 1;
	}
	memset(string_buffer, 0, 256);
	snprintf(string_buffer, 256, "%s/%s.data", output_dir, name);
	return imageWriteData(image_list, string_buffer);
}

static int saveHeader(struct Gm1 *gm1, const char *output_dir)
{
	char string_buffer[256];
	snprintf(string_buffer, 256, "%s/gm1_header.json", output_dir);
	return gm1SaveHeader(gm1, string_buffer);
}

static int savePalette(struct Gm1 *gm1, const char *output_dir)
{
	char string_buffer[256];
	struct Image img;

	snprintf(string_buffer, 256, "%s/palette.png", output_dir);
	gm1CreatePaletteImage(&img, gm1->palette, 16);
	return imageSave(&img, string_buffer);
}

static int convertTgx(const char *input_file, const char *output_dir)
{
	char string_buffer[256];
	struct Image image;
	struct Tgx tgx;

	if (tgxCreateFromFile(&tgx, input_file) == -1) {
		fprintf(stderr, "Error on loading file\n");
		return 1;
	}

	if (tgxCreateImage(&image, tgx.width, tgx.height, tgx.data, tgx.size,
	                   NULL) == -1) {
		fprintf(stderr, "Error on decoding image\n");
		tgxDelete(&tgx);
		return 1;
	}

	snprintf(string_buffer, 256, "%s/0.png", output_dir);
	if (imageSave(&image, string_buffer) == -1) {
		fprintf(stderr, "Error on saving images\n");
		tgxDelete(&tgx);
		imageDelete(&image, NULL);
		return 1;
	}

	imageDelete(&image, NULL);
	tgxDelete(&tgx);

	return 0;
}

static int convertGm1(const char *input_file, const char *output_dir,
                      const char *name, struct Options *options)
{
	struct ImageList image_list;
	struct Gm1 *gm1 = malloc(sizeof(*gm1));

	if (gm1CreateFromFile(gm1, input_file) == -1) {
		fprintf(stderr, "Error on loading file\n");
		return 1;
	}

	if (gm1CreateImageList(&image_list, PIXEL_BUFFER_SIZE, gm1,
	                       options->palette, options->assemble) == -1) {
		fprintf(stderr, "Error on decoding image\n");
		gm1Delete(gm1);
		return 1;
	}
	if (options->pack) {
		struct Image atlas;
		if (imagecreateAtlas(&atlas, &image_list, 1024, options->sort,
		                     options->assemble) == -1) {
			imageDeleteList(&image_list);
			gm1Delete(gm1);
			return -1;
		}
		if (saveAtlas(&atlas, &image_list, output_dir, name) == -1) {
			fprintf(stderr, "Error on saving images\n");
			imageDeleteList(&image_list);
			imageDelete(&atlas, NULL);
			gm1Delete(gm1);
			return 1;
		}
		imageDelete(&atlas, NULL);
	} else {
		if (saveImages(&image_list, output_dir) == -1) {
			fprintf(stderr, "Error on saving images\n");
			imageDeleteList(&image_list);
			gm1Delete(gm1);
			return 1;
		}
	}

	imageDeleteList(&image_list);

	if (options->save_header == 1) {
		if (saveHeader(gm1, output_dir) == -1 ||
		    savePalette(gm1, output_dir) == -1) {
			fprintf(stderr, "Error on saving header\n");
			gm1Delete(gm1);
			free(gm1);
			return 1;
		}
	}
	gm1Delete(gm1);
	free(gm1);

	return 0;
}

int main(int argc, char *argv[])
{
	const char *input_file = NULL;
	const char *output_dir = NULL;
	const char *name = NULL;
	struct Options options;
	memset(&options, 0x0, sizeof(struct Options));

	for (int i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
			printHelp(stdout);
			return 0;
		}

		if ((strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "--tgx") == 0)) {
			options.convert_tgx = 1;
		}

		if (strcmp(argv[i], "--header") == 0) {
			options.save_header = 1;
		}

		if ((strcmp(argv[i], "-p") == 0) ||
		    (strcmp(argv[i], "--palette") == 0)) {
			char *tmp = NULL;
			unsigned long val = strtoul(argv[++i], &tmp, 10);
			if (*tmp != '\0') {
				printHelp(stderr);
				return 1;
			}

			if (val >= 10) {
				fprintf(stderr, "Error: Palette has to be between 0 and 9\n");
				return 1;
			}
			printf("%d\n", (int)val);
			options.palette = val;
		}
		if ((strcmp(argv[i], "-a") == 0) ||
		    (strcmp(argv[i], "--assemble") == 0)) {
			options.assemble = 1;
		}
		if ((strcmp(argv[i], "-P")) == 0 || (strcmp(argv[i], "--pack") == 0)) {
			options.pack = 1;
		}
		if ((strcmp(argv[i], "-s")) == 0 || (strcmp(argv[i], "--sort") == 0)) {
			options.sort = 1;
		}
	}

	if (argc < 4) {
		printHelp(stderr);
		return 1;
	}

	input_file = argv[argc - 3];
	output_dir = argv[argc - 2];
	name = argv[argc - 1];

	if (mkdir(output_dir, 0775) == -1 && errno != EEXIST) {
		fprintf(stderr, "Error on creating directory\n");
		return 1;
	}

	if (options.convert_tgx == 1) {
		return convertTgx(input_file, output_dir);
	} else {
		return convertGm1(input_file, output_dir, name, &options);
	}
}
