#include "bitmap.h"

//Min / max:
#define BITMAP_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define BITMAP_MAX(a, b) (((a) > (b)) ? (a) : (b))

//Includes from the standard library:
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Includes from POSIX:
#include <unistd.h>

//Constants:
#define BITMAP_MAGIC_NUMBER 0x4D42

//The internal representation of a bitmap.
typedef struct _bitmap_t_
{
	//The embedded bitmap parameters:
	bitmap_parameters_t parameters;

	//The file pointer:
	FILE* file;

	//Where do the pixels start?
	uint32_t pixelOffset;
} bitmap_t;

//Internal logging function based on BITMAP_LOGGING.
//This always writes full lines.
void bitmapLog(bitmap_logging_t logging, const char* format, ...)
{
	//Check level:
	if (BITMAP_LOGGING < logging)
	{
		return;
	}

	//Delegate to vprintf:
	va_list args;
	va_start(args, format);

	vprintf(format, args);
	printf("\n");

	va_end(args);
}

/**********************************************************************************************************************************************************************
	Converting pixels
	Thanks to http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
	Do we need FP? Not right now ...
**********************************************************************************************************************************************************************/

bitmap_pixel_rgb_t pixelToRGB(bitmap_pixel_t pixel, bitmap_color_space_t colorSpace)
{
	bitmap_pixel_rgb_t newPixel;

	switch (colorSpace)
	{
	case BITMAP_COLOR_SPACE_HSV:

		newPixel.c3 = pixel.c3;

		if (pixel.c1 == 0)
		{
			newPixel.r = pixel.c2;
			newPixel.g = pixel.c2;
			newPixel.b = pixel.c2;

        	break;
		}

		bitmap_component_t region = pixel.c0 / 43;
		bitmap_component_t remainder = (pixel.c0 - (region * 43)) * 6;

		bitmap_component_t p = (pixel.c2 * (255 - pixel.c1)) >> 8;
		bitmap_component_t q = (pixel.c2 * (255 - ((pixel.c1 * remainder) >> 8))) >> 8;
		bitmap_component_t t = (pixel.c2 * (255 - ((pixel.c1 * (255 - remainder)) >> 8))) >> 8;

		switch (region)
		{
		case 0:

			newPixel.r = pixel.c2;
			newPixel.g = t;
			newPixel.b = p;

			break;

		case 1:

			newPixel.r = q;
			newPixel.g = pixel.c2;
			newPixel.b = p;

			break;

		case 2:

			newPixel.r = p;
			newPixel.g = pixel.c2;
			newPixel.b = t;

			break;

		case 3:

			newPixel.r = p;
			newPixel.g = q;
			newPixel.b = pixel.c2;

			break;

		case 4:

			newPixel.r = t;
			newPixel.g = p;
			newPixel.b = pixel.c2;

			break;

		default:

			newPixel.r = pixel.c2;
			newPixel.g = p;
			newPixel.b = q;
		}

		break;

	default:

		//RGB:
		newPixel.r = pixel.c0;
		newPixel.g = pixel.c1;
		newPixel.b = pixel.c2;
		newPixel.c3 = pixel.c3;
	}

	return newPixel;
}

bitmap_pixel_t rgbToPixel(bitmap_pixel_rgb_t pixel, bitmap_color_space_t colorSpace)
{
	bitmap_pixel_t newPixel;

	switch (colorSpace)
	{
	case BITMAP_COLOR_SPACE_HSV:

		newPixel.c3 = pixel.c3;

		bitmap_component_t rgbMin = BITMAP_MIN(pixel.r, BITMAP_MIN(pixel.g, pixel.b));
		bitmap_component_t rgbMax = BITMAP_MAX(pixel.r, BITMAP_MAX(pixel.g, pixel.b));

		newPixel.c2 = rgbMax;

		if (newPixel.c2 == 0)
		{
			newPixel.c0 = 0;
			newPixel.c1 = 0;

			break;
		}

		newPixel.c1 = (bitmap_component_t)((255 * (uint16_t)(rgbMax - rgbMin)) / rgbMax);

		if (newPixel.c1 == 0)
		{
			newPixel.c0 = 0;
			break;
		}

		if (rgbMax == pixel.r)
			newPixel.c0 = 0 + ((43 * (pixel.g - pixel.b)) / (rgbMax - rgbMin));
		else if (rgbMax == pixel.g)
			newPixel.c0 = 85 + ((43 * (pixel.b - pixel.r)) / (rgbMax - rgbMin));
		else
			newPixel.c0 = 171 + ((43 * (pixel.r - pixel.g)) / (rgbMax - rgbMin));

		break;

	default:

		//RGB:
		newPixel.c0 = pixel.r;
		newPixel.c1 = pixel.g;
		newPixel.c2 = pixel.b;
		newPixel.c3 = pixel.c3;
	}

	return newPixel;
}

/**********************************************************************************************************************************************************************
	Reading
**********************************************************************************************************************************************************************/

//Internal reading function.
//Always reads "count" bytes.
//
//Errors:
//
//- BITMAP_ERROR_IO  A read error has occurred. Includes unexpected EOF.
//
//The file will not be closed by this function.
bitmap_error_t bitmapReadBytes(FILE* file, uint8_t* buffer, int count)
{
	if (fread(buffer, 1, count, file) != count)
	{
		if (feof(file))
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "Failed to read bytes: Unexpected end of file.");
			return BITMAP_ERROR_IO;
		}

		int err = ferror(file);

		if (err)
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "Failed to read bytes: IO error (%d).", err);
			return BITMAP_ERROR_IO;
		}

		bitmapLog(BITMAP_LOGGING_DEFAULT, "Failed to read bytes: Unknown IO error.");
		return BITMAP_ERROR_IO;
	}

	return BITMAP_ERROR_SUCCESS;
}

//Some typed functions with the same behavior:
bitmap_error_t bitmapReadU8(FILE* file, uint8_t* value)
{
	return bitmapReadBytes(file, value, sizeof(uint8_t));
}

bitmap_error_t bitmapReadI8(FILE* file, int8_t* value)
{
	return bitmapReadBytes(file, (uint8_t*)value, sizeof(int8_t));
}

bitmap_error_t bitmapReadU16(FILE* file, uint16_t* value)
{
	return bitmapReadBytes(file, (uint8_t*)value, sizeof(uint16_t));
}

bitmap_error_t bitmapReadI16(FILE* file, int16_t* value)
{
	return bitmapReadBytes(file, (uint8_t*)value, sizeof(int16_t));
}

bitmap_error_t bitmapReadU32(FILE* file, uint32_t* value)
{
	return bitmapReadBytes(file, (uint8_t*)value, sizeof(uint32_t));
}

