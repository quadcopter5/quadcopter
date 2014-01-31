/*
	Philip Romano
	7/27/2013
	loadimage.cpp
	
	Simplified interfaces for loading images.
*/

#include <string>
#include <vector>
#include <string.h>

#include <png.h>
#include <jpeglib.h>
#include <jerror.h>

#include "error.h"
#include "graphics/loadimage.h"

/*
	PNG Loading Routine
*/

unsigned char *loadPNG(const std::string &filename, int *width, int *height)
{
	png_image img;
	memset(&img, 0, sizeof(png_image));
	img.version = PNG_IMAGE_VERSION;
	
	char *filecontents = 0, *temp;
	unsigned int filebytes = 0;
	Filesystem::File *infile;
	
	try {
		infile = Filesystem::openRead(filename);
	} catch (FileException &e) {
		THROW_EXCEPT(GraphicLoadException, "The file \"" + filename
				+ "\" could not be opened for reading");
	}
	
	try {
		char buffer[4096];
		unsigned int bytes = 4096;
		while (bytes == 4096)
		{
			bytes = Filesystem::read(infile, buffer, 4096);
			
			temp = filecontents;
			filecontents = new char[filebytes + bytes];
			memcpy(filecontents, temp, filebytes);  // Copy previous to new array
			delete[] temp;
			memcpy(filecontents + filebytes, buffer, bytes); // Append new data
			
			filebytes += bytes;
		}
	} catch (FileException &e) {
		try {
			Filesystem::close(infile);
		} catch (FileException &e) { /* Don't care if close failed */ }
		delete[] filecontents;
		THROW_EXCEPT(GraphicLoadException, "An error occurred while reading \""
				+ filename + "\"");
	}
	
	std::string message;
	if (png_image_begin_read_from_memory(&img, filecontents, filebytes))
	{
		int imgwidth = img.width,
			 imgheight = img.height;
		unsigned char *image_data
				= new unsigned char[PNG_IMAGE_SIZE(img)];
		
		img.format = PNG_FORMAT_RGBA;
		if (png_image_finish_read(&img, NULL, image_data, 0, NULL))
		{
			png_image_free(&img);
			
			if (width)
				*width = imgwidth;
			if (height)
				*height = imgheight;
			
			delete[] filecontents;
			return image_data;
		}
		else
		{
			png_image_free(&img);
			delete[] image_data;
			message = "Could not read \"" + filename + "\" as PNG image";
		}
	}
	else
		message = "Failed to begin reading PNG image from \"" + filename + "\"";
	
	delete[] filecontents;
	if (img.warning_or_error)
		message += ": " + std::string(img.message);
	THROW_EXCEPT(GraphicLoadException, message);
}

/*
	JPEG Loading Routine
*/

/**
	Definitions for libjpeg error handling routines.
	
	jpegErrorExit() always throws GraphicLoadException to return control to the
	application.
*/
static void jpegErrorExit(j_common_ptr cinfo);
static void jpegErrorEmitMessage(j_common_ptr cinfo, int msg_level);
static void jpegErrorOutputMessage(j_common_ptr cinfo);

/**
	Definitions for custom libjpeg data source manager routines.
*/
struct SourceManager
{
	jpeg_source_mgr  mgr;
	Filesystem::File *file;
	JOCTET           *buffer;
	bool             file_started;
};

#define INPUT_BUF_SIZE	4096

static void jpegCreateSource(j_decompress_ptr cinfo, Filesystem::File *infile);
static void jpegInitSource(j_decompress_ptr cinfo);
static boolean jpegFillInputBuffer(j_decompress_ptr cinfo);
static void jpegSkipInputData(j_decompress_ptr cinfo, long num_bytes);
static void jpegTermSource(j_decompress_ptr cinfo);

