#ifndef PVR_TEXTURE_LOAD_H
#define PVR_TEXTURE_LOAD_H
#include <kos.h>
#include <dc/pvr.h>
#include <plx/texture.h>

/* Load a PVR texture - located in pvr-texture.c */
plx_texture_t *TextureLoadPVR(char *fname);
#endif /* PVR_TEXTURE_LOAD_H */
