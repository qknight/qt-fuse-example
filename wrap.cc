#include "wrap.hh"
#include "examplefs.hh"

#include <QDebug>

void set_rootdir(const char *path) {
    printf("%s\n", __FUNCTION__);
    ExampleFS::Instance()->setRootDir(path);
}

int wrap_getattr(const char *path, struct stat *statbuf) {
    printf("%s\n", __FUNCTION__);
    return ExampleFS::Instance()->Getattr(path, statbuf);
}
int wrap_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    printf("%s\n", __FUNCTION__);
    return ExampleFS::Instance()->Readdir(path, buf, filler, offset, fileInfo);
}
int wrap_open(const char *path, struct fuse_file_info *fileInfo) {
    printf("%s\n", __FUNCTION__);
    return ExampleFS::Instance()->Open(path, fileInfo);
}
int wrap_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    printf("%s\n", __FUNCTION__);
    return ExampleFS::Instance()->Read(path, buf, size, offset, fileInfo);
}
int wrap_init(struct fuse_conn_info *conn) {
    printf("%s\n", __FUNCTION__);
    return ExampleFS::Instance()->Init(conn);
}

int wrap_getxattr(const char *path, const char *name, char *value, size_t size) {
    printf("%s\n", __FUNCTION__);
    return ExampleFS::Instance()->Getxattr(path, name, value, size);
}

int wrap_opendir(const char *path, struct fuse_file_info *fileInfo) {
    printf("%s\n", __FUNCTION__);
    return ExampleFS::Instance()->Opendir(path, fileInfo);
}
