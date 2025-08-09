#include <libc/core.h>
#include <Am/Imaging/Png/PngLoader.h>
#include <Am/Imaging/Image.h>
#include <libc/Am/Imaging/Png/PngLoader.h>
#include <Am/Lang/Object.h>
#include <Am/IO/File.h>
#include <libc/core_inline_functions.h>

#include <png.h>
#include <stdio.h>

function_result Am_Imaging_Png_PngLoader__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Imaging_Png_PngLoader__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Imaging_Png_PngLoader__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Imaging_Png_PngLoader_loadFromFile_0(aobject *const this, aobject *file)
{
    function_result __result = {.has_return_value = true};
    bool __returning = false;
    if (this != NULL)
    {
        __increase_reference_count(this);
    }
    if (file != NULL)
    {
        __increase_reference_count(file);
    }

    int x, y;

    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep * row_pointers;

    printf("load 1\n");
    aobject *file_name_string = file->object_properties.class_object_properties.properties[Am_IO_File_P_filename].nullable_value.value.object_value;

    printf("load 2 %p\n", file_name_string);

    string_holder *file_name_holder = file_name_string->object_properties.class_object_properties.object_data.value.custom_value;
    printf("load 3\n");

    char * const file_name = file_name_holder->string_value;
    printf("load %s\n", file_name);

    char header[8]; // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    printf("load 4\n");
    if (!fp) {
        __throw_simple_exception("File could not be opened for reading", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
//        printf("[read_png_file] File %s could not be opened for reading", file_name);
        goto __exit;
    }
    printf("load 5\n");

    fread(header, 1, 8, fp);
    if (png_sig_cmp((png_const_bytep) header, 0, 8)) {
        __throw_simple_exception("File is not recognized as a PNG file", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);        
//        printf("[read_png_file] File %s is not recognized as a PNG file", file_name);
        goto __exit2;
    }
    printf("load 6\n");
    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
        __throw_simple_exception("png_create_read_struct failed", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
//        printf("[read_png_file] png_create_read_struct failed");
        goto __exit2;
    }
    printf("load 7\n");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        __throw_simple_exception("png_create_info_struct failed", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
//        printf("[read_png_file] png_create_info_struct failed");
        goto __exit3;
    }

    printf("load 8\n");
    
    if (setjmp(png_jmpbuf(png_ptr))) {        
        printf("setjmp failed\n");
        __throw_simple_exception("Error during init_io", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
//        printf("[read_png_file] Error during init_io");
        goto __exit3;
    }
    printf("load 8c\n");
    png_init_io(png_ptr, fp);
    printf("load 9\n");
    png_set_sig_bytes(png_ptr, 8);
    printf("load 10\n");
    png_read_info(png_ptr, info_ptr);

    printf("load 11\n");
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    printf("load 15, w: %d, h: %d, coltype: %d, bit depth: %d \n", width, height, color_type, bit_depth);
    number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    printf("load 16\n");
    /* read file */
    if (setjmp(png_jmpbuf(png_ptr))) {
        printf("[read_png_file] Error during read_image");
        goto __exit3;
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        printf("load 17a\n");

        printf("Palette size: %d\n", 1 << bit_depth);

        function_result fr = Am_Imaging_Image_f_createIndexed_0(width, height, 1 << bit_depth);
        if (fr.exception) {
            __result.exception = fr.exception;
            goto __exit3;
        }
        printf("load 17b\n");
        aobject *image = fr.return_value.value.object_value;
        printf("load 17b2\n");
        __result.return_value.value.object_value = image;
        printf("load 17b3\n");
        aobject * pixel_indices_array = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelIndices].nullable_value.value.object_value;
        printf("load 17b4\n");

        array_holder *pixel_indices_array_holder = get_array_holder(pixel_indices_array);
        printf("Pixel indices Array size: %d\n", pixel_indices_array_holder->size);
        unsigned char * pixel_indices = (unsigned char *) get_array_data(pixel_indices_array_holder);

        printf("load 17c\n");

        row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);

        for (y = 0; y < height; y++) {
            row_pointers[y] = (png_byte *) pixel_indices + y * width;
        }
        printf("load 17d\n");

        png_read_image(png_ptr, row_pointers);
        free(row_pointers);
        printf("load 17e\n");

        // read palette

        aobject * palette_array = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_palette].nullable_value.value.object_value;
        array_holder *palette_array_holder = get_array_holder(palette_array);
        printf("Palette Array size: %d, item size: %d\n", palette_array_holder->size, palette_array_holder->item_size);
        unsigned int * palette_colors = (unsigned int *) get_array_data(palette_array_holder);

        png_colorp palette;
        int num_palette;

        if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette) == PNG_INFO_PLTE) {
            printf("Palette has %d colors.\n", num_palette);
            for (int i = 0; i < num_palette; i++) {
                palette_colors[i] = palette[i].red << 16 | palette[i].green << 8 | palette[i].blue;
//                printf("Color %d: R=%d, G=%d, B=%d\n", i, palette[i].red, palette[i].green, palette[i].blue);
            }
        } else {
            printf("Error retrieving the palette.\n");
        }
    } else {
        __throw_simple_exception("Unsupported color type", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit3;
    }
__exit3:
    printf("load 18\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    printf("load 19\n");
__exit2:
    fclose(fp);
    printf("load 20\n");
__exit:;
    if (this != NULL)
    {
        __decrease_reference_count(this);
    }
    if (file != NULL)
    {
        __decrease_reference_count(file);
    }
    return __result;
};


