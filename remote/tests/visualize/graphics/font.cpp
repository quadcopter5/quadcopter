/*
	Philip Romano
	7/26/2013
	font.cpp
	
	Font class - loads fonts and facilitates rendering of text. A Font
		represents a single font face.
*/

#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "exception.h"
#include "graphics/image.h"
#include "graphics/font.h"
#include "geometry.h"

FT_Library Font::mFTLib;

void Font::initializeSystem()
{
	FT_Error error = FT_Init_FreeType(&mFTLib);
	if (error != 0)
		THROW_EXCEPT(FontException,
				"Failed to initialize FreeType, FreeType Error "
				+ boost::lexical_cast<std::string>(error));
}

void Font::closeSystem()
{
	FT_Done_FreeType(mFTLib);
}

Font::Font(const std::string &filename, unsigned int dpi)
{
	mFilename = filename;
	mDPI      = dpi;

	std::ifstream infile(mFilename.c_str(), std::ios::in | std::ios::binary);
	char buffer[4096];

	if (infile.fail())
		THROW_EXCEPT(FontException, "Could not open file '" + mFilename
				+ "' for reading");

	do {
		infile.read(buffer, 4096);
		mFileContents.insert(mFileContents.end(),
				buffer, buffer + infile.gcount());
	} while (!infile.eof());

	infile.close();

	if (mFileContents.size() == 0)
		THROW_EXCEPT(FontException, "File '" + mFilename
				+ "'cannot be opened as a font because it is empty");
	
	// Set up mFTFace to reference the loaded file in memory
	FT_Error error = FT_New_Memory_Face(mFTLib, &mFileContents[0],
			mFileContents.size(), 0, &mFTFace);
	if (error)
		THROW_EXCEPT(FontException, "Failed to create font face from \""
				+ mFilename + "\", FreeType Error "
				+ boost::lexical_cast<std::string>(error));
}

Font::~Font()
{
	FT_Done_Face(mFTFace);
}

void Font::loadSize(unsigned int ptsize)
{
	getFace(ptsize);
}

int Font::getLineHeight(unsigned int ptsize)
{
	FT_Error error = FT_Set_Char_Size(mFTFace, 0, ptsize << 6, mDPI, mDPI);
	if (error)
		THROW_EXCEPT(FontException, "Could not set font size, FreeType Error "
				+ boost::lexical_cast<std::string>(error));
	
	if (FT_IS_SCALABLE(mFTFace))
		return FT_MulFix(mFTFace->height, mFTFace->size->metrics.y_scale) >> 6;
	else
		return mFTFace->size->metrics.height >> 6;
}

int Font::getAscent(unsigned int ptsize)
{
	FT_Error error = FT_Set_Char_Size(mFTFace, 0, ptsize << 6, mDPI, mDPI);
	if (error)
		THROW_EXCEPT(FontException, "Could not set font size, FreeType Error "
				+ boost::lexical_cast<std::string>(error));
	
	return FT_MulFix(mFTFace->bbox.yMax, mFTFace->size->metrics.y_scale) >> 6;
}

Font::Glyph Font::getGlyph(unsigned int ptsize, char charcode)
{
	boost::shared_ptr<Font_Base> face = getFace(ptsize);
	return face->getGlyph(charcode);
}

Vector2<int> Font::getKerning(const Glyph &left, const Glyph &right)
{
	boost::shared_ptr<Font_Base> face = getFace(left.size);
	return face->getKerning(left.index, right.index);
}

//void Font::layoutText(const std::string &text, unsigned int ptsize,
//		int x, int y, HAlignment halign, VAlignment valign, float red,
//		float green, float blue, float alpha, unsigned int maxwidth)
//{
//	boost::shared_ptr<Font_Base> face = getFace(ptsize);
//	
//	int xpos = x, ypos = y;
//	FT_UInt indexprev = 0;
//	FT_Vector kern;
//	
//	for (std::string::const_iterator iter = text.begin();
//			iter != text.end();
//			++iter)
//	{
//		if (*iter == '\n')
//		{
//			xpos = x;
//			ypos += getLineHeight(ptsize);
//		}
//		else
//		{
//			Glyph glyph = face->getGlyph(*iter);
//			
//			kern.x = 0;
//			kern.y = 0;
//			if (indexprev != 0)
//				kern = face->getKerning(indexprev, glyph.index);
//			
//			boost::shared_ptr<Image> glyphimg(glyph.image);
//			if (glyphimg)
//			{
//				glyphimg->bindTexture();
//				glyphimg->renderColor(xpos + glyph.bearingx + kern.x,
//						ypos + glyph.bearingy + kern.y,
//						0, 0, 0, 0, alpha, red, green, blue);
//			}
//			xpos += glyph.advancex + kern.x;
//			ypos += glyph.advancey + kern.y;
//			indexprev = glyph.index;
//		}
//	}
//}