bitmap_error_t bitmapReadI32(FILE* file, int32_t* value)
{
	return bitmapReadBytes(file, (uint8_t*)value, sizeof(int32_t));
}

//Internal pixel row read function (BITMAP_COLOR_DEPTH_1).
//The buffers will not be released by this function.
void bitmapReadRowColorDepth_1(bitmap_t* bitmap, const uint8_t* rowData, bitmap_pixel_t* outputData, int rowPx)
{
	uint32_t widthPx = bitmap->parameters.widthPx;
	int pixelsRead = 0;
	int baseIndex = rowPx * widthPx;

	do
	{
		int pixelsToRead = widthPx - pixelsRead;

		if (pixelsToRead > 8)
		{
			pixelsToRead = 8;
		}

		uint8_t currByte = rowData[pixelsRead / 8];

		for (int i = 7; i >= (8 - pixelsToRead); i--)
		{
			outputData[baseIndex + pixelsRead + 7 - i] = (currByte & (1 << i)) ? bitmap->parameters.colorTable[1] : bitmap->parameters.colorTable[0];
		}

		pixelsRead += pixelsToRead;
	} while (pixelsRead < widthPx);
}

//Internal pixel row read function (BITMAP_COLOR_DEPTH_8).
//The buffers will not be released by this function.

void bitmapReadRowColorDepth_8(bitmap_t* bitmap, const uint8_t* rowData, bitmap_pixel_t* outputData, int rowPx)
{
	uint32_t widthPx = bitmap->parameters.widthPx;
	int baseIndex = rowPx * widthPx;

	for (int colPx = 0; colPx < widthPx; colPx++)
	{
		outputData[baseIndex + colPx] = bitmap->parameters.colorTable[rowData[colPx]];
	}
}

//Internal pixel row read function (BITMAP_COLOR_DEPTH_24).
//The buffers will not be released by this function.

void bitmapReadRowColorDepth_24(bitmap_t* bitmap, const uint8_t* rowData, bitmap_pixel_t* outputData, int rowPx)
{
	uint32_t widthPx = bitmap->parameters.widthPx;
	bitmap_color_space_t colorSpace = bitmap->parameters.colorSpace;
	int baseIndex = rowPx * widthPx;

	for (int colPx = 0; colPx < widthPx; colPx++)
	{
		bitmap_pixel_rgb_t currPixel;

		currPixel.r = rowData[(3 * colPx) + 2];
		currPixel.g = rowData[(3 * colPx) + 1];
		currPixel.b = rowData[(3 * colPx) + 0];
		currPixel.c3 = 0x00;

		outputData[baseIndex + colPx] = rgbToPixel(currPixel, colorSpace);
	}
}

//Internal pixel row read function (BITMAP_COLOR_DEPTH_32).
//The buffers will not be released by this function.

void bitmapReadRowColorDepth_32(bitmap_t* bitmap, const uint8_t* rowData, bitmap_pixel_t* outputData, int rowPx)
{
	uint32_t widthPx = bitmap->parameters.widthPx;
	bitmap_color_space_t colorSpace = bitmap->parameters.colorSpace;
	int baseIndex = rowPx * widthPx;

	for (int colPx = 0; colPx < widthPx; colPx++)
	{
		bitmap_pixel_rgb_t currPixel;

		currPixel.r = rowData[(4 * colPx) + 1];
		currPixel.g = rowData[(4 * colPx) + 2];
		currPixel.b = rowData[(4 * colPx) + 3];
		currPixel.c3 = rowData[(4 * colPx) + 0];

		outputData[baseIndex + colPx] = rgbToPixel(currPixel, colorSpace);
	}
}

//Internal pixel read function (BITMAP_COMPRESSION_NONE).
//If the function succeeds, the pixel pointer is valid.
//
//Errors:
//- BITMAP_ERROR_IO                   An IO error has occurred.
//- BITMAP_ERROR_MEMORY               Insufficient memory.
//
//The file will not be closed by this function.
bitmap_error_t bitmapReadPixelsCompression_None(bitmap_t* bitmap, bitmap_pixel_t** pixels)
{
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Decompressing BITMAP_COMPRESSION_NONE ...");

	//Get width and height:
	uint32_t widthPx = bitmap->parameters.widthPx;
	uint32_t heightPx = bitmap->parameters.heightPx;
	uint32_t totalPx = widthPx * heightPx;

	//Allocate space for the pixels:
	bitmap_pixel_t* outputData = (bitmap_pixel_t*)malloc(totalPx * sizeof(bitmap_pixel_t));

	if (!outputData)
	{
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Insufficient memory for allocating pixel buffer.");
		return BITMAP_ERROR_MEMORY;
	}

	//How many bytes are in a row?
	int bitsPerRow = bitmap->parameters.colorDepth * widthPx;
	int bytesPerRow = ((bitsPerRow + 31) / 32) * 4;

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Bits / bytes per row: %u / %u", bitsPerRow, bytesPerRow);

	//Allocate memory for a row:
	uint8_t* rowData = (uint8_t*)malloc(bytesPerRow);

	if (!rowData)
	{
		//Free the pixel buffer:
		free(outputData);

		bitmapLog(BITMAP_LOGGING_DEFAULT, "Insufficient memory for allocating row buffer.");
		return BITMAP_ERROR_MEMORY;
	}

	//Status var:
	bitmap_error_t success;

	//Read row by row:
	for (int rowPx = 0; rowPx < heightPx; rowPx++)
	{
		//Read the row:
		if ((success = bitmapReadBytes(bitmap->file, rowData, bytesPerRow)) != BITMAP_ERROR_SUCCESS)
		{
			break;
		}

		//Parse it, depending on the color depth:
		switch (bitmap->parameters.colorDepth)
		{
		case BITMAP_COLOR_DEPTH_1:

			bitmapReadRowColorDepth_1(bitmap, rowData, outputData, rowPx);
			break;

		case BITMAP_COLOR_DEPTH_8:

			bitmapReadRowColorDepth_8(bitmap, rowData, outputData, rowPx);
			break;

		case BITMAP_COLOR_DEPTH_24:

			bitmapReadRowColorDepth_24(bitmap, rowData, outputData, rowPx);
			break;

		case BITMAP_COLOR_DEPTH_32:

			bitmapReadRowColorDepth_32(bitmap, rowData, outputData, rowPx);
			break;

		default:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "Color depth is not (yet) supported. Sorry!");
			success = BITMAP_ERROR_INVALID_FILE_FORMAT;
		}

		//Check success:
		if (success != BITMAP_ERROR_SUCCESS)
		{
			break;
		}
	}

	//If we were successful, we assign the pointers:
	if (success == BITMAP_ERROR_SUCCESS)
	{
		*pixels = outputData;

		//Finished!
		bitmapLog(BITMAP_LOGGING_VERBOSE, "All pixels have been decompressed.");
	}
	else
	{
		//Free the pixel buffer:
		free(outputData);
	}

	//Free the row data:
	free(rowData);

	return success;
}

