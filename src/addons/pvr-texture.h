#ifndef PVR_TEXTURE_H
#define PVR_TEXTURE_H
#include <kos.h>

/* Load a PVR texture - located in pvr-texture.c */
pvr_ptr_t TextureLoadPVR(char *fname, unsigned char isMipMapped, unsigned char glMipMap);
#endif /* PVR_TEXTURE_H */