void Font::unloadSize(unsigned int ptsize)
{
	mFontSizes.erase(ptsize); // Won't throw (less<unsigned int> doesn't throw)
}

std::string Font::getFilename()
{
	return mFilename;
}

/*
	Font_Base class
*/

Font::Font_Base::Font_Base(FT_Face face, unsigned int size, unsigned int dpi)
{
	mFTFace = face;
	mSize = size << 6;
	mDPI = dpi;
	
	FT_Error error = FT_Set_Char_Size(mFTFace, 0, mSize, mDPI, mDPI);
	if (error)
		THROW_EXCEPT(FontException,
				"Could not set font size, FreeType Error "
				+ boost::lexical_cast<std::string>(error));
	
	// Load/render the standard glyphs
	char code = 32;     // ASCII Dec 32  == ' '
	while (code <= 126) // ASCII Dec 126 == '~'
	{
		getGlyph(code);
		++code;
	}
}

Font::Glyph Font::Font_Base::getGlyph(char charcode)
{
	std::map< char,Glyph >::iterator iter = mGlyphs.find(charcode);
	if (iter == mGlyphs.end())
	{
		// Glyph does not yet exist in map. Render and insert it
		FT_Error error;
		
		error = FT_Set_Char_Size(mFTFace, 0, mSize, mDPI, mDPI);
		if (error)
			THROW_EXCEPT(FontException, "Could not set font size, FreeType Error "
					+ boost::lexical_cast<std::string>(error));
		
		FT_UInt index = FT_Get_Char_Index(mFTFace, charcode);
		if (index == 0)
			THROW_EXCEPT(FontException,
					"Could not get glyph index, FreeType Error "
					+ boost::lexical_cast<std::string>(error));
		
		error = FT_Load_Glyph(mFTFace, index, FT_LOAD_RENDER);
		if (error)
			THROW_EXCEPT(FontException,
					"Could not render glyph, FreeType Error "
					+ boost::lexical_cast<std::string>(error));
		
		// Make rendered glyph into an Image
		FT_GlyphSlot ftglyph = mFTFace->glyph;
		FT_Bitmap bmp = ftglyph->bitmap;
		Glyph glyph;
		
		if (bmp.buffer)
		{
			unsigned char img[bmp.rows][bmp.width * 4];
			unsigned char *ftcurrent = bmp.buffer;
			
			memset(img, 255, bmp.rows * bmp.width * 4); // Initialize img to white
			for (int r = 0; r < bmp.rows; ++r)
			{
				for (int c = 0; c < bmp.width; ++c)
					img[r][c * 4 + 3] = ftcurrent[c]; // Copy into alpha channel
				ftcurrent += bmp.pitch;
			}
			
			try {
				glyph.image = boost::shared_ptr<Image>(new Image(
						*img, bmp.width, bmp.rows, 4, false));
			} catch (ImageException &e) {
				THROW_EXCEPT(FontException,
						"Could not create Image from font glyph: " + e.getMessage());
			}
		}
		// else, glyph.image is a null shared_ptr by default construction
		
		glyph.index = index;
		glyph.size  = mSize >> 6;
		glyph.bearingx = ftglyph->bitmap_left;
		glyph.bearingy = -ftglyph->bitmap_top;
		glyph.advancex = ftglyph->advance.x >> 6; // The value returned by FT
		glyph.advancey = ftglyph->advance.y >> 6; // is 26.6 fixed-point
		
		// Insert into map
		mGlyphs.insert(std::pair<char,Glyph>(charcode, glyph));
		
		// Return the Glyph
		return glyph;
	}
	else
		return iter->second;
}

Vector2<int> Font::Font_Base::getKerning(FT_UInt left, FT_UInt right)
{
	FT_Error error = FT_Set_Char_Size(mFTFace, 0, mSize, mDPI, mDPI);
	if (error)
		THROW_EXCEPT(FontException, "Could not set font size, FreeType Error "
				+ boost::lexical_cast<std::string>(error));
	
	FT_Vector k;
	error = FT_Get_Kerning(mFTFace, left, right, FT_KERNING_DEFAULT, &k);
	if (error)
		THROW_EXCEPT(FontException, "Could not get font kerning, FreeType Error "
				+ boost::lexical_cast<std::string>(error));
	
	Vector2<int> kern;
	kern.x = k.x >> 6;
	kern.y = k.y >> 6;
	return kern;
}

/**
	Private member functions (class Font)
*/

boost::shared_ptr<Font::Font_Base> Font::getFace(unsigned int size)
{
	std::map< unsigned int,boost::shared_ptr<Font_Base> >::iterator iter
			= mFontSizes.find(size);
	if (iter == mFontSizes.end())
	{
		boost::shared_ptr<Font_Base> face(new Font_Base(mFTFace, size, mDPI));
		return mFontSizes.insert(
				std::pair< unsigned int,boost::shared_ptr<Font_Base> >(
				size, face)).first->second; // iterator -> shared_ptr<Font_Base>
	}
	else
		return iter->second;
}
