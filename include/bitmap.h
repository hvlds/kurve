#ifndef BITMAP_H
#define BITMAP_H

//Includes from the standard library:
#include <stdint.h>

//Boolean stuff:
typedef uint8_t bitmap_bool_t;

#define BITMAP_BOOL_TRUE 1
#define BITMAP_BOOL_FALSE 0

//Logging:
typedef int bitmap_logging_t;

#define BITMAP_LOGGING_NONE 0
#define BITMAP_LOGGING_DEFAULT 1
#define BITMAP_LOGGING_VERBOSE 2

#define BITMAP_LOGGING BITMAP_LOGGING_DEFAULT

//The kind of DIB header that is used for the bitmap:
typedef uint16_t bitmap_dib_header_format_t;

#define BITMAP_DIB_HEADER_CORE_OS2_1X 12
#define BITMAP_DIB_HEADER_OS2_2X 64
#define BITMAP_DIB_HEADER_OS2_2X_SHORT 16
#define BITMAP_DIB_HEADER_INFO 40
#define BITMAP_DIB_HEADER_INFO_V2 52
#define BITMAP_DIB_HEADER_INFO_V3 56
#define BITMAP_DIB_HEADER_INFO_V4 108
#define BITMAP_DIB_HEADER_INFO_V5 124

//The color depth of a bitmap:
typedef uint16_t bitmap_color_depth_t;

#define BITMAP_COLOR_DEPTH_1 1
#define BITMAP_COLOR_DEPTH_4 4
#define BITMAP_COLOR_DEPTH_8 8
#define BITMAP_COLOR_DEPTH_16 16
#define BITMAP_COLOR_DEPTH_24 24
#define BITMAP_COLOR_DEPTH_32 32

//The compression of a bitmap:
typedef int bitmap_compression_t;

#define BITMAP_COMPRESSION_NONE 0
#define BITMAP_COMPRESSION_RLE 1
#define BITMAP_COMPRESSION_BITFIELD_RGB 2
#define BITMAP_COMPRESSION_BITFIELD_ARGB 6

//A single component of a pixel (R, G, B, or A).
typedef uint8_t bitmap_component_t;

//A single pixel.
typedef struct _bitmap_pixel_t_
{
	bitmap_component_t c0;
	bitmap_component_t c1;
	bitmap_component_t c2;
	bitmap_component_t c3;
} bitmap_pixel_t;

//The color space:
typedef int bitmap_color_space_t;

#define BITMAP_COLOR_SPACE_RGB 0
#define BITMAP_COLOR_SPACE_HSV 1

//Typed pixels:
typedef struct _bitmap_pixel_rgb_t_
{
	bitmap_component_t r;
	bitmap_component_t g;
	bitmap_component_t b;
	bitmap_component_t c3;
} bitmap_pixel_rgb_t;

typedef struct _bitmap_pixel_hsv_t_
{
	bitmap_component_t h;
	bitmap_component_t s;
	bitmap_component_t v;
	bitmap_component_t c3;
} bitmap_pixel_hsv_t;

//Parameters for bitmap creation.
//This is our own format! We convert it dynamically from / to the actual bitmap.
typedef struct _bitmap_parameters_t_
{
	//Is this bitmap bottom-up?
	bitmap_bool_t bottomUp;

	//Width in pixels:
	uint32_t widthPx;

	//Height in pixels:
	uint32_t heightPx;

	//Which color depth is used?
	bitmap_color_depth_t colorDepth;

	//Which compression is used?
	bitmap_compression_t compression;

	//Bit masks (R, G, B for BITMAP_COMPRESSION_BITFIELD_RGB AND A, R, G, B for BITMAP_COMPRESSION_BITFIELD_ARGB):
	uint32_t bitmasks[4];

	//Which kind of DIB header is used?
	bitmap_dib_header_format_t dibHeaderFormat;

	//The number of valid entries in the color table:
	uint32_t colorTableEntries;

	//The color space the user provides:
	bitmap_color_space_t colorSpace;

	//The color table:
	bitmap_pixel_t colorTable[256 * sizeof(bitmap_pixel_t)];
} bitmap_parameters_t;

//Bitmap errors:
typedef int bitmap_error_t;

#define BITMAP_ERROR_SUCCESS             0
#define BITMAP_ERROR_INVALID_PATH        1
#define BITMAP_ERROR_INVALID_FILE_FORMAT 2
#define BITMAP_ERROR_IO                  3
#define BITMAP_ERROR_MEMORY              4
#define BITMAP_ERROR_FILE_EXISTS         5

/**********************************************************************************************************************************************************************
	Read an existing bitmap file.
	If the functions return successfully, the allocated buffer must be released.

	Errors:
	- BITMAP_ERROR_INVALID_PATH         The given path is not valid in any way (missing file, bad permissions etc.).
	- BITMAP_ERROR_INVALID_FILE_FORMAT  The given file is no valid bitmap.
	- BITMAP_ERROR_IO                   An IO error has occurred.
	- BITMAP_ERROR_MEMORY               Insufficient memory.
**********************************************************************************************************************************************************************/

bitmap_error_t bitmapReadPixels(const char* filePath, bitmap_pixel_t** pixels, int* widthPx, int* heightPx, bitmap_color_space_t colorSpace);

/**********************************************************************************************************************************************************************
	Write a bitmap file. Use the provided bitmap parameters.

	Errors:
	- BITMAP_ERROR_INVALID_PATH         The given path is not valid in any way (missing intermediates, bad permissions etc.).
	- BITMAP_ERROR_INVALID_FILE_FORMAT  There are problems with the provided bitmap parameters.
	- BITMAP_ERROR_IO                   An IO error has occurred.
	- BITMAP_ERROR_MEMORY               Insufficient memory.
	- BITMAP_ERROR_FILE_EXISTS          The file at the given path already exists (and overwriteExisting is false).
**********************************************************************************************************************************************************************/

bitmap_error_t bitmapWritePixels(const char* filePath, bitmap_bool_t overwriteExisting, bitmap_parameters_t* parameters, const bitmap_pixel_t* pixels);

#endif
