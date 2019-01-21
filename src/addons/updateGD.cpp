#include "updateGD.h"

UpdateGD::UpdateGD()
{
}
UpdateGD::~UpdateGD()
{
}

void UpdateGD::draw(int list)
{
}

void UpdateGD::next()
{
    gdemu_img_cmd(GDEMU_NEXT_IMG);
    read_disc_info();
}

void UpdateGD::prev()
{
    gdemu_img_cmd(GDEMU_PREV_IMG);
    read_disc_info();
}

int UpdateGD::is_dc_img()
{
    if (!strncmp((char *)secbuf, "SEGA SEGAKATANA SEGA ENTERPRISES", 32))
    {
        return 1;
    }

    return 0;
}

int UpdateGD::is_psx_img()
{
    if (!strncmp((char *)secbuf + 8, "PLAYSTATION", 11))
    {
        return 1;
    }

    return 0;
}

void UpdateGD::read_disc_info()
{
    int status = -1, disc_type = -1;

    cdrom_reinit();

    do
    {
        cdrom_get_status(&status, &disc_type);

        if (status == CD_STATUS_PAUSED ||
            status == CD_STATUS_STANDBY ||
            status == CD_STATUS_PLAYING)
        {
            break;
        }
    } while (1);

    if (disc_type == CD_GDROM)
    {
        cdrom_read_toc(&toc, 1);
        cdrom_read_sectors((void *)secbuf, 45150, 1);
    }
    else
    {
        cdrom_read_toc(&toc, 0);
        cdrom_read_sectors((void *)secbuf, cdrom_locate_data_track(&toc), 1);
    }

    if (is_dc_img())
    {
        memcpy(str, secbuf + 128, 32);
        str[32] = '\0';
    }
    else
    {
        cdrom_read_sectors((void *)secbuf, 166, 1);

        if (is_psx_img())
        {
            memcpy(str, secbuf + 40, 32);
            str[32] = '\0';
            printf("[PSX] ");
        }
        else
        {
            strcpy(str, "Unknown disc");
        }
    }
}