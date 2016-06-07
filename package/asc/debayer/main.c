/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** main.h
** Simple stupid de-bayering application that outputs pngs/
**
** Author: Dimitri Emmerich (me@dimatosaurus.com)
** -------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <libpng16/png.h>

typedef struct pixel_t {
    unsigned char r,g,b;
} pixel_t;

int main(int argc, char ** argv) {
    if (argc < 5) {
        printf("Not enough arguments \n");
        return 1;
    }

    size_t width = (size_t)atoi(argv[1]);
    size_t height = (size_t)atoi(argv[2]);

    const char * in_file = argv[3];
    const char * out_file = argv[4];

    FILE *in = fopen(in_file, "rb");
    FILE *out = fopen(out_file, "wb");

    if(in == NULL || out == NULL) {
        printf("Could not open files");
        return 1;
    }

    unsigned short *line_in = (unsigned short*)malloc(width * 4);
    pixel_t *line_out = (pixel_t*)malloc(sizeof(pixel_t) * width * 2); // two lines

    if(line_in == NULL || line_out == NULL) {
        printf("Out of memory.\n");
        return 1;
    }

    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        return 1;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        return 1;
    }

    png_init_io(png_ptr, out);
    // Write header
    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    int i=0;
    while(i < height) {
        // Read in two line
        size_t r = fread(line_in, 1, width * 4, in);
        if(r != (width * 4)) {
            printf("Failed to read line");
            return 1;
        }

        // Build output lines
        int j=0;
        while (j < width) {

            unsigned short blue      = line_in[j];
            unsigned short green_1   = line_in[j+1];
            unsigned short red       = line_in[j+1+width];
            unsigned short green_2   = line_in[j+width];

            pixel_t p;

            p.r = red / (1 << 4);
            p.g = green_1 / (1 << 4);
            p.b = blue / (1 << 4);

            line_out[j]         = p;
            line_out[j+1]       = p;
            line_out[j+1+width] = p;
            line_out[j+width]   = p;

            j += 2;
        }

        png_write_row(png_ptr, (png_const_bytep)line_out);
        png_write_row(png_ptr, (png_const_bytep)line_out);

        i += 2;
    }

    free(line_in);
    free(line_out);

    fclose(in);
    fclose(out);

    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

    return 0;
}
