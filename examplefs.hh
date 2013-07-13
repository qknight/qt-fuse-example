#ifndef examples_hh
#define examples_hh

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>

class ExampleFS {
private:
    const char *_root;
    static ExampleFS *_instance;
    void AbsPath(char dest[PATH_MAX], const char *path);

public:
    static ExampleFS *Instance();

    ExampleFS();
    ~ExampleFS();

    void setRootDir(const char *path);

    int Getattr(const char *path, struct stat *statbuf);
    int Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo);
    int Open(const char *path, struct fuse_file_info *fileInfo);
    int Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo);
    int Init(struct fuse_conn_info *conn);
    int Getxattr(const char *path, const char *name, char *value, size_t size);
    int Opendir(const char *path, struct fuse_file_info *fileInfo);

};

#endif //examples_hh

