#include <libc/core.h>
#include <Am/Imaging/Jpg/JpgLoader.h>
#include <Am/Imaging/Image.h>
#include <libc/Am/Imaging/Jpg/JpgLoader.h>
#include <Am/Lang/Object.h>
#include <Am/IO/File.h>
#include <libc/core_inline_functions.h>

#include <jpeglib.h>
#include <stdio.h>
#include <setjmp.h>

function_result Am_Imaging_Jpg_JpgLoader__native_init_0(aobject * const this)
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

function_result Am_Imaging_Jpg_JpgLoader__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Imaging_Jpg_JpgLoader__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Imaging_Jpg_JpgLoader_loadFromFile_0(aobject *const this, aobject *file)
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

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *fp;
    JSAMPARRAY buffer;
    int row_stride;
    int width, height;
    int y;

    aobject *file_name_string = file->object_properties.class_object_properties.properties[Am_IO_File_P_filename].nullable_value.value.object_value;

    string_holder *file_name_holder = file_name_string->object_properties.class_object_properties.object_data.value.custom_value;

    char * const file_name = file_name_holder->string_value;

    /* open file for reading */
    fp = fopen(file_name, "rb");
    if (!fp) {
        __throw_simple_exception("File could not be opened for reading", "Am_Imaging_Jpg_JpgLoader_loadFromFile_0", &__result);
        goto __exit;
    }

    /* initialize JPEG decompression object */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fp);

    /* read JPEG header */
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        __throw_simple_exception("Error reading JPEG header", "Am_Imaging_Jpg_JpgLoader_loadFromFile_0", &__result);
        goto __exit2;
    }

    /* start decompression */
    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;

    /* Only support RGB color space (3 components) */
    if (cinfo.output_components != 3) {
        __throw_simple_exception("Unsupported color format (not RGB)", "Am_Imaging_Jpg_JpgLoader_loadFromFile_0", &__result);
        goto __exit3;
    }

    /* Create ARGB image */
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

    /* Allocate scanline buffer */
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    /* Read scanlines and convert RGB to ARGB */
    y = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        
        /* Convert RGB to ARGB (or BGRA depending on endianness) */
        /* Format: A R G B (big endian) or B G R A (little endian in memory) */
        for (int x = 0; x < width; x++) {
            unsigned char r = buffer[0][x * 3 + 0];
            unsigned char g = buffer[0][x * 3 + 1];
            unsigned char b = buffer[0][x * 3 + 2];
            
            /* Store as ARGB in platform-appropriate format */
            int pixel_index = y * width + x;
            pixel_data[pixel_index * 4 + 0] = 0xFF;  /* Alpha */
            pixel_data[pixel_index * 4 + 1] = r;      /* Red */
            pixel_data[pixel_index * 4 + 2] = g;      /* Green */
            pixel_data[pixel_index * 4 + 3] = b;      /* Blue */
        }
        y++;
    }

__exit3:
    jpeg_finish_decompress(&cinfo);
__exit2:
    jpeg_destroy_decompress(&cinfo);
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


