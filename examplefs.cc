#include "examplefs.hh"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

ExampleFS* ExampleFS::_instance = NULL;

#define RETURN_ERRNO(x) (x) == 0 ? 0 : -errno

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

ExampleFS* ExampleFS::Instance() {
    if(_instance == NULL) {
        _instance = new ExampleFS();
    }
    return _instance;
}

ExampleFS::ExampleFS() {
    printf("ExampleFS::ExampleFS\n");
}

ExampleFS::~ExampleFS() {
    printf("ExampleFS::~ExampleFS\n");
}

void ExampleFS::AbsPath(char dest[PATH_MAX], const char *path) {
    printf("translated path: %s to %s\n", path, dest);
    strcpy(dest, _root);
    strncat(dest, path, PATH_MAX);
}

void ExampleFS::setRootDir(const char *path) {
    printf("setting FS root to: %s\n", path);
    _root = path;
}

int ExampleFS::Getattr(const char *path, struct stat *statbuf) {
    printf("ExampleFS::Getattr - called with: %s\n", path);
    int res = 0;

    memset(statbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        statbuf->st_mode = S_IFDIR | 0755;
        statbuf->st_nlink = 2;
        printf("ExampleFS::Getattr return %s\n", "/");
    } else if (strcmp(path, hello_path) == 0) {
        statbuf->st_mode = S_IFREG | 0444;
        statbuf->st_nlink = 1;
        statbuf->st_size = strlen(hello_str);
        printf("ExampleFS::Getattr return %s\n", hello_str);
    } else {
        res = -ENOENT;
        printf("ExampleFS::Getattr return %s\n", "error!");
    }

    printf("ExampleFS::Getattr return value of %i\n", res);
    return res;
}

int ExampleFS::Open(const char *path, struct fuse_file_info *fileInfo) {
    printf("ExampleFS::Open\n");

    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fileInfo->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

int ExampleFS::Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    printf("ExampleFS::Read\n");
    size_t len;
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;

    len = strlen(hello_str);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, hello_str + offset, size);
    } else
        size = 0;

    return size;
}

int ExampleFS::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    printf("ExampleFS::Readdir\n");
    (void) offset;
    (void) fileInfo;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, hello_path + 1, NULL, 0);

    return 0;
}


int ExampleFS::Init(struct fuse_conn_info *conn) {
    printf("ExampleFS::Init\n");
    return 0;
}

int ExampleFS::Getxattr(const char *path, const char *name, char *value, size_t size) {
        printf("getxattr(path=%s, name=%s, size=%d\n", path, name, (int)size);
        char fullPath[PATH_MAX];
        AbsPath(fullPath, path);
        return RETURN_ERRNO(getxattr(fullPath, name, value, size));
}

int ExampleFS::Opendir(const char *path, struct fuse_file_info *fileInfo) {
        printf("opendir(path=%s)\n", path);

        char fullPath[PATH_MAX];
        AbsPath(fullPath, path);
	printf("1\n");
        DIR *dir = opendir(fullPath);
	printf("2\n");
        fileInfo->fh = (uint64_t)dir;
	printf("3\n");
        return NULL == dir ? -errno : 0;
}
