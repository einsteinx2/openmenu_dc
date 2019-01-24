/*
   pvr-texture.c
   (c) 2019 hayden Kowalchuk

   Load A PVR Texture to the PVR
*/
#include "pvr-texture.h"

/** Texture structure */
/*typedef struct plx_texture {
	pvr_ptr_t	ptr;
	int		w;
	int		h;
	int		fmt; < PVR texture format (e.g., PVR_TXRFMT_ARGB4444)

	pvr_poly_cxt_t	cxt_opaque,
			cxt_trans,
			cxt_pt;
	pvr_poly_hdr_t	hdr_opaque,
			hdr_trans,
			hdr_pt;
} plx_texture_t;*/

extern void fill_contexts(plx_texture_t *txr);

#define PVR_HDR_SIZE 0x20
/* Load a PVR texture file into memory, and then bind the texture to Parallax */
plx_texture_t *TextureLoadPVR(char *fname)
{
    /* create a new plx texture */
    plx_texture_t *txr;
    txr = malloc(sizeof(plx_texture_t));
    pvr_ptr_t rv;

    FILE *tex = NULL;
    unsigned char *texBuf;
    unsigned int texSize;

    tex = fopen(fname, "rb");

    if (tex == NULL)
    {
        printf("------    Failed to load image file: %s\n", fname);
        return NULL;
    }

    fseek(tex, 0, SEEK_END);
    texSize = ftell(tex);

    texBuf = (unsigned char *)malloc(texSize);
    fseek(tex, 0, SEEK_SET);
    fread(texBuf, 1, texSize, tex);
    fclose(tex);

    int texW = texBuf[PVR_HDR_SIZE - 4] | texBuf[PVR_HDR_SIZE - 3] << 8;
    int texH = texBuf[PVR_HDR_SIZE - 2] | texBuf[PVR_HDR_SIZE - 1] << 8;
    int texFormat, texColor;
    int Bpp = 2; // in Bytes

    switch ((unsigned int)texBuf[PVR_HDR_SIZE - 8])
    {
    case 0x00:
        texColor = PVR_TXRFMT_ARGB1555;
        Bpp = 2;
        break; //(bilevel translucent alpha 0,255)

    case 0x01:
        texColor = PVR_TXRFMT_RGB565;
        Bpp = 2;
        break; //(non translucent RGB565 )

    case 0x02:
        texColor = PVR_TXRFMT_ARGB4444;
        Bpp = 2;
        break; //(translucent alpha 0-255)

    case 0x03:
        texColor = PVR_TXRFMT_YUV422;
        Bpp = 1;
        break; //(non translucent UYVY )

    case 0x04:
        texColor = PVR_TXRFMT_BUMP;
        Bpp = 2;
        break; //(special bump-mapping format)

    case 0x05:
        texColor = PVR_TXRFMT_PAL4BPP;
        Bpp = 1;
        break; //(4-bit palleted texture)

    case 0x06:
        texColor = PVR_TXRFMT_PAL8BPP;
        Bpp = 1;
        break; //(8-bit palleted texture)

    default:
        texColor = PVR_TXRFMT_RGB565;
        Bpp = 2;
        break;
    }

    switch ((unsigned int)texBuf[PVR_HDR_SIZE - 7])
    {
    case 0x01:
        texFormat = PVR_TXRFMT_TWIDDLED;
        break; //SQUARE TWIDDLED

    case 0x03:
        texFormat = PVR_TXRFMT_VQ_ENABLE;
        break; //VQ TWIDDLED

    case 0x09:
        texFormat = PVR_TXRFMT_NONTWIDDLED;
        break; //RECTANGLE

    case 0x0B:
        texFormat = PVR_TXRFMT_STRIDE | PVR_TXRFMT_NONTWIDDLED;
        break; //RECTANGULAR STRIDE

    case 0x0D:
        texFormat = PVR_TXRFMT_TWIDDLED;
        break; //RECTANGULAR TWIDDLED

    case 0x10:
        texFormat = PVR_TXRFMT_VQ_ENABLE | PVR_TXRFMT_NONTWIDDLED;
        break; //SMALL VQ

    default:
        texFormat = PVR_TXRFMT_NONE;
        break;
    }

    if (!(rv = pvr_mem_malloc(texW * texH * Bpp)))
    {
        printf("------    Couldn't allocate memory for texture!\n");
        free(texBuf);
        return NULL;
    }
    pvr_txr_load(texBuf + PVR_HDR_SIZE, rv, texW * texH * Bpp);

    free(texBuf);

    /* Fill our texture */
    txr->w = texW;
    txr->h = texH;
    txr->fmt = texFormat | texColor;
    txr->ptr = rv;
    pvr_poly_cxt_txr(&txr->cxt_opaque, PVR_LIST_OP_POLY, txr->fmt, txr->w, txr->h, txr->ptr, PVR_FILTER_BILINEAR);
    pvr_poly_cxt_txr(&txr->cxt_trans, PVR_LIST_TR_POLY, txr->fmt, txr->w, txr->h, txr->ptr, PVR_FILTER_BILINEAR);
    pvr_poly_cxt_txr(&txr->cxt_pt, PVR_LIST_PT_POLY, txr->fmt, txr->w, txr->h, txr->ptr, PVR_FILTER_BILINEAR);

    plx_txr_flush_hdrs(txr);

    return txr;
}
