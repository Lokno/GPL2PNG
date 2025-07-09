#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char r, g, b;
} Color;

Color* parse_gpl(const char *filename, int *color_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open GPL file");
        return NULL;
    }

    char line[256];
    *color_count = 0;

    while (fgets(line, sizeof(line), file)) {
        // Skip comments and headers
        if (line[0] == '#' || line[0] == '\n' || strncmp(line, "GIMP", 4) == 0 ||
            strncmp(line, "Name:", 5) == 0 || strncmp(line, "Columns:", 8) == 0)
            continue;

        int r, g, b;
        if (sscanf(line, "%d %d %d", &r, &g, &b) == 3) {
            (*color_count)++;
        }
    }

    int p = 0;
    Color* colors = (Color*)malloc(sizeof(Color)**color_count);

    fseek(file,0,SEEK_SET);

    while (fgets(line, sizeof(line), file)) {
        // Skip comments and headers
        if (line[0] == '#' || line[0] == '\n' || strncmp(line, "GIMP", 4) == 0 ||
            strncmp(line, "Name:", 5) == 0 || strncmp(line, "Columns:", 8) == 0)
            continue;

        int r, g, b;
        if (sscanf(line, "%d %d %d", &r, &g, &b) == 3) {
            colors[p].r = (unsigned char)r;
            colors[p].g = (unsigned char)g;
            colors[p].b = (unsigned char)b;
            p++;
        }
    }

    fclose(file);
    return colors;
}

int main( int argc, char** argv ) {
    Color* colors = NULL;
    int color_count = 0;

    if( argc != 2 )
    {
    	printf("   usage %s palette.gpl\n", argv[0]);
    	exit(-1);
    }

    if (!(colors = parse_gpl(argv[1], &color_count))) {
    	printf("ERROR: Parsing failed\n");
        return 1;
    }

    int width = color_count;
    int height = 1;
    unsigned char *image_data = malloc(width * height * 3);

    for (int i = 0; i < color_count; i++) {
        image_data[i * 3 + 0] = colors[i].r;
        image_data[i * 3 + 1] = colors[i].g;
        image_data[i * 3 + 2] = colors[i].b;
    }

    if (!stbi_write_png("palette.png", width, height, 3, image_data, width * 3)) {
        fprintf(stderr, "Failed to write PNG\n");
        free(image_data);
        return 1;
    }

    printf("palette.png generated with %d colors.\n", color_count);
    free(image_data);
    free(colors);
    return 0;
}
