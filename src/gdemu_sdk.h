//Provided by megavolt85
#ifndef GDEMU_SDK_H
#define GDEMU_SDK_H
#include <kos.h>

/* return 8 byte: 00 00 09 01 00 00 14 05
 * 01 09 00 00 - internal bootloader version. don't used in early models
 * 05 14 00 00 - FW version (5.14.0)
*/
int gdemu_get_version(void *buffer, uint32_t *size);

/* param = 0x55 next img */
/* param = 0x44 prev img */
#define GDEMU_NEXT_IMG 0x55
#define GDEMU_PREV_IMG 0x44
int gdemu_img_cmd(uint8_t cmd);

int gdemu_set_img_num(uint8_t img_num_lb, uint8_t img_num_hb);
#endif /* GDEMU_SDK_H */