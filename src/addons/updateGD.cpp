#include "updateGD.h"

extern "C"
{
#include "../lowlevel/cdfs.h"
#include "../lowlevel/cdrom.h"
}

static uint8_t *ip_bin = (uint8_t *)0xAC008000;
static uint32_t *bin = (uint32_t *)0xAC010000;
static uint32_t *bin_8 = (uint32_t *)0x8C010000;

UpdateGD::UpdateGD()
{
    status = -1;
    disc_type = -1;
    rv = -1;
    sz = 408;
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
    uint32_t i = 1;
    if (disc_type == CD_GDROM)
    {
        printf("Read IP.BIN from GDROM\n");
        for (i = 0; i < 16; ++i)
        {
            sz = 2048;
            gd_read_sector(gd_find_datatrack(&toc) + i, (uint16_t *)(ip_bin + i * 2048), &sz);
        }
    }
    else
    {
        printf("Loading IP.BIN from CDI (Homebrew/Piracy)!\n");
        cdrom_read_toc(&toc, 0);
        cdrom_read_sectors((void *)ip_bin, cdrom_locate_data_track(&toc), 16);
    }
    dcache_flush_range((uint32_t)&ip_bin, 32768);
    fflush(stdout);
    return ERR_OK;
}
int UpdateGD::readBinaryToMem()
{
    FILE *fd;
    int f_bin;
    printf("Reading BINARY [%s]....\n", binary_name);
    switch (disc_type)
    {
    case CD_GDROM:
        printf("GD-ROM Routine\n");
        break;
    case CD_CDROM:
        printf("CD-R Routine\n");
        break;
    case CD_CDROM_XA:
        printf("CD-R-XA Routine\n");
        break;
    case CD_CDI:
        printf("CD-CDI Routine\n");
        break;
    default:
        printf("NO CLUE WHAT\n");
        break;
    }

    if (disc_type == CD_GDROM)
    {
        int cur = 0, rsz = 0;
        /* Read the binary in. This reads directly into the correct address. */
        if ((f_bin = gd_open(binary_name, O_RDONLY)) < 0)
        {
            printf("ERROR opening gd_open()!\n");
            fflush(stdout);
            return -1;
        }
        while ((rsz = gd_read(f_bin, bin + cur, 2048)) > 0)
        {
            cur += rsz;
#ifdef DEBUG
            printf("Total Read bytes %d\n", cur);
            fflush(stdout);
#endif
        }
        gd_close(f_bin);
        printf("Size of Binary %d\n", cur);
        bin_size = cur;
        return ERR_OK;
    }
    else
    {
        printf("Trying to load CD-R\n");
        char cd_bin_name[21];
        sprintf(cd_bin_name, "/cd/%s", binary_name);
        /* Open the input bin file */
        fd = fopen(cd_bin_name, "rb");
        assert(fd);

        /* obtain the bin file size using fseek */
        fseek(fd, 0, SEEK_END);
        int bin_size = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        printf("Size of Binary %d\n", bin_size);

        /* allocate the buffer, then read the file into it */
        //unsigned int * bin_buffer = malloc( lSize );
        //assert(bin);
        fread(bin, 1, bin_size, fd);
        fclose(fd);
        fflush(stdout);
        return ERR_OK;
    }
    printf("WE GOT HERE! BADDDDDDDDDDDD\n");
    /* Shouldn't get here */
    return -1;
}

void UpdateGD::run()
{
    runit();
}

void UpdateGD::run_alt()
{
    runit_kos(bin_8, bin_size);
}

void UpdateGD::read_disc_info()
{
    int i;
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

    /* Reinitialize the drive */
    /*do
    {
        rv = gdrom_reinit();
    } while (rv != ERR_OK);*/
    gd_cdrom_init();

    gd_cdrom_get_status(&status, &disc_type);

    printf("Disc checking: ");
    if (disc_type == CD_GDROM)
    {
        printf("Found GDROM!\n");
        gd_gd_read_toc((uint16_t *)&toc, &sz);
        sz = 2048;
        gd_read_sector(gd_find_datatrack(&toc), (uint16_t *)(secbuf), &sz);
        memcpy((void *)ip_bin, secbuf, sz);
    }
    else
    {
        printf("Found CD-R (Piracy/Homebrew)\n");
        cdrom_read_toc(&toc, 0);
        cdrom_read_sectors((void *)secbuf, cdrom_locate_data_track(&toc), 1);
    }

    if (is_dc_img())
    {
        memcpy(game_title, secbuf + 128, 32);
        game_title[32] = '\0';
        /* trim game title */
        char *p = game_title;
        int l = strlen(p);

        while (isspace(p[l - 1]))
            p[--l] = 0;
        while (*p && isspace(*p))
            ++p, --l;

        memmove(game_title, p, l + 1);
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

    /* setup binary name */
    memcpy(binary_name, (char *)secbuf + 0x60, 16);
    binary_name[15] = '\0';
    /* Remove any spaces at the end of the binary_name */
    for (i = 0; i < 16; ++i)
    {
        if (binary_name[i] == ' ')
        {
            binary_name[i] = '\0';
            break;
        }
    }
}