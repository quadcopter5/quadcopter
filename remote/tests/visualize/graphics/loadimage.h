/*
	Philip Romano
	7/16/2013
	loadimage.h
	
	Simplified interfaces for loading images.
	
	File input is done through Filesystem.
	
	Currently supports PNG and JPEG formats:
		loadPNG(...)
		loadJPEG(...)
*/

#ifndef LOADJPEG_H
#define LOADJPEG_H

#include <string>

/**
	Returns an unsigned char array of image data, which is in RGBA format
	(4 bytes per pixel)
	
	The memory is allocated using new[].
	It is the responsibility of the user to delete[] the returned array!
	
	The width and height of the loaded image are stored in the given arguments.
	
	Throws GraphicLoadException if the operation fails. width and height are
	guaranteed to be unchanged if an exception occurs.
*/
unsigned char *loadPNG(const std::string &filename, int *width, int *height);

/**
	Returns an unsigned char array of image data, which is in RGB format
	(3 bytes per pixel)
	
	The memory is allocated using new[].
	It is the responsibility of the user to delete[] the returned array!
	
	The width and height of the loaded image are stored in the given arguments.
	
	Throws GraphicLoadException if the operation fails. width and height are
	guaranteed to be unchanged if an exception occurs.
*/
unsigned char *loadJPEG(const std::string &filename, int *width, int *height);

#endif
