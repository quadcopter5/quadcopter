/*
	Philip Romano
	7/23/2013
	image.h
	
	Image class - facilitates loading and working with images.
*/

#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <GL/gl.h>

#include "exception.h"

// Exceptions thrown by Image
class ImageException : public Exception {
	public: ImageException(const std::string &msg, const std::string &file,
					int line) : Exception(msg, file, line) { }
};

class Image
{
	public:
		/**
			Constructor
			Loads the image contained in filename.
			Image is loaded into graphics memory for use by OpenGL.
			
			If keepdata is true, the image data is kept in software memory after
			loading. Otherwise, the software memory is freed.
			
			Data must be kept in software memory if you wish to make a subimage.
			
			Throws GraphicLoadException if the load fails.
		*/
//		Image(const std::string &filename, bool keepdata = false);
		
		/**
			Constructor
			Copies the given image data into an OpenGL texture.
			
			If keepdata is true, the given data is copied into the new Image's
			data.
			
			Throws GraphicLoadException if the image creation fails.
		*/
		Image(const unsigned char *data,
				int width,
				int height,
				int numchannels,
				bool keepdata = false);
		
		~Image();
		
		/**
			Binds (activates) this Image in the OpenGL context. This must be
			called before any render*() call in order to tell OpenGL to use this
			Image.
		*/
		void bindTexture();
		
		/**
			render()
			
			Render the image to the screen, using all options (position, size,
			   origin, subimage, angle, and color).
			
			If a problem occurs during the rendering process, then a
			   GraphicException is thrown and the rendering is canceled.
			
			Arguments:
				x         - X position to render onto screen
				y         - Y position to render onto screen
				width     - On-screen width
				               A value <= 0 maintains 1:1 pixel ratio
				height    - On-screen height
				               A value <= 0 maintains 1:1 pixel ratio
				originx   - The X position of the origin about which drawing is centered
				originy   - The Y position of the origin about which drawing is centered
								   The image is rotated about this position on the image/subimage,
				               and the final image is translated such that this pixel is at
				               the position provided by the first two arguments (x, y)
				
				               If a subimage is taken, then this coordinate will be
				               relative to the top-left corner of the subimage
				               (instead of top-left corner of the original image)
				
				               Scaling due to the arguments width and height is also applied
				               here; origin is based on native scaling of image.
				subx      - X position of top-left corner of subimage
				suby      - Y position of top-left corner of subimage
				subwidth  - Width of subimage
				subheight - Height of subimage
				               If width or height <= 0, or results in a rectangle
				               that extends outside bounds of the image, that
				               dimension is set to the maximum possible value
				               within the bounds of the image.
				angle	    - Angle to rotate the image
				               Angle is in degrees CCW, with 0 degrees
				               pointing east; 360 degrees in one rotation
				alpha     - Factor to multiply with alpha channel when drawing
				red       - Factor to multiply with red channel when drawing
				green     - Factor to multiply with green channel when drawing
				blue      - Factor to multiply with blue channel when drawing
				               A value of 1.0f will retain the original value
				               for the channel. A value of 1.0f for all channels
				               will draw the image without any color modifications.
				bind      - If true, glBindTexture() will be called for the OpenGL
									texture for this image. Set this to false if the
									texture is already known to be bound.
		*/
		void render(int x,
				int y,
				int width = 0,
				int height = 0,
				int originx = 0,
				int originy = 0,
				int subx = 0,
				int suby = 0,
				int subwidth = 0,
				int subheight = 0,
				float angle = 0.0f,
				float alpha = 1.0f,
				float red = 1.0f,
				float green = 1.0f,
				float blue = 1.0f);
		
