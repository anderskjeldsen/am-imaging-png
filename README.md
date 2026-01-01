
# am-imaging-png

A PNG image loading library for AmLang that provides cross-platform PNG file support.

## Overview

This library provides PNG image loading capabilities for the AmLang programming language. It wraps the libpng library to enable loading PNG images on multiple platforms including AmigaOS and macOS.

## Features

- Load PNG images from files
- Support for indexed color (palette-based) PNG images
- Support for RGB PNG images (converted to ARGB)
- Cross-platform support (AmigaOS 68k, macOS ARM)

## Usage

```aml
import Am.Imaging.Png
import Am.IO

var file = new File("path/to/image.png")
var image = PngLoader.load(file)
```

## Project Structure

- `src/Am/Imaging/Png/PngLoader.aml` - AmLang class definition
- `src/native-c/libc/Am/Imaging/Png/PngLoader.c` - Cross-platform native C implementation
- `src/native-c/amigaos/` - AmigaOS-specific native code
- `src/native-c/macos-arm/` - macOS ARM-specific native code

## Dependencies

- [am-lang-core](https://github.com/anderskjeldsen/am-lang-core) - AmLang core library
- [am-imaging](https://github.com/anderskjeldsen/am-imaging) - AmLang imaging library
- libpng - PNG reference library (libpng-dev for Ubuntu)

## Building

This library is configured in `package.yml` and can be built for multiple platforms using the AmLang build system.
