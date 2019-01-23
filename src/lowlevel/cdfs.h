 #ifndef DC_GDFS_H_
 #define DC_GDFS_H_
 #include <kos.h>
#include "cdrom.h"
#include "gd.h"
/*
 * Error codes are negative
 */
#define ERR_SYSERR   -1    /* Generic error from GDROM system       */
#define ERR_DIRERR   -2    /* Root directory not found              */
#define ERR_NOFILE   -3    /* File not found                        */
#define ERR_PARAM    -4    /* Invalid parameters passed to function */
#define ERR_NUMFILES -5    /* Max number of open files exceeded     */
#define ERR_NODISK   -6    /* No disc present                       */
#define ERR_DISKCHG  -7    /* Disc has been replaced with a new one */

/* 
 *  An open directory
 */
typedef struct {
  /* All members are private! */
  int     dd_fd;
  int     dd_loc;
  int     dd_size;
  char    *dd_buf;
} DIR;

#define MAX_OPEN_FILES 8

/*
 *  Flags for argument 3 to lseek
 */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* Prototypes */
int gd_open(const char *path, int oflag);
int gd_close(int fd);
int gd_pread(int fd, void *buf, unsigned int nbyte, unsigned int offset);
int gd_read(int fd, void *buf, unsigned int nbyte);
long int gd_lseek(int fd, long int offset, int whence);
DIR *gd_opendir(const char *dirname);
int gd_closedir(DIR *dirp);
int gd_readdir_r(DIR *dirp, struct kos_dirent *entry, struct kos_dirent **res);
struct kos_dirent *gd_readdir(DIR *dirp);
void gd_cdfs_init(void);
unsigned int gd_find_datatrack(CDROM_TOC *toc);

#endif /* DC_GDFS_H_ */
