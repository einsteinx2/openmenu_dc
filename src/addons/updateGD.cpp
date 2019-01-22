#include "updateGD.h"

static uint8_t *ip_bin = (uint8_t *)0xAC008000;
static uint32_t *bin = (uint32_t *)0xAC010000;

UpdateGD::UpdateGD()
{
    status = -1;
    disc_type = -1;
    rv = -1;
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

#define ERR_OK 0 /**< \brief No error */

int UpdateGD::readIPtoMem()
{
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
        cdrom_read_sectors((void *)ip_bin, 45150, 16);
    }
    else
    {
        cdrom_read_toc(&toc, 0);
        cdrom_read_sectors((void *)ip_bin, cdrom_locate_data_track(&toc), 16);
    }
    dcache_flush_range((uint32_t)&ip_bin, 32768);
    return ERR_OK;
}
int UpdateGD::readBinaryToMem()
{
    printf("Opening BINARY\n");
    FILE* fd;
    int cur = 0, rsz = 0;
    /* Read the binary in. This reads directly into the correct address. */
    fd = fopen("/cd/2ST_READ.BIN", "rb");
    printf("Reading BINARY\n");
    /* Open the input bin file */
    assert( fd);

    /* obtain the bin file size using fseek */
    fseek ( fd , 0, SEEK_END );
    int bin_size = ftell   ( fd );
    fseek ( fd , 0, SEEK_SET );
    printf("Size of Binary %d\n", bin_size);

    /* allocate the buffer, then read the file into it */
    //unsigned int * bin_buffer = malloc( lSize );
    assert( bin );
    fread ( bin, 1, bin_size, fd );
    fclose(fd);
    printf("Read then Closed file!\n");
    fflush(stdout);
    return ERR_OK;
}

void UpdateGD::run(){
    runit();
}

void UpdateGD::run_alt(){
    runit_kos(bin, bin_size);
}

int UpdateGD::find_gdrom_data_track()
{
    /*
    This file is part of Sylverant PSO Patcher
    Copyright (C) 2011, 2012, 2013 Lawrence Sebald
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as
    published by  the Free Software Foundation.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
    int i, first, last;

    first = TOC_TRACK(toc.first);
    last = TOC_TRACK(toc.last);

    if (first < 1 || last > 99 || first > last)
    {
        /* Guess that its the first High Density area track... */
        return 45150;
    }

    for (i = first; i <= last; ++i)
    {
        if (TOC_CTRL(toc.entry[i - 1]) == 4)
        {
            return TOC_LBA(toc.entry[i - 1]);
        }
    }

    /* Punt. */
    return 45150;
}

void UpdateGD::read_disc_info()
{
    int i;

    /* Reinitialize the drive */
    do
    {
        rv = cdrom_reinit();
    } while (rv != ERR_OK);

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
        cdrom_read_sectors((void *)secbuf, find_gdrom_data_track(), 1);
    }
    else
    {
        cdrom_read_toc(&toc, 0);
        cdrom_read_sectors((void *)secbuf, cdrom_locate_data_track(&toc), 1);
    }

    if (is_dc_img())
    {
        memcpy(game_title, secbuf + 128, 32);
        game_title[32] = '\0';
    }
    else
    {
        cdrom_read_sectors((void *)secbuf, 166, 1);

        if (is_psx_img())
        {
            memcpy(game_title, secbuf + 40, 32);
            game_title[32] = '\0';
            printf("[PSX] ");
        }
        else
        {
            strcpy(game_title, "Unknown disc");
        }
    }
    /* Figure out what the boot file is called */
    sprintf(binary_name, "%s", "/cd/");
    memcpy(binary_name + 4, (char *)secbuf + 0x60, 16);
    binary_name[16 + 4] = 0;
    /* Remove any spaces at the end of the binary_name */
    for (i = 0; i < 16; ++i)
    {
        if (binary_name[i + 4] == ' ')
        {
            binary_name[i + 4] = 0;
            break;
        }
    }
}