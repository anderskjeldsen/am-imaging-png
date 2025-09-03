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

    aobject *file_name_string = file->object_properties.class_object_properties.properties[Am_IO_File_P_filename].nullable_value.value.object_value;

    string_holder *file_name_holder = file_name_string->object_properties.class_object_properties.object_data.value.custom_value;

    char * const file_name = file_name_holder->string_value;

    char header[8]; // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        __throw_simple_exception("File could not be opened for reading", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp((png_const_bytep) header, 0, 8)) {
        __throw_simple_exception("File is not recognized as a PNG file", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);        
        goto __exit2;
    }
    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
        __throw_simple_exception("png_create_read_struct failed", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit2;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        __throw_simple_exception("png_create_info_struct failed", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit3;
    }
    
    if (setjmp(png_jmpbuf(png_ptr))) {        
        __throw_simple_exception("Error during init_io", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit3;
    }
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    number_of_passes = png_set_interlace_handling(png_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr))) {
        __throw_simple_exception("Error during read_image", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit3;
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_colorp palette;
        int num_palette;

        if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette) == PNG_INFO_PLTE) {

            if (bit_depth < 8) {
                png_set_packing(png_ptr);
            }
            png_read_update_info(png_ptr, info_ptr);

            function_result fr = Am_Imaging_Image_f_createIndexed_0(width, height, num_palette);
            if (fr.exception) {
                __result.exception = fr.exception;
                goto __exit3;
            }

            aobject *image = fr.return_value.value.object_value;
            __result.return_value.value.object_value = image;


            aobject * palette_array = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_palette].nullable_value.value.object_value;
            array_holder *palette_array_holder = get_array_holder(palette_array);
            unsigned int * palette_colors = (unsigned int *) get_array_data(palette_array_holder);

            for (int i = 0; i < num_palette; i++) {
                palette_colors[i] = palette[i].red << 16 | palette[i].green << 8 | palette[i].blue;
            }

            aobject * pixel_indices_array = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelIndices].nullable_value.value.object_value;

            array_holder *pixel_indices_array_holder = get_array_holder(pixel_indices_array);
            unsigned char * pixel_indices = (unsigned char *) get_array_data(pixel_indices_array_holder);

            row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
            if (row_pointers == NULL) {
                __throw_simple_exception("Out of memory", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
                goto __exit3;
            }

            for (y = 0; y < height; y++) {
                row_pointers[y] = (png_byte *) (pixel_indices + y * width);
            }

            png_read_image(png_ptr, row_pointers);
            free(row_pointers);

        } else {
            __throw_simple_exception("Error retrieving the palette.", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        }


    }
    else if (color_type == PNG_COLOR_TYPE_RGB) {
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_BEFORE);
        png_read_update_info(png_ptr, info_ptr);
        function_result fr = Am_Imaging_Image_f_createARGB_0(width, height);
        if (fr.exception) {
            __result.exception = fr.exception;
            goto __exit3;
        }
        aobject *image = fr.return_value.value.object_value;
        __result.return_value.value.object_value = image;
        aobject * pixel_data_array = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;

        array_holder *pixel_data_array_holder = get_array_holder(pixel_data_array);
        unsigned char * pixel_data = (unsigned char *) get_array_data(pixel_data_array_holder);

        row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
        if (row_pointers == NULL) {
            __throw_simple_exception("Out of memory", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
            goto __exit3;
        }

        png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

        for (y = 0; y < height; y++) {
            row_pointers[y] = (png_byte *) pixel_data + (y * rowbytes) ;
        }

        png_read_image(png_ptr, row_pointers);
        free(row_pointers);
    } else {
        __throw_simple_exception("Unsupported color type", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit3;
    }
__exit3:
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
__exit2:
    fclose(fp);
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