//Internal DIB header reading function (BITMAP_DIB_HEADER_INFO).
//
//Errors:
//
//- BITMAP_ERROR_INVALID_FILE_FORMAT  Something bad inside the DIB header. Or we don't support some feature.
//- BITMAP_ERROR_IO                   A read error has occurred.
//
//The file will not be closed by this function.
bitmap_error_t bitmapReadDIBHeader_Info(bitmap_t* bitmap)
{
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Starting to read DIB header (BITMAP_DIB_HEADER_INFO) ...");

	//Status var:
	bitmap_error_t success;

	//Read the width:
	if ((success = bitmapReadU32(bitmap->file, &(bitmap->parameters.widthPx))) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Read the height:
	int32_t heightPx;

	if ((success = bitmapReadI32(bitmap->file, &heightPx)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Top-Down or bottom-up?
	if (heightPx > 0)
	{
		bitmap->parameters.bottomUp = 1;
		bitmap->parameters.heightPx = (uint32_t)heightPx;
	}
	else
	{
		bitmap->parameters.bottomUp = 0;
		bitmap->parameters.heightPx = (uint32_t)-heightPx;
	}

	//Check size:
	if ((bitmap->parameters.widthPx == 0) || (bitmap->parameters.heightPx == 0))
	{
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Width and / or height in pixels is 0.");
		return BITMAP_ERROR_INVALID_FILE_FORMAT;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Bitmap pixel dimensions: %u x %u (width x height)", bitmap->parameters.widthPx, bitmap->parameters.heightPx);
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Bottom-up: %s", (bitmap->parameters.bottomUp ? "true" : "false"));

	//Read the biPlanes (ignored):
	uint16_t biPlanes;

	if ((success = bitmapReadU16(bitmap->file, &biPlanes)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "BiPlanes: %u (ignored)", biPlanes);

	if (biPlanes != 1)
	{
		bitmapLog(BITMAP_LOGGING_VERBOSE, "... but shouldn't that be 1 ?!");
	}

	//Read the color depth:
	if ((success = bitmapReadU16(bitmap->file, &(bitmap->parameters.colorDepth))) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	switch (bitmap->parameters.colorDepth)
	{
	case BITMAP_COLOR_DEPTH_1:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Color depth: 2 indexed colors");
		break;

	case BITMAP_COLOR_DEPTH_4:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Color depth: 16 indexed colors");
		break;

	case BITMAP_COLOR_DEPTH_8:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Color depth: 256 indexed colors");
		break;

	case BITMAP_COLOR_DEPTH_16:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Color depth: 16 bit");
		break;

	case BITMAP_COLOR_DEPTH_24:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Color depth: 24 bit");
		break;

	case BITMAP_COLOR_DEPTH_32:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Color depth: 32 bit");
		break;

	default:

		bitmapLog(BITMAP_LOGGING_DEFAULT, "Unknown color depth: %u bit", bitmap->parameters.colorDepth);
		return BITMAP_ERROR_INVALID_FILE_FORMAT;
	}

	//Read the compression:
	uint32_t rawCompression;

	if ((success = bitmapReadU32(bitmap->file, &rawCompression)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	switch (rawCompression)
	{
	case 0:

		bitmap->parameters.compression = BITMAP_COMPRESSION_NONE;
		bitmapLog(BITMAP_LOGGING_VERBOSE, "Compression: none");

		break;

	case 1:

		bitmap->parameters.compression = BITMAP_COMPRESSION_RLE;
		bitmapLog(BITMAP_LOGGING_VERBOSE, "Compression: RLE (8 bit)");

		//Make sure this is valid:
		if ((bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_8) || !(bitmap->parameters.bottomUp))
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "Invalid bitmap compression in this context.");
			return BITMAP_ERROR_INVALID_FILE_FORMAT;
		}

		break;

	case 2:

		bitmap->parameters.compression = BITMAP_COMPRESSION_RLE;
		bitmapLog(BITMAP_LOGGING_VERBOSE, "Compression: RLE (4 bit)");

		//Make sure this is valid:
		if ((bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_4) || !(bitmap->parameters.bottomUp))
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "Invalid bitmap compression in this context.");
			return BITMAP_ERROR_INVALID_FILE_FORMAT;
		}

		break;

	case 3:

		bitmap->parameters.compression = BITMAP_COMPRESSION_BITFIELD_RGB;
		bitmapLog(BITMAP_LOGGING_VERBOSE, "Compression: Bitfield (RGB)");

		//Make sure this is valid:
		if ((bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_16) && (bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_32))
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "Invalid bitmap compression in this context.");
			return BITMAP_ERROR_INVALID_FILE_FORMAT;
		}

		break;

	case 6:

		bitmap->parameters.compression = BITMAP_COMPRESSION_BITFIELD_ARGB;
		bitmapLog(BITMAP_LOGGING_VERBOSE, "Compression: Bitfield (ARGB)");

		//Make sure this is valid:
		if ((bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_16) && (bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_32))
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "Invalid bitmap compression in this context.");
			return BITMAP_ERROR_INVALID_FILE_FORMAT;
		}

		break;

	default:

		bitmapLog(BITMAP_LOGGING_DEFAULT, "Unknown bitmap compression: %u", rawCompression);
		return BITMAP_ERROR_INVALID_FILE_FORMAT;
	}

	//Read the size of the pixel data (ignored):
	uint32_t pixelDataSize;

	if ((success = bitmapReadU32(bitmap->file, &pixelDataSize)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Pixel data size in bytes: %u (ignored)", pixelDataSize);

	//Read the pixels per meter (ignored):
	uint32_t pixelsPerMeterX;

	if ((success = bitmapReadU32(bitmap->file, &pixelsPerMeterX)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	uint32_t pixelsPerMeterY;

	if ((success = bitmapReadU32(bitmap->file, &pixelsPerMeterY)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Bitmap destination resolution: %u p/m in X, %u p/m in Y (ignored)", pixelsPerMeterX, pixelsPerMeterY);

	//Read the number of valid color table entries:
	uint32_t colorTableEntries;

	if ((success = bitmapReadU32(bitmap->file, &colorTableEntries)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Correct potential zeros:
	if (colorTableEntries == 0)
	{
		switch (bitmap->parameters.colorDepth)
		{
		case BITMAP_COLOR_DEPTH_1:

			colorTableEntries = 2;
			break;

		case BITMAP_COLOR_DEPTH_4:

			colorTableEntries = 16;
			break;

		case BITMAP_COLOR_DEPTH_8:

			colorTableEntries = 256;
			break;
		}
	}
	else
	{
		//Clamp the value if necessary:
		if (colorTableEntries > 256)
		{
			bitmapLog(BITMAP_LOGGING_VERBOSE, "Clamping number of color table entries from %u to 256.", colorTableEntries);
			colorTableEntries = 256;
		}
	}

	bitmap->parameters.colorTableEntries = colorTableEntries;
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Color table entries: %u", bitmap->parameters.colorTableEntries);

	//Read the number of important color table entries (ignored):
	uint32_t importantColorTableEntries;

	if ((success = bitmapReadU32(bitmap->file, &importantColorTableEntries)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Important color table entries: %u (ignored)", importantColorTableEntries);

	//Depending on the compression, we have to read the bitmasks now:
	if (bitmap->parameters.compression == BITMAP_COMPRESSION_BITFIELD_RGB)
	{
		if ((success = bitmapReadBytes(bitmap->file, (uint8_t*)&(bitmap->parameters.bitmasks), 3 * sizeof(uint32_t))) != BITMAP_ERROR_SUCCESS)
		{
			return success;
		}

		bitmapLog(BITMAP_LOGGING_VERBOSE, "RGB bitmasks have been read.");
	}
	else if (bitmap->parameters.compression == BITMAP_COMPRESSION_BITFIELD_ARGB)
	{
		if ((success = bitmapReadBytes(bitmap->file, (uint8_t*)&(bitmap->parameters.bitmasks), 4 * sizeof(uint32_t))) != BITMAP_ERROR_SUCCESS)
		{
			return success;
		}

		bitmapLog(BITMAP_LOGGING_VERBOSE, "ARGB bitmasks have been read.");
	}
	else
	{
		bitmapLog(BITMAP_LOGGING_VERBOSE, "No bitmasks have been read.");
	}

	//Finally, read the color table:
	if (bitmap->parameters.colorTableEntries)
	{
		bitmap_pixel_rgb_t colorTable[256];

		if ((success = bitmapReadBytes(bitmap->file, (uint8_t*)&colorTable, bitmap->parameters.colorTableEntries * sizeof(bitmap_pixel_rgb_t))) != BITMAP_ERROR_SUCCESS)
		{
			return success;
		}

		//Now shuffle the table to the right order:
		bitmapLog(BITMAP_LOGGING_VERBOSE, "Color table (RGBA):");

		for (int i = 0; i < bitmap->parameters.colorTableEntries; i++)
		{
			bitmap_pixel_rgb_t currPixel = colorTable[i];
			bitmap_pixel_rgb_t newPixel;

			newPixel.r = currPixel.b;
			newPixel.g = currPixel.g;
			newPixel.b = currPixel.r;
			newPixel.c3 = currPixel.c3;

			bitmapLog(BITMAP_LOGGING_VERBOSE, " (0x%02X, 0x%02X, 0x%02X, 0x%02X)", newPixel.r, newPixel.g, newPixel.b, newPixel.c3);

			//Convert the pixel to our color space and insert it:
			bitmap->parameters.colorTable[i] = rgbToPixel(newPixel, bitmap->parameters.colorSpace);
		}
	}
	else
	{
		bitmapLog(BITMAP_LOGGING_VERBOSE, "No color table has been read.");
	}

	//Okay, that worked :-)
	bitmapLog(BITMAP_LOGGING_VERBOSE, "DIB header (info) has successfully been read.");

	return BITMAP_ERROR_SUCCESS;
}

//Internal header reading function.
//
//Errors:
//
//- BITMAP_ERROR_INVALID_FILE_FORMAT  Something bad inside the header. Or we don't support some feature.
//- BITMAP_ERROR_IO                   A read error has occurred.
//
//The file will not be closed by this function.
bitmap_error_t bitmapReadHeader(bitmap_t* bitmap)
{
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Starting to read header ...");

	//Status var:
	bitmap_error_t success;

	//Read and verify the magic number:
	uint16_t magicNumber;

	if ((success = bitmapReadU16(bitmap->file, &magicNumber)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	if (magicNumber != BITMAP_MAGIC_NUMBER)
	{
		bitmapLog(BITMAP_LOGGING_VERBOSE, "Wrong magic number (0x%04X). This does not look like a bitmap.", magicNumber);
		return BITMAP_ERROR_INVALID_FILE_FORMAT;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Magic number is a match.");

	//Read and ignore the size in bytes:
	uint32_t sizeInBytes;

	if ((success = bitmapReadU32(bitmap->file, &sizeInBytes)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Size in bytes: %u (ignored)", sizeInBytes);

	//Read and ignore the writer-specific blob:
	uint32_t writerSpecific;

	if ((success = bitmapReadU32(bitmap->file, &writerSpecific)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writer-specific blob: %u (ignored)", writerSpecific);

	//Read the pixel offset:
	if ((success = bitmapReadU32(bitmap->file, &(bitmap->pixelOffset))) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Pixel offset: 0x%04X", bitmap->pixelOffset);

	//Read the size of the info header structure:
	uint32_t infoHeaderSize;

	if ((success = bitmapReadU32(bitmap->file, &infoHeaderSize)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Info header size in bytes: %u", infoHeaderSize);

	//The size tells us about the type.
	switch (infoHeaderSize)
	{
		case BITMAP_DIB_HEADER_CORE_OS2_1X:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Identified DIB header: BITMAP_DIB_HEADER_CORE_OS2_1X");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

		case BITMAP_DIB_HEADER_OS2_2X:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Identified DIB header: BITMAP_DIB_HEADER_OS2_2X");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

		case BITMAP_DIB_HEADER_OS2_2X_SHORT:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Identified DIB header: BITMAP_DIB_HEADER_OS2_2X_SHORT");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

        case BITMAP_DIB_HEADER_INFO_V5:

            bitmapLog(BITMAP_LOGGING_VERBOSE, "Identified DIB header: BITMAP_DIB_HEADER_INFO_V5");
            bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported.");
            bitmapLog(BITMAP_LOGGING_DEFAULT, "Trying fallback to V4 ...");

        case BITMAP_DIB_HEADER_INFO_V4:

            bitmapLog(BITMAP_LOGGING_VERBOSE, "Identified DIB header: BITMAP_DIB_HEADER_INFO_V4");
            bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported.");
            bitmapLog(BITMAP_LOGGING_DEFAULT, "Trying fallback to V3 ...");

        case BITMAP_DIB_HEADER_INFO_V3:

            bitmapLog(BITMAP_LOGGING_VERBOSE, "Identified DIB header: BITMAP_DIB_HEADER_INFO_V3");
            bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported.");
            bitmapLog(BITMAP_LOGGING_DEFAULT, "Trying fallback to V2 ...");

        case BITMAP_DIB_HEADER_INFO_V2:

            bitmapLog(BITMAP_LOGGING_VERBOSE, "Identified DIB header: BITMAP_DIB_HEADER_INFO_V2");
            bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported.");
            bitmapLog(BITMAP_LOGGING_DEFAULT, "Trying fallback to V1 ...");

		case BITMAP_DIB_HEADER_INFO:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Identified DIB header: BITMAP_DIB_HEADER_INFO");

			if ((success = bitmapReadDIBHeader_Info(bitmap)))
			{
				return success;
			}

			break;

		default:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is unknown.");
			return BITMAP_ERROR_INVALID_FILE_FORMAT;
	}

	//That's it:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Bitmap header parsing has succeeded!");

	return BITMAP_ERROR_SUCCESS;
}

//Internal file open function.
//If the function succeeds, the file pointer in the bitmap struct is valid.
//
//Errors:
//- BITMAP_ERROR_INVALID_PATH         The given path is not valid in any way (missing file, bad permissions etc.).
//- BITMAP_ERROR_INVALID_FILE_FORMAT  The given file is no valid bitmap.
//- BITMAP_ERROR_IO                   An IO error has occurred.
//- BITMAP_ERROR_MEMORY               Insufficient memory.
//
bitmap_error_t bitmapOpenFile(bitmap_t* bitmap, const char* filePath)
{
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Going to open file \"%s\" ...", filePath);

	//Try to open the file:
	FILE* file = fopen(filePath, "rb");

	if (!file)
	{
		switch (errno)
		{
		case ENOENT:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "File path to \"%s\" is invalid.", filePath);
			return BITMAP_ERROR_INVALID_PATH;

		case ENOMEM:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "Insufficient memory for opening file.");
			return BITMAP_ERROR_MEMORY;

		default:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "Error opening file at \"%s\": %s", filePath, strerror(errno));
			return BITMAP_ERROR_INVALID_PATH;
		}
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "File has been opened successfully.");

	//Set file pointer:
	bitmap->file = file;

	return BITMAP_ERROR_SUCCESS;
}

//User-accessible.
bitmap_error_t bitmapReadPixels(const char* filePath, bitmap_pixel_t** pixels, int* widthPx, int* heightPx, bitmap_color_space_t colorSpace)
{
	//NULL the pointers:
	*pixels = NULL;
	*widthPx = 0;
	*heightPx = 0;

	//Init a bitmap struct:
	bitmap_t bitmap;
	memset(&bitmap, 0, sizeof(bitmap_t));

	//Assign our color space:
	bitmap.parameters.colorSpace = colorSpace;

	//Status var:
	bitmap_error_t success;

	//Open the bitmap file for reading:
	if ((success = bitmapOpenFile(&bitmap, filePath)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Read the bitmap header:
	if ((success = bitmapReadHeader(&bitmap)) != BITMAP_ERROR_SUCCESS)
	{
		//Close the file:
		fclose(bitmap.file);

		return success;
	}

	//Jump to the pixel offset:
	fseek(bitmap.file, bitmap.pixelOffset, SEEK_SET);
	int err = ferror(bitmap.file);

	if (err)
	{
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Failed to seek pixel offset: IO error (%d).", err);

		//Close the file:
		fclose(bitmap.file);

		return BITMAP_ERROR_IO;
	}

	//Switch over the compression.
	//Only BITMAP_COMPRESSION_NONE for the moment.
	switch (bitmap.parameters.compression)
	{
	case BITMAP_COMPRESSION_NONE:

		success = bitmapReadPixelsCompression_None(&bitmap, pixels);
		break;

	case BITMAP_COMPRESSION_RLE:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Decompressing BITMAP_COMPRESSION_RLE ...");
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Compression scheme is not (yet) supported. Sorry!");

		success = BITMAP_ERROR_INVALID_FILE_FORMAT;
		break;

	case BITMAP_COMPRESSION_BITFIELD_RGB:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Decompressing BITMAP_COMPRESSION_BITFIELD_RGB ...");
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Compression scheme is not (yet) supported. Sorry!");

		success = BITMAP_ERROR_INVALID_FILE_FORMAT;
		break;

	case BITMAP_COMPRESSION_BITFIELD_ARGB:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Decompressing BITMAP_COMPRESSION_BITFIELD_ARGB ...");
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Compression scheme is not (yet) supported. Sorry!");

		success = BITMAP_ERROR_INVALID_FILE_FORMAT;
		break;

	default:

		bitmapLog(BITMAP_LOGGING_DEFAULT, "Unknown compression scheme.");
		success = BITMAP_ERROR_INVALID_FILE_FORMAT;

		break;
	}

	//Close the file:
	fclose(bitmap.file);

	//If that has worked, we can set the dimensions now.
	if (success == BITMAP_ERROR_SUCCESS)
	{
		*widthPx = bitmap.parameters.widthPx;
		*heightPx = bitmap.parameters.heightPx;
	}

	return success;
}

/**********************************************************************************************************************************************************************
	Writing
**********************************************************************************************************************************************************************/

//Internal writing function.
//Always writes "count" bytes.
//
//Errors:
//
//- BITMAP_ERROR_IO  A write error has occurred.
//
//The file will not be closed by this function.
bitmap_error_t bitmapWriteBytes(FILE* file, uint8_t* buffer, int count)
{
	if (fwrite(buffer, 1, count, file) != count)
	{
		int err = ferror(file);

		if (err)
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "Failed to write bytes: IO error (%d).", err);
			return BITMAP_ERROR_IO;
		}

		bitmapLog(BITMAP_LOGGING_DEFAULT, "Failed to write bytes: Unknown IO error.");
		return BITMAP_ERROR_IO;
	}

	return BITMAP_ERROR_SUCCESS;
}

//Some typed functions with the same behavior:
bitmap_error_t bitmapWriteU8(FILE* file, uint8_t value)
{
	return bitmapWriteBytes(file, &value, sizeof(uint8_t));
}

bitmap_error_t bitmapWriteI8(FILE* file, int8_t value)
{
	return bitmapWriteBytes(file, (uint8_t*)&value, sizeof(int8_t));
}

bitmap_error_t bitmapWriteU16(FILE* file, uint16_t value)
{
	return bitmapWriteBytes(file, (uint8_t*)&value, sizeof(uint16_t));
}

bitmap_error_t bitmapWriteI16(FILE* file, int16_t value)
{
	return bitmapWriteBytes(file, (uint8_t*)&value, sizeof(int16_t));
}

bitmap_error_t bitmapWriteU32(FILE* file, uint32_t value)
{
	return bitmapWriteBytes(file, (uint8_t*)&value, sizeof(uint32_t));
}

bitmap_error_t bitmapWriteI32(FILE* file, int32_t value)
{
	return bitmapWriteBytes(file, (uint8_t*)&value, sizeof(int32_t));
}

//Internal pixel row writing function (BITMAP_COLOR_DEPTH_24).
//
//Errors:
//- BITMAP_ERROR_IO                   An IO error has occurred.
//
//The file will not be closed by this function.
bitmap_error_t bitmapWriteRowColorDepth_24(bitmap_t* bitmap, const bitmap_pixel_t* rowData)
{
	//Get the width:
	uint32_t widthPx = bitmap->parameters.widthPx;

	//Get the color space:
	bitmap_color_space_t colorSpace = bitmap->parameters.colorSpace;

	//Status var:
	bitmap_error_t success;

	for (int colPx = 0; colPx < widthPx; colPx++)
	{
		bitmap_pixel_rgb_t currPixel = pixelToRGB(rowData[colPx], colorSpace);
		bitmap_component_t pixelData[3] = { currPixel.b, currPixel.g, currPixel.r };

		if ((success = bitmapWriteBytes(bitmap->file, (uint8_t*)pixelData, sizeof(pixelData))) != BITMAP_ERROR_SUCCESS)
		{
			return success;
		}
	}

	return BITMAP_ERROR_SUCCESS;
}

//Internal pixel row writing function (BITMAP_COLOR_DEPTH_32).
//
//Errors:
//- BITMAP_ERROR_IO                   An IO error has occurred.
//
//The file will not be closed by this function.
bitmap_error_t bitmapWriteRowColorDepth_32(bitmap_t* bitmap, const bitmap_pixel_t* rowData)
{
	//Get the width:
	uint32_t widthPx = bitmap->parameters.widthPx;

	//Get the color space:
	bitmap_color_space_t colorSpace = bitmap->parameters.colorSpace;

	//Status var:
	bitmap_error_t success;

	for (int colPx = 0; colPx < widthPx; colPx++)
	{
		bitmap_pixel_rgb_t currPixel = pixelToRGB(rowData[colPx], colorSpace);
		bitmap_component_t pixelData[4] = { currPixel.c3, currPixel.b, currPixel.g, currPixel.r };

		if ((success = bitmapWriteBytes(bitmap->file, (uint8_t*)pixelData, sizeof(pixelData))) != BITMAP_ERROR_SUCCESS)
		{
			return success;
		}
	}

	return BITMAP_ERROR_SUCCESS;
}

//Internal pixel writing function (BITMAP_COMPRESSION_NONE).
//
//Errors:
//- BITMAP_ERROR_IO                   An IO error has occurred.
//
//The file will not be closed by this function.
bitmap_error_t bitmapWritePixelsCompression_None(bitmap_t* bitmap, const bitmap_pixel_t* pixels)
{
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Compressing BITMAP_COMPRESSION_NONE ...");

	//Get width and height:
	uint32_t widthPx = bitmap->parameters.widthPx;
	uint32_t heightPx = bitmap->parameters.heightPx;

	//How many bytes are in a row?
	int bitsPerRow = bitmap->parameters.colorDepth * widthPx;
	int bytesPerRow = ((bitsPerRow + 31) / 32) * 4;

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Bits / bytes per row: %u / %u", bitsPerRow, bytesPerRow);

	//Status var:
	bitmap_error_t success;

	//Write row by row:
	for (int rowPx = 0; rowPx < heightPx; rowPx++)
	{
		//Get the row:
		const bitmap_pixel_t* rowData = &pixels[(rowPx * widthPx)];

		//Write it, depending on the color depth:
		switch (bitmap->parameters.colorDepth)
		{
		case BITMAP_COLOR_DEPTH_24:

			bitmapWriteRowColorDepth_24(bitmap, rowData);
			success = BITMAP_ERROR_SUCCESS;

			break;

		case BITMAP_COLOR_DEPTH_32:

			bitmapWriteRowColorDepth_32(bitmap, rowData);
			success = BITMAP_ERROR_SUCCESS;

			break;

		default:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "Color depth is not (yet) supported. Sorry!");
			success = BITMAP_ERROR_INVALID_FILE_FORMAT;
		}

		//Check success:
		if (success != BITMAP_ERROR_SUCCESS)
		{
			break;
		}
	}

	//Finished!
	bitmapLog(BITMAP_LOGGING_VERBOSE, "All pixels have been compressed.");

	return success;
}

//Internal DIB header writing function (BITMAP_DIB_HEADER_INFO).
//
//Errors:
//
//- BITMAP_ERROR_INVALID_FILE_FORMAT  Something bad inside the parameters. Or we don't support some feature.
//- BITMAP_ERROR_IO                   A read error has occurred.
//
//The file will not be closed by this function.
bitmap_error_t bitmapWriteDIBHeader_Info(bitmap_t* bitmap)
{
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Starting to write DIB header (BITMAP_DIB_HEADER_INFO) ...");

	//Status var:
	bitmap_error_t success;

	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing DIB header length ...");

	if ((success = bitmapWriteU32(bitmap->file, BITMAP_DIB_HEADER_INFO)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write pixel width:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing pixel width ...");

	if ((success = bitmapWriteU32(bitmap->file, bitmap->parameters.widthPx)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write pixel height:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing pixel height ...");

	int32_t heightPx = bitmap->parameters.heightPx;

	if (!bitmap->parameters.bottomUp)
	{
		bitmapLog(BITMAP_LOGGING_VERBOSE, "Negating pixel height (because we want top-down) ...");
		heightPx *= -1;
	}

	if ((success = bitmapWriteI32(bitmap->file, heightPx)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write BiPlanes:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing BiPlanes ...");

	if ((success = bitmapWriteU16(bitmap->file, 1)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Switch over color depth:
	switch (bitmap->parameters.colorDepth)
	{
	case BITMAP_COLOR_DEPTH_1:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing color depth: 2 indexed colors ...");
		break;

	case BITMAP_COLOR_DEPTH_4:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing color depth: 16 indexed colors ...");
		break;

	case BITMAP_COLOR_DEPTH_8:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing color depth: 256 indexed colors ...");
		break;

	case BITMAP_COLOR_DEPTH_16:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing color depth: 16 bit ...");
		break;

	case BITMAP_COLOR_DEPTH_24:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing color depth: 24 bit ...");
		break;

	case BITMAP_COLOR_DEPTH_32:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing color depth: 32 bit ...");
		break;

	default:

		bitmapLog(BITMAP_LOGGING_DEFAULT, "Unknown color depth: %u bit", bitmap->parameters.colorDepth);
		return BITMAP_ERROR_INVALID_FILE_FORMAT;
	}

	if ((success = bitmapWriteU16(bitmap->file, bitmap->parameters.colorDepth)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Switch over compression:
	switch (bitmap->parameters.compression)
	{
	case BITMAP_COMPRESSION_NONE:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing compression: BITMAP_COMPRESSION_NONE ...");

		if ((success = bitmapWriteU32(bitmap->file, 0)) != BITMAP_ERROR_SUCCESS)
		{
			return success;
		}

		break;

	case BITMAP_COMPRESSION_RLE:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing compression: BITMAP_COMPRESSION_RLE ...");

		switch (bitmap->parameters.colorDepth)
		{
		case BITMAP_COLOR_DEPTH_8:

			if ((success = bitmapWriteU32(bitmap->file, 1)) != BITMAP_ERROR_SUCCESS)
			{
				return success;
			}

			break;

		case BITMAP_COLOR_DEPTH_4:

			if ((success = bitmapWriteU32(bitmap->file, 2)) != BITMAP_ERROR_SUCCESS)
			{
				return success;
			}

			break;

		default:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "Invalid bitmap compression in this context.");
			return BITMAP_ERROR_INVALID_FILE_FORMAT;
		}

		break;

	case BITMAP_COMPRESSION_BITFIELD_RGB:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing compression: BITMAP_COMPRESSION_BITFIELD_RGB ...");

		if ((bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_16) && (bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_32))
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "Invalid bitmap compression in this context.");
			return BITMAP_ERROR_INVALID_FILE_FORMAT;
		}

		if ((success = bitmapWriteU32(bitmap->file, 3)) != BITMAP_ERROR_SUCCESS)
		{
			return success;
		}

		break;

	case BITMAP_COMPRESSION_BITFIELD_ARGB:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing compression: BITMAP_COMPRESSION_BITFIELD_ARGB ...");

		if ((bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_16) && (bitmap->parameters.colorDepth != BITMAP_COLOR_DEPTH_32))
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "Invalid bitmap compression in this context.");
			return BITMAP_ERROR_INVALID_FILE_FORMAT;
		}

		if ((success = bitmapWriteU32(bitmap->file, 6)) != BITMAP_ERROR_SUCCESS)
		{
			return success;
		}

		break;

	default:

		bitmapLog(BITMAP_LOGGING_DEFAULT, "Unknown bitmap compression.");
		return BITMAP_ERROR_INVALID_FILE_FORMAT;
	}

	//Write the size of the pixel data (TODO: Fix for other compressions):
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing pixel data size in bytes (currently 0, fixme) ...");

	if ((success = bitmapWriteU32(bitmap->file, 0)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write the bitmap destination resolution:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing bitmap destination resolution ...");

	if ((success = bitmapWriteU32(bitmap->file, 0)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	if ((success = bitmapWriteU32(bitmap->file, 0)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write the color table entries (TODO: Fix for other compressions):
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing color table entries (currently 0, fixme) ...");

	if ((success = bitmapWriteU32(bitmap->file, 0)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write the important color table entries (TODO: Fix for other compressions):
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing important color table entries (currently 0, fixme) ...");

	if ((success = bitmapWriteU32(bitmap->file, 0)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write the bitmasks:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing bitmasks (currently not at all, fixme) ...");

	//Write the color table:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing color table (currently not at all, fixme) ...");

	//Okay, that worked :-)
	bitmapLog(BITMAP_LOGGING_VERBOSE, "DIB header (info) has successfully been written.");

	return BITMAP_ERROR_SUCCESS;
}



//Internal header writing function.
//
//Errors:
//
//- BITMAP_ERROR_INVALID_FILE_FORMAT  Something bad inside the header. Or we don't support some feature.
//- BITMAP_ERROR_IO                   A read error has occurred.
//
//The file will not be closed by this function.
bitmap_error_t bitmapWriteHeader(bitmap_t* bitmap)
{
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Starting to write header ...");

	//Status var:
	bitmap_error_t success;

	//Write magic number:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing magic number ...");

	if ((success = bitmapWriteU16(bitmap->file, BITMAP_MAGIC_NUMBER)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write the size in bytes (0 as dummy, fill this in later):
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing file size in bytes (dummy) ...");

	if ((success = bitmapWriteU32(bitmap->file, 0)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write the writer-specific blob:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing writer-specific blob ...");

	if ((success = bitmapWriteU32(bitmap->file, 0)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Write the pixel offset (0 as dummy, fill this in later):
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing pixel offset (dummy) ...");

	if ((success = bitmapWriteU32(bitmap->file, 0)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Which DIB header format do we use?
	switch (bitmap->parameters.dibHeaderFormat)
	{
		case BITMAP_DIB_HEADER_CORE_OS2_1X:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing DIB header size: BITMAP_DIB_HEADER_CORE_OS2_1X");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

		case BITMAP_DIB_HEADER_OS2_2X:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing DIB header size: BITMAP_DIB_HEADER_OS2_2X");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

		case BITMAP_DIB_HEADER_OS2_2X_SHORT:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing DIB header size: BITMAP_DIB_HEADER_OS2_2X_SHORT");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

		case BITMAP_DIB_HEADER_INFO:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Identified DIB header: BITMAP_DIB_HEADER_INFO");

			if ((success = bitmapWriteDIBHeader_Info(bitmap)) != BITMAP_ERROR_SUCCESS)
			{
				return success;
			}

			break;

		case BITMAP_DIB_HEADER_INFO_V2:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing DIB header size: BITMAP_DIB_HEADER_INFO_V2");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

		case BITMAP_DIB_HEADER_INFO_V3:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing DIB header size: BITMAP_DIB_HEADER_INFO_V3");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

		case BITMAP_DIB_HEADER_INFO_V4:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing DIB header size: BITMAP_DIB_HEADER_INFO_V4");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

		case BITMAP_DIB_HEADER_INFO_V5:

			bitmapLog(BITMAP_LOGGING_VERBOSE, "Writing DIB header size: BITMAP_DIB_HEADER_INFO_V5");
			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is not (yet) supported. Sorry!");

			return BITMAP_ERROR_INVALID_FILE_FORMAT;

		default:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "DIB header format is unknown.");
			return BITMAP_ERROR_INVALID_FILE_FORMAT;
	}

	//That's it:
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Bitmap header writing has succeeded!");

	return BITMAP_ERROR_SUCCESS;
}

//Internal file creation function.
//If the function succeeds, the file pointer in the bitmap struct is valid.
//
//Errors:
//
//- BITMAP_ERROR_INVALID_PATH  The given path is not valid in any way (missing intermediates, bad permissions etc.).
//- BITMAP_ERROR_MEMORY        Insufficient memory.
//- BITMAP_ERROR_FILE_EXISTS   The file at the given path already exists (and overwriteExisting is false).
bitmap_error_t bitmapCreateFile(bitmap_t* bitmap, const char* filePath, bitmap_bool_t overwriteExisting)
{
	bitmapLog(BITMAP_LOGGING_VERBOSE, "Going to create file \"%s\" ...", filePath);

	//Check the access to the file path:
	if (access(filePath, R_OK | W_OK) == 0)
	{
		//The file already exists. Is that allowed?
		if (!overwriteExisting)
		{
			bitmapLog(BITMAP_LOGGING_DEFAULT, "File at \"%s\" already exists.", filePath);
			bitmapLog(BITMAP_LOGGING_VERBOSE, "Set \"overwriteExisting = true\" to ignore that case.");

			return BITMAP_ERROR_FILE_EXISTS;
		}

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Overwriting existing file because of BITMAP_FLAG_OVERWRITE_EXISTING ...");
	}
	else
	{
		switch (errno)
		{
		case ENOENT:

			//This should be the normal case because the file does not exist.
			//It might also appear because of invalid path components. That will be revealed at fopen(...).
			break;

		case ENOMEM:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "Insufficient memory for accessing the file.");
			return BITMAP_ERROR_MEMORY;

		default:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "Error accessing file at \"%s\": %s", filePath, strerror(errno));
			return BITMAP_ERROR_INVALID_PATH;
		}
	}

	//Try to open the file:
	FILE* file = fopen(filePath, "wb+");

	if (!file)
	{
		switch (errno)
		{
		case ENOENT:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "File path to \"%s\" is invalid. Are the intermediate directories there?", filePath);
			return BITMAP_ERROR_INVALID_PATH;

		case ENOMEM:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "Insufficient memory for creating file.");
			return BITMAP_ERROR_MEMORY;

		default:

			bitmapLog(BITMAP_LOGGING_DEFAULT, "Error creating file at \"%s\": %s", filePath, strerror(errno));
			return BITMAP_ERROR_INVALID_PATH;
		}
	}

	bitmapLog(BITMAP_LOGGING_VERBOSE, "File has been created successfully.");

	//Set file pointer:
	bitmap->file = file;

	return BITMAP_ERROR_SUCCESS;
}

bitmap_error_t bitmapWritePixels(const char* filePath, bitmap_bool_t overwriteExisting, bitmap_parameters_t* parameters, const bitmap_pixel_t* pixels)
{
	//Init a bitmap struct:
	bitmap_t bitmap;
	memset(&bitmap, 0, sizeof(bitmap_t));

	//Status var:
	bitmap_error_t success;

	//Create the bitmap file:
	if ((success = bitmapCreateFile(&bitmap, filePath, overwriteExisting)) != BITMAP_ERROR_SUCCESS)
	{
		return success;
	}

	//Copy the parameters:
	bitmap.parameters = *parameters;

	//Write the header:
	if ((success = bitmapWriteHeader(&bitmap)) != BITMAP_ERROR_SUCCESS)
	{
		//Close the file:
		fclose(bitmap.file);

		return success;
	}

	//Remember the pixel offset:
	bitmap.pixelOffset = ftell(bitmap.file);

	//Switch over the compression.
	//Only BITMAP_COMPRESSION_NONE for the moment.
	switch (bitmap.parameters.compression)
	{
	case BITMAP_COMPRESSION_NONE:

		success = bitmapWritePixelsCompression_None(&bitmap, pixels);
		break;

	case BITMAP_COMPRESSION_RLE:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Compressing BITMAP_COMPRESSION_RLE ...");
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Compression scheme is not (yet) supported. Sorry!");

		success = BITMAP_ERROR_INVALID_FILE_FORMAT;
		break;

	case BITMAP_COMPRESSION_BITFIELD_RGB:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Compressing BITMAP_COMPRESSION_BITFIELD_RGB ...");
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Compression scheme is not (yet) supported. Sorry!");

		success = BITMAP_ERROR_INVALID_FILE_FORMAT;
		break;

	case BITMAP_COMPRESSION_BITFIELD_ARGB:

		bitmapLog(BITMAP_LOGGING_VERBOSE, "Compressing BITMAP_COMPRESSION_BITFIELD_ARGB ...");
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Compression scheme is not (yet) supported. Sorry!");

		success = BITMAP_ERROR_INVALID_FILE_FORMAT;
		break;

	default:

		bitmapLog(BITMAP_LOGGING_DEFAULT, "Unknown compression scheme.");
		success = BITMAP_ERROR_INVALID_FILE_FORMAT;

		break;
	}

	//Get the file length:
	uint32_t fileSize = ftell(bitmap.file);

	//Seek the offset for it:
	fseek(bitmap.file, 2, SEEK_SET);
	int err = ferror(bitmap.file);

	if (err)
	{
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Failed to seek file size offset: IO error (%d).", err);

		//Close the file:
		fclose(bitmap.file);

		return BITMAP_ERROR_IO;
	}

	//Write the file size:
	if ((success = bitmapWriteU32(bitmap.file, fileSize)) != BITMAP_ERROR_SUCCESS)
	{
		//Close the file:
		fclose(bitmap.file);

		return BITMAP_ERROR_IO;
	}

	//Seek the offset for the pixel offset:
	fseek(bitmap.file, 10, SEEK_SET);
	err = ferror(bitmap.file);

	if (err)
	{
		bitmapLog(BITMAP_LOGGING_DEFAULT, "Failed to seek pixel offset offset: IO error (%d).", err);

		//Close the file:
		fclose(bitmap.file);

		return BITMAP_ERROR_IO;
	}

	//Write the pixel offset:
	if ((success = bitmapWriteU32(bitmap.file, bitmap.pixelOffset)) != BITMAP_ERROR_SUCCESS)
	{
		//Close the file:
		fclose(bitmap.file);

		return BITMAP_ERROR_IO;
	}

	//Flush:
	fflush(bitmap.file);

	//Close the file:
	fclose(bitmap.file);

	return BITMAP_ERROR_SUCCESS;
}
