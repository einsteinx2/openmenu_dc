#ifndef __UPDATE_GD_H
#define __UPDATE_GD_H

#include <tsu/drawable.h>
extern "C"
{
#include "../gdemu_sdk.h"
}

class UpdateGD : public Drawable
{
public:
	UpdateGD();
	virtual ~UpdateGD();

	virtual void draw(int list);
	char *getTitle() { return str; };

	void next();
	void prev();

private:
	uint8_t secbuf[2048];
	char str[64];
	CDROM_TOC toc;

	int is_dc_img();
	int is_psx_img();
	void read_disc_info();
};

#endif /* __UPDATE_GD_H */