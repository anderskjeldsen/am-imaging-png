#include <libc/core.h>
#include <Am/Imaging/Png/PngLoader.h>
#include <Am/Imaging/Image.h>
#include <libc/Am/Imaging/Png/PngLoader.h>
#include <Am/Lang/Object.h>
#include <Am/IO/File.h>
#include <Am/Lang/Exception.h>
#include <libc/core_inline_functions.h>

#include <png.h>
#include <stdio.h>

// MorphOS PPC ships libpng 1.2.59 as png.library (dispatched through
// PNGBase). Two compat wrinkles:
//
//   1. `png_const_bytep` (libpng 1.5+) doesn't exist; png_sig_cmp's
//      signature is `(png_bytep, png_size_t, png_size_t)`. AM_PNG_SIG_CAST
//      drops the const on MorphOS.
//
//   2. `png_jmpbuf(p)` on MorphOS expands to `(p)->jmpbuf59`, a 59-int
//      buffer. The cross-toolchain's setjmp() expects ~219 ints (PPC
//      saves r13-r31 + f14-f31 + AltiVec state), so calling setjmp on
//      that buffer would stack-smash. The bundled <libraries/png.h>
//      enforces this with a macro that fails to compile if you try.
//
//      Workaround: register a libpng error callback that longjmps to
//      OUR own jmp_buf (which IS sized for the host setjmp). libpng's
//      `png_error()` calls error_fn first; if it never returns, the
//      `png_default_error -> longjmp(jmpbuf59)` fallback never runs,
//      so the 59-int mismatch is irrelevant. Single global is fine
//      here — PngLoader.loadFromFile is one-shot per call on one
//      thread.
#ifdef __MORPHOS__
#include <setjmp.h>
#include <string.h>

static jmp_buf g_am_png_err_buf;
static char g_am_png_err_msg[128];

static void am_png_error_fn(png_structp png_ptr, png_const_charp msg)
{
    (void) png_ptr;
    const char *s = msg ? msg : "unknown libpng error";
    strncpy(g_am_png_err_msg, s, sizeof(g_am_png_err_msg) - 1);
    g_am_png_err_msg[sizeof(g_am_png_err_msg) - 1] = 0;
    longjmp(g_am_png_err_buf, 1);
}

static void am_png_warn_fn(png_structp png_ptr, png_const_charp msg)
{
    (void) png_ptr; (void) msg;
}

#define AM_PNG_CREATE_READ_STRUCT() \
    png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, am_png_error_fn, am_png_warn_fn)
#define AM_PNG_JMPBUF()    (g_am_png_err_buf)
#define AM_PNG_SIG_CAST(p) ((png_bytep)(p))
#else
#define AM_PNG_CREATE_READ_STRUCT() \
    png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)
#define AM_PNG_JMPBUF()    (png_jmpbuf(png_ptr))
#define AM_PNG_SIG_CAST(p) ((png_const_bytep)(p))
#endif

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

    // __unwrap every aobject data deref — `file` and `image` may be
    // cross-thread wrappers (class_ptr == NULL) whose properties[]
    // slots are aliased with an unrelated union variant, so raw derefs
    // read NULL / garbage. See feedback_amlang_native_unwrap_cross_thread.
    aobject *file_name_string_ref = __unwrap(file)->object_properties.class_object_properties.properties[Am_IO_File_P_filename].nullable_value.value.object_value;

    string_holder *file_name_holder = __unwrap(file_name_string_ref)->object_properties.class_object_properties.object_data.value.custom_value;

    char * const file_name = file_name_holder->string_value;

    char header[8]; // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        __throw_simple_exception("File could not be opened for reading", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit;
    }

    if (fread(header, 1, 8, fp) != 8 || png_sig_cmp(AM_PNG_SIG_CAST(header), 0, 8)) {
        __throw_simple_exception("File is not recognized as a PNG file", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit2;
    }
    /* initialize stuff */
    png_ptr = AM_PNG_CREATE_READ_STRUCT();

    if (!png_ptr) {
        __throw_simple_exception("png_create_read_struct failed", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit2;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        __throw_simple_exception("png_create_info_struct failed", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
        goto __exit3;
    }
    
    if (setjmp(AM_PNG_JMPBUF())) {
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
    if (setjmp(AM_PNG_JMPBUF())) {
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


            aobject * palette_array = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_palette].nullable_value.value.object_value;
            array_holder *palette_array_holder = get_array_holder(__unwrap(palette_array));
            unsigned int * palette_colors = (unsigned int *) get_array_data(palette_array_holder);

            for (int i = 0; i < num_palette; i++) {
                palette_colors[i] = palette[i].red << 16 | palette[i].green << 8 | palette[i].blue;
            }

            aobject * pixel_indices_array = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelIndices].nullable_value.value.object_value;

            array_holder *pixel_indices_array_holder = get_array_holder(__unwrap(pixel_indices_array));
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
        aobject * pixel_data_array = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;

        array_holder *pixel_data_array_holder = get_array_holder(__unwrap(pixel_data_array));
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
    }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        // RGBA 8-bit input. libpng emits R G B A bytes per pixel
        // by default; png_set_swap_alpha rotates that to A R G B,
        // which is the in-memory order Am.Imaging.Image expects
        // for its ARGB pixel words on big-endian targets (m68k).
        // On little-endian targets the same byte sequence reads
        // back as a different UInt value, but the rendering
        // pipeline that consumes these pixels works on the byte
        // layout, not the integer value — same convention the
        // RGB branch above relies on.
        //
        // If the PNG carries 16-bit channels (rare; comes from
        // some HDR exports), scale them down to 8 first so the
        // per-pixel size matches createARGB's UInt[] backing.
        if (bit_depth == 16) {
            png_set_strip_16(png_ptr);
        }
        png_set_swap_alpha(png_ptr);
        png_read_update_info(png_ptr, info_ptr);

        function_result fr = Am_Imaging_Image_f_createARGB_0(width, height);
        if (fr.exception) {
            __result.exception = fr.exception;
            goto __exit3;
        }
        aobject *image = fr.return_value.value.object_value;
        __result.return_value.value.object_value = image;
        aobject * pixel_data_array = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;

        array_holder *pixel_data_array_holder = get_array_holder(__unwrap(pixel_data_array));
        unsigned char * pixel_data = (unsigned char *) get_array_data(pixel_data_array_holder);

        row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
        if (row_pointers == NULL) {
            __throw_simple_exception("Out of memory", "Am_Imaging_Png_PngLoader_loadFromFile_0", &__result);
            goto __exit3;
        }

        png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

        for (y = 0; y < height; y++) {
            row_pointers[y] = (png_byte *) pixel_data + (y * rowbytes);
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


