#include "updateGD.h"

extern "C"
{
#include "../lowlevel/cdfs.h"
#include "../lowlevel/cdrom.h"
}

static uint8_t *ip_bin = (uint8_t *)0xAC008000;
static uint32_t *bin = (uint32_t *)0xAC010000;

UpdateGD::UpdateGD()
{
    status = -1;
    disc_type = -1;
    rv = -1;
    sz = 2048;
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
        gd_cdrom_get_status(&status, &disc_type);

        if (status == CD_STATUS_PAUSED ||
            status == CD_STATUS_STANDBY ||
            status == CD_STATUS_PLAYING)
        {
            break;
        }
    } while (1);

    if (disc_type == CD_GDROM)
    {
        gd_cdrom_read_toc(&toc, 1);
        int i;
        for (i = 0; i < 16; ++i)
        {
            sz = 2048;
            gd_read_sector(gd_find_datatrack(&toc) + i, (uint16_t *)(ip_bin + i * 2048), &sz);
        }
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

    char files[256][256];
    char filename[256];
    char *ext;
    file_t d;
    dirent_t *de;
    DIR *gd_dir;
    int i = 0;
    if (disc_type == CD_GDROM)
    {
        gd_dir = gd_opendir("/");
        if (!gd_dir)
        {
            printf("couldn't open the dir.\n");
            return 1;
        }
        printf("opened the dir\n");

        while ((de = gd_readdir(gd_dir)) && (i < 256))
        {
            memcpy(filename, de->name, 255);
            filename[255] = 0;
            printf("%s\n", filename);

            ext = filename + strlen(filename) - 4;
            if (!strcmp(ext, ".exe") || !strcmp(ext, ".bin") || !strcmp(ext, ".img"))
            {
                strcpy(files[i], filename);
                i++;
            }
        }

        gd_close(d);
        printf("%d files found.\n", i);
    }

    return ERR_OK;

    printf("Opening BINARY\n");
    FILE *fd;
    file_t f_bin;
    int cur = 0, rsz = 0;
    /* Read the binary in. This reads directly into the correct address. */
    fd = fopen(binary_name, "rb");
    printf("Reading BINARY [%s]\n", binary_name);
    if (!fd)
    {
        f_bin = fs_open(binary_name, O_RDONLY);
        if (f_bin)
        {

            fs_seek(f_bin, 0, SEEK_END);
            int lSize = fs_tell(f_bin);
            fs_seek(f_bin, 0, SEEK_SET);
            while ((rsz = fs_read(f_bin, bin + cur, 2048)) > 0)
            {
                cur += rsz;
            }
            fs_close(f_bin);
        }
    }
    else
    {
        /* Open the input bin file */
        assert(fd);

        /* obtain the bin file size using fseek */
        fseek(fd, 0, SEEK_END);
        int bin_size = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        printf("Size of Binary %d\n", bin_size);

        /* allocate the buffer, then read the file into it */
        //unsigned int * bin_buffer = malloc( lSize );
        assert(bin);
        fread(bin, 1, bin_size, fd);
        fclose(fd);
        printf("Read then Closed file!\n");
        fflush(stdout);
    }
    return ERR_OK;
}

void UpdateGD::run()
{
    runit();
}

void UpdateGD::run_alt()
{
    runit_kos(bin, bin_size);
}

void UpdateGD::read_disc_info()
{
    int i;

    /* Reinitialize the drive */
    //do
    //{
    rv = gdrom_reinit();
    //} while (rv != ERR_OK);

    do
    {
        gd_cdrom_get_status(&status, &disc_type);

        if (status == CD_STATUS_PAUSED ||
            status == CD_STATUS_STANDBY ||
            status == CD_STATUS_PLAYING)
        {
            break;
        }
    } while (1);

    if (disc_type == CD_GDROM)
    {
        gd_cdrom_read_toc(&toc, 1);
        int i;
        for (i = 0; i < 16; ++i)
        {
            sz = 2048;
            gd_read_sector(gd_find_datatrack(&toc) + i, (uint16_t *)(ip_bin + i * 2048), &sz);
        }
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
    memcpy(binary_name, (char *)secbuf + 0x60, 16);
    binary_name[16] = 0;
    /* Remove any spaces at the end of the binary_name */
    for (i = 0; i < 16; ++i)
    {
        if (binary_name[i] == ' ')
        {
            binary_name[i] = 0;
            break;
        }
    }
}