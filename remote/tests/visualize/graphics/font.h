/*
	Philip Romano
	7/26/2013
	font.h
	
	Font class - loads fonts and facilitates rendering of text. A Font
		represents a single font face.
	
	Before using any Fonts, the static function Font::initializeSystem()
	must be called. This initializes FreeType.
	
	This class only utilizes the first face specified in a font file, so when
	designing fonts it is best to divide different styles into separate files.
*/

#ifndef FONT_H
#define FONT_H

#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "exception.h"
#include "geometry.h"

// Forward class declarations
class Image; // "graphics/image.h"

// Exceptions thrown by Font
class FontException : public Exception {
	public: FontException(const std::string &msg, const std::string &file,
					int line) : Exception(msg, file, line) { }
};

class Font
{
	public:
		/**
			Initialize the font system, i.e. FreeType.
			
			This must be called prior to using anything else in the Font class!
			
			Throws FontException if the initialization fails.
		*/
		static void initializeSystem();
		
		/**
			Close the font system, i.e. FreeType.
			
			This should be called when Fonts are no longer needed, such as when
			freeing up the program before exiting. Make sure that individual Font
			objects have already been freed before calling this.
			
			Does not throw Exceptions.
		*/
		static void closeSystem();
		
		/**
			Glyph holds a single rendered glyph, along with relevant rendering
			information.
		*/
		struct Glyph
		{
			boost::shared_ptr<Image> image;
			unsigned int index,
			             size;  // The size of this Glyph in integer points (no
			                    // fractional parts)
			
			// Units for advancex, advancey, bearingx, and bearingy are all
			// integer pixels
			int advancex,
			    advancey;
			
			// The following values are calculated such that they should
			// be used to position the *Image*, whose origin is the top-left
			// corner, whereas a font glyph's origin is the bottom-left corner.
			// Using these values will align the glyphs to the baseline.
			int bearingx, // Amount to shift horizontally to position glyph
			              // relative to Image origin. Positive is right
			    bearingy; // Amount to shift vertically to position glyph
			              // relative to Image origin, so it rests on the
			              // baseline. Positive is down
		};
		
		/**
			Constructor
			Loads a font from the given filename and gets it ready for use.
			
			The Font object will use the supplied DPI (dots per inch) for the
			remainder of its lifetime. If you need to change the DPI setting
			during runtime, you will need to create a new Font object. Typical
			setting for computer monitors is 96 DPI.
			
			Throws FontException if the loading fails (usually file not
			found).
		*/
		Font(const std::string &filename, unsigned int dpi = 96);
		
		~Font();
		
		/**
			Get a specific font size ready for rendering. This should be used
			so that glyph rendering does not have to occur in real time.
			
			Throws FontException if the loading fails.
		*/
		void loadSize(unsigned int ptsize);
		
		/**
			Returns the font-recommended line height for the given size, in
			pixels.
			
			Throws FontException if the request fails.
		*/
		int getLineHeight(unsigned int ptsize);
		
		/**
			Returns the height of the font in the specified size. The "height" in
			this case is considered the distance from the baseline to the top of
			the tallest glyph.
			
			Throws FontException if the request fails.
		*/
		int getAscent(unsigned int ptsize);
		
		/**
			Returns the Glyph for the specified character code and size.
			
			Note that when using the Glyph Image, make sure to first check that
			the Image is not null; a font might not specify a glyph for a certain
			character code but define metrics for that code (in particular, ' ')
			
			If the size has not yet been loaded (e.g. by using loadSize()), then
			this function will load the size specified.
			
			Throws FontException if the Glyph cannot be loaded, or if the creation
			of a new size fails.
		*/
		Glyph getGlyph(unsigned int ptsize, char charcode);
		
		/**
			Get the kerning between a pair of Glyphs in pixels. Note that the two
			Glyphs provided MUST both be from the same Font through which this
			function is called.
			
			The size from the left Glyph is used (though it is expected that both
			Glyphs are also from the same Font size).
			
			Throws FontException if the request fails. 
		*/
		Vector2<int> getKerning(const Glyph &left, const Glyph &right);
		
		/**
			Provided a string of text, a font size, a color, and a maximum width,
			
		*/
//		void layoutText(const std::string &text,
//				unsigned int ptsize,
//				int x,
//				int y,
//				HAlignment halign = HALIGN_LEFT,
//				VAlignment valign = VALIGN_BASELINE,
//				float red = 1.0f,
//				float green = 1.0f,
//				float blue = 1.0f,
//				float alpha = 1.0f,
//				unsigned int maxwidth = 0);
		
		/**
			Remove the specified size from the cache. Use this function to free
			up memory when a size is no longer needed, but other sizes within the
			Font are still needed.
			
			Doesn't throw exceptions.
		*/
		void unloadSize(unsigned int ptsize);
		
		/**
			Returns the filename used to load this Font.
		*/
		std::string getFilename();
		
	private:
		/*
			Font_Base holds a single font size and actually handles the
			interface with FreeType.
		*/
		class Font_Base
		{
			public:
				/**
					Constructor
					
					FT_Face must already be set up. size is the font size to use
					in whole integer points. DPI will be used throughout the
					object's lifetime.
					
					Throws FontException if the initialization fails.
				*/
				Font_Base(FT_Face face,
						unsigned int size,
						unsigned int dpi);
				
				/**
					Return the struct Glyph for the given character code in UTF-8
					(ASCII).
					
					If the Glyph does not yet exist in the map cache, then it is
					rendered, added to the cache, and returned.
					
					Throws FontException if creation of the glyph fails (if it
					didn't already exist).
				*/
				Glyph getGlyph(char charcode);
				
				/**
					Return an FT_Vector containing the kerning values for the pair
					such that left is the glyph index of the left glyph and right is
					the glyph index of the right glyph.
					
					Throws FontException if the request fails.
				*/
				Vector2<int> getKerning(FT_UInt left, FT_UInt right);
				
			private:
				FT_Face      mFTFace;
				FT_F26Dot6   mSize;   // The size in 26.6 fixed-point
				unsigned int mDPI;
				
				// Glyph cache to avoid re-rendering
				std::map<char,Glyph> mGlyphs;
		};
		
		/**
			Private auxiliary function to return a shared_ptr to the existing
			element, or if non-existent insert a new Font_Base and return a
			shared_ptr to the new element.
		*/
		boost::shared_ptr<Font_Base> getFace(unsigned int size);
		
		static FT_Library mFTLib; // Shared across all Fonts
		
		std::string mFilename;
		std::vector<unsigned char> mFileContents;
		FT_Face mFTFace;      // Shared among all sizes within this Font
		unsigned int mDPI;
		std::map< unsigned int,boost::shared_ptr<Font_Base> > mFontSizes;
};

#endif
