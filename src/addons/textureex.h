/*      
   Tsunami for KallistiOS ##version##
   Added support for PVR loading
        
   textureex.h

   (c)2002 Dan Potter
   (c)2019 Hayden kow

*/

#ifndef __TSUNAMI_TEXTUREEX_H
#define __TSUNAMI_TEXTUREEX_H
#include <tsu/texture.h>
#include <plx/texture.h>

class TextureEx : public Texture {
public:
	TextureEx(char *fn);
    TextureEx();
	virtual ~TextureEx();

	// Load this texture from a file (if it hasn't been done already)
	bool loadPVRFromFile(char *fn);
};

#endif	/* __TSUNAMI_TEXTUREEX_H */
