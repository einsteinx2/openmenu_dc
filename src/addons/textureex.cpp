/*
   Tsunami for KallistiOS ##version##
   Added support for PVR loading
        
   textureex.h

   (c)2002 Dan Potter
   (c)2019 Hayden kow
*/

#include "textureex.h"

extern "C" {
    #include "pvr-texture.h"
}

TextureEx::TextureEx(char *fn)
{
	m_txr = NULL;
	if (!loadPVRFromFile(fn))
		assert(false);
}

TextureEx::TextureEx()
{
	m_txr = NULL;
}

TextureEx::~TextureEx()
{
	if (m_txr != NULL)
		plx_txr_destroy(m_txr);
}

bool TextureEx::loadPVRFromFile(char *fn)
{
	m_txr = TextureLoadPVR(fn);
	if (m_txr == NULL)
	{
		printf("------    Texture::loadFromFile: Can't load '%s'\n", fn);
		return false;
	}
	else
		return true;
}