		/**
			renderColor()
			
			Render the image to the screen, using basic options (position, size,
			   origin, and color).
			
			If a problem occurs during the rendering process, then a
			   GraphicException is thrown and the rendering is canceled.
			
			Arguments:
				x         - X position to render onto screen
				y         - Y position to render onto screen
				width     - On-screen width
				               A value <= 0 maintains 1:1 pixel ratio
				height    - On-screen height
				               A value <= 0 maintains 1:1 pixel ratio
				alpha     - Factor to multiply with alpha channel when drawing
				red       - Factor to multiply with red channel when drawing
				green     - Factor to multiply with green channel when drawing
				blue      - Factor to multiply with blue channel when drawing
				               A value of 1.0f will retain the original value
				               for the channel. A value of 1.0f for all channels
				               will draw the image without any color modifications.
				bind      - If true, glBindTexture() will be called for the OpenGL
									texture for this image. Set this to false if the
									texture is already known to be bound.
		*/
		void renderColor(int x,
				int y,
				int width = 0,
				int height = 0,
				int originx = 0,
				int originy = 0,
				float alpha = 1.0f,
				float red = 1.0f,
				float green = 1.0f,
				float blue = 1.0f);
		
		/**
			renderTiled()
			
			Render the image to screen, tiling in specified directions.
			
			Arguments:
				x      - x-coordinate of the top-left corner of the box to tile into
				y      - y-coordinate of the top-left corner of the box to tile into
				width  - width of the rectangle to tile into
							   A value <= 0 uses the image's width
				height - height of the rectangle to tile into
				            A value <= 0 uses the image's height
				tilex  - If true, image will be tiled in the x direction
				            Otherwise, it will be stretched in this direction
				tiley  - If true, image will be tiled in the y direction
				alpha  - Factor to multiply with alpha channel when drawing
				red    - Factor to multiply with red channel when drawing
				green  - Factor to multiply with green channel when drawing
				blue   - Factor to multiply with blue channel when drawing
				            A value of 1.0f will retain the original value
				            for the channel. A value of 1.0f for all channels
				            will draw the image without any color modifications.
				bind   - If true, glBindTexture() will be called for the OpenGL
				            texture for this image. Set this to false if the
				            texture is already known to be bound.
		*/
		void renderTiled(int x,
				int y,
				int width,
				int height,
				bool tilex = true,
				bool tiley = true,
				float alpha = 1.0f,
				float red = 1.0f,
				float green = 1.0f,
				float blue = 1.0f);
		
		/**
			Returns the GL texture ID for this Image.
		*/
		GLuint getID();
		
		/**
			Returns the filename that was used to load this Image.
		*/
		std::string getFilename();
		
		/**
			Get the dimensions of this Image.
		*/
		int getWidth();
		int getHeight();
		
		/**
			If data is still in software memory, the copy of the image in
			local software memory is freed. The copy of the image in the
			graphics card still exists.
		*/
		void freeData();
		
		/**
			Returns a subimage of this image starting at position:
			   (x, y) and with specified width and height
			
			The object returned is a new Image, so the object must be deleted
			   by the user.
			
			Returns 0 if the specified dimensions are invalid, or the data has
			   already been freed with freeData(). Dimensions are invalid if they
			   include any area outside the image or would result in an image
			   with no area (width or height == 0)
			
			If keepsubdata is true, the newly created Image will retain its
			   image data in memory.
		*/
		Image *subImage(int x, int y, int width, int height, bool keepsubdata = false);
		
	private:
		GLuint        mID;
		std::string   mFilename;
		unsigned char *mImageData;
		int           mNumChannels,
		              mWidth,
		              mHeight,
		              mPOTWidth,
		              mPOTHeight;
		
		// The width and height of the actual image in texels,
		// 	because the original image is only a subimage of the
		// 	OpenGL texture
		float mTexelWidth,
		      mTexelHeight;
		
		/**
			Creates an image that has power-of-two width and height, using
			   provided data. The new power-of-two width and height
			   are stored into *potwidth and *potheight. The new image may
			   be rectangular.
			The given image is placed in the top left corner of the new POT image,
			   and the image is padded with transparent black (0000)
			The arguments width and height specify the dimensions of the given
			   image data.
			
			Returns:
			   On success, a pointer to the newly allocated data array
			   0 if there is an error
		*/
		unsigned char *makePOTImage(const unsigned char *data, int width, int height, int *potwidth, int *potheight);
};

#endif

