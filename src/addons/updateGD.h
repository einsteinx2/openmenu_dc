#ifndef __UPDATE_GD_H
#define __UPDATE_GD_H
#include <kos.h>
#include <tsu/drawable.h>

extern "C"
{
	#include "../gdemu_sdk.h"
	extern void runit(void);
	extern void runit_kos(uint32_t *bin, uint32_t bin_size);
}

class UpdateGD : public Drawable
{
  public:
	UpdateGD();
	virtual ~UpdateGD();

	virtual void draw(int list);
	char *getTitle() { return game_title; };
	char *getBinary() { return binary_name; };

	void next();
	void prev();

	int readIPtoMem();
	int readBinaryToMem();

	void run();
	void run_alt();

  private:
	uint8_t secbuf[2048];
	char game_title[64];
	char binary_name[21];
	CDROM_TOC toc;

	int is_dc_img();
	int is_psx_img();
	void read_disc_info();

	int status;
	int disc_type;
	int rv;
	int bin_size;
	uint32_t sz;
};

#endif /* __UPDATE_GD_H */