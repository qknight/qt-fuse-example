#ifndef wrap_hh
#define wrap_hh

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>

#ifdef __cplusplus
extern "C" {
#endif

void set_rootdir(const char *path);

int wrap_getattr(const char *path, struct stat *statbuf);
int wrap_open(const char *path, struct fuse_file_info *fileInfo);
int wrap_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
int wrap_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo);
int wrap_init(struct fuse_conn_info *conn);
int wrap_getxattr(const char *path, const char *name, char *value, size_t size);
int wrap_opendir(const char *path, struct fuse_file_info *fileInfo);

#ifdef __cplusplus
}
#endif

#endif //wrap_hh


