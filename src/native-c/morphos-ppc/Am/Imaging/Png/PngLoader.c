#include <libc/core.h>
#include <Am/Imaging/Png/PngLoader.h>
#include <morphos-ppc/Am/Imaging/Png/PngLoader.h>
#include <Am/Lang/Object.h>
#include <Am/Imaging/Image.h>
#include <Am/IO/File.h>
#include <libc/core_inline_functions.h>

// MorphOS PPC placeholder. The actual stubs live in
// native-c/libc/Am/Imaging/Png/PngLoader.c under #ifdef __MORPHOS__
// because the build pipeline doesn't always stage this morphos-ppc/
// native dir for am-imaging-png — keeping the symbols there avoids
// link-time misses regardless.