unsigned char *loadJPEG(const std::string &filename, int *width, int *height)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr errmgr;
	cinfo.err = jpeg_std_error(&errmgr);
	errmgr.error_exit = jpegErrorExit;
	errmgr.emit_message = jpegErrorEmitMessage;
	errmgr.output_message = jpegErrorOutputMessage;
	
	unsigned char *image_data = 0;
	
	try {
		jpeg_create_decompress(&cinfo);
	} catch (GraphicLoadException &e) {
		THROW_EXCEPT(GraphicLoadException,
				"Failed to create JPEG decompression object");
	}
	
	try {
		Filesystem::File *imagefile = Filesystem::openRead(filename);
		
		jpegCreateSource(&cinfo, imagefile);
		jpeg_read_header(&cinfo, TRUE);
		
		cinfo.out_color_space = JCS_RGB;
		cinfo.output_components = 3;
		
		image_data = new unsigned char[cinfo.image_width
										  * cinfo.image_height
										  * cinfo.output_components];
		unsigned char *current = image_data;
		
		jpeg_start_decompress(&cinfo);
		
		unsigned int totallines = 0, lines;
		while (totallines < cinfo.output_height)
		{
			lines = jpeg_read_scanlines(&cinfo, &current, 1);
			current += lines * cinfo.image_width * cinfo.output_components;
			totallines += lines;
		}
		
		jpeg_finish_decompress(&cinfo);
		
		if (width)
			*width = cinfo.image_width;
		if (height)
			*height = cinfo.image_height;
		jpeg_destroy_decompress(&cinfo);
		
		try {
			Filesystem::close(imagefile);
		} catch (FileException &e) { }
		
		return image_data;
		
	} catch (Exception &e) {
		jpeg_destroy_decompress(&cinfo);
		delete image_data;
		THROW_EXCEPT(GraphicLoadException, e.getMessage());
	}
}

/*
	Error handling routines
*/

void jpegErrorExit(j_common_ptr cinfo)
{
	char msgbuffer[JMSG_LENGTH_MAX];
	cinfo->err->format_message(cinfo, msgbuffer);
	jpeg_destroy(cinfo);
	
	THROW_EXCEPT(GraphicLoadException, msgbuffer);
}

void jpegErrorEmitMessage(j_common_ptr cinfo, int msg_level)
{
	/*
	// Don't worry about warning messages
	if (msg_level < 0)
		cinfo->err->output_message(cinfo);
	*/
}

void jpegErrorOutputMessage(j_common_ptr cinfo)
{
	/*
	// No output for messages directly from here (handled at higher level
	// by throwing exceptions)
	char msgbuffer[JMSG_LENGTH_MAX];
	cinfo->err->format_message(cinfo, msgbuffer);
	LogFile::writeWarning(msgbuffer);
	*/
}

/*
	Data source manager routines
*/

void jpegCreateSource(j_decompress_ptr cinfo, Filesystem::File *infile)
{
	cinfo->src = (jpeg_source_mgr*)
			(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_IMAGE,
			sizeof(SourceManager));
	
	SourceManager *src = (SourceManager*)cinfo->src;
	src->mgr.init_source = jpegInitSource;
	src->mgr.fill_input_buffer = jpegFillInputBuffer;
	src->mgr.skip_input_data = jpegSkipInputData;
	src->mgr.resync_to_restart = jpeg_resync_to_restart; // Default
	src->mgr.term_source = jpegTermSource;
	
	src->buffer = (JOCTET*)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo,
			JPOOL_IMAGE, INPUT_BUF_SIZE * sizeof(JOCTET));
	src->file = infile;
	
	src->mgr.bytes_in_buffer = 0;    // Force fill_input_buffer on first read
	src->mgr.next_input_byte = NULL;
}

void jpegInitSource(j_decompress_ptr cinfo)
{
	SourceManager *src = (SourceManager*)cinfo->src;
	src->file_started = false;
}

boolean jpegFillInputBuffer(j_decompress_ptr cinfo)
{
	SourceManager *src = (SourceManager*)cinfo->src;
	int bytes = 0;
	try {
		bytes = Filesystem::read(src->file, src->buffer,
				INPUT_BUF_SIZE * sizeof(JOCTET));
	} catch (FileException &e) {
		ERREXIT(cinfo, JERR_FILE_READ);
	}
	
	if (bytes <= 0)
	{
		if (!src->file_started)
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		src->buffer[0] = 0xFF;
		src->buffer[1] = JPEG_EOI;
		bytes = 2;
	}
	
	src->mgr.next_input_byte = src->buffer;
	src->mgr.bytes_in_buffer = bytes;
	src->file_started = true;
	
	return TRUE;
}

void jpegSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
	SourceManager *src = (SourceManager*)cinfo->src;
	
	if (num_bytes > (long)src->mgr.bytes_in_buffer)
	{
		// Skipping bytes beyond the buffer
		num_bytes -= src->mgr.bytes_in_buffer;
		try {
			Filesystem::seek(src->file, Filesystem::tell(src->file) + num_bytes);
		} catch (FileException &e) {
			ERREXIT(cinfo, JERR_FILE_READ);
		}
		src->mgr.bytes_in_buffer = 0;
		src->mgr.next_input_byte = NULL;
	}
	else
	{
		// Skipping bytes only within the buffer
		src->mgr.bytes_in_buffer -= num_bytes;
		src->mgr.next_input_byte += num_bytes;
	}
}

void jpegTermSource(j_decompress_ptr cinfo)
{
	/* Don't need to do anything, since the Filesystem::File is handled
	   outside of libjpeg */
}
