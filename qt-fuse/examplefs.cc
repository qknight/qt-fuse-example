#include "examplefs.hh"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <QEventLoop>
#include "MyGlobalSingleton.hh"

#include "ColorCodes.hh"
#include "MyWrapper.hh"

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
    printf(GREEN "ExampleFS::ExampleFS" RESET "\n");
}

ExampleFS::~ExampleFS() {
    printf(GREEN "ExampleFS::~ExampleFS" RESET "\n");
}

void ExampleFS::AbsPath(char dest[PATH_MAX], const char *path) {
    printf(GREEN "translated path: %s to %s" RESET "\n", path, dest);
    strcpy(dest, _root);
    strncat(dest, path, PATH_MAX);
}

void ExampleFS::setRootDir(const char *path) {
    printf(GREEN "setting FS root to: %s" RESET "\n", path);
    _root = path;
}

int ExampleFS::Getattr(const char *path, struct stat *statbuf) {
    printf(GREEN "ExampleFS::Getattr - called with: %s" RESET "\n", path);
    int res = 0;

    memset(statbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        statbuf->st_mode = S_IFDIR | 0755;
        statbuf->st_nlink = 2;
    } else if (strcmp(path, hello_path) == 0) {
        statbuf->st_mode = S_IFREG | 0444;
        statbuf->st_nlink = 1;
        statbuf->st_size = strlen(hello_str);
    } else {
        res = -ENOENT;
    }

    return res;
}

int ExampleFS::Open(const char *path, struct fuse_file_info *fileInfo) {
    printf(GREEN "ExampleFS::Open %s" RESET "\n", path);

    if (strcmp(path, hello_path) != 0)
        return -ENOENT;

    if ((fileInfo->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

// http://qt-project.org/wiki/ThreadsEventsQObjects#7494f2b4836907fc1c09311e3a0305e6
int ExampleFS::Read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info */*fileInfo*/) {
    printf(GREEN "ExampleFS::Read" RESET "\n");

    /////////////////////////// code to split sync code into signals+slots code //////////////////////////
    // FIXME this code might still be broken, see http://stackoverflow.com/questions/19426900/pthread-2-signals-and-slots-wrapper-mit-qeventloop
    //       as MyGlobalSingleton::Instance() might be living in a different QThread (QCoreApplication)
    MyWrapper myW;
    QEventLoop loop;

    QObject::connect(&myW, SIGNAL(request(QString)), MyGlobalSingleton::Instance(), SLOT(processRequest(QString)), Qt::QueuedConnection);
    QObject::connect(MyGlobalSingleton::Instance(), SIGNAL(reply(QString)), &myW, SLOT(receiveReply(QString)));
    QObject::connect(&myW, SIGNAL(finished()), &loop, SLOT(quit()));

    myW.sendRequest("foobar");
    loop.exec();
    /* reply has finished, use it */

    Q_ASSERT(QThread::currentThread() == MyGlobalSingleton::Instance()->thread());
    
    qDebug() << __FUNCTION__ << " result: " << myW.result();
    /////////////////////////// code to split sync code into signals+slots code //////////////////////////

    size_t len;
    if(strcmp(path, hello_path) != 0)
        return -ENOENT;

    len = strlen(hello_str);
    if (offset < (off_t) len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, hello_str + offset, size);
    } else {
        size = 0;
    }

    return size;
}

/** Store data from an open file in a buffer
*
* Similar to the read() method, but data is stored and
* returned in a generic buffer.
*
* No actual copying of data has to take place, the source
* file descriptor may simply be stored in the buffer for
* later data transfer.
*
* The buffer must be allocated dynamically and stored at the
* location pointed to by bufp.  If the buffer contains memory
* regions, they too must be allocated using malloc().  The
* allocated memory will be freed by the caller.
*
* Introduced in version 2.9
*/
// Also so wie ich das verstehe:
// die geben dort nen Zeiger auf nen dynamischen Buffer rein dessen der dort in der Funktion erst mit "src" belegt wird.
// und src ist halt ein auch in dieser Funktion initialisierter struct von fuse.
// Und die versprechen halt, dass der Buffer dann dort existiert aber keine Daten kopiert wurden, wenn ichs richtig verstehe.
// Also quasi: Alle Flags und infos und offsets gesetzt aber keine Daten kopiert.
// Und wenn ich nich ganz falsch liege, dann liesst der quasi erst daten wenn du sie anfragst ... also eine etwas komische Art nen Buffer zu verwenden.
int ExampleFS::Read_buf(const char *path, struct fuse_bufvec **bufp, size_t size, off_t off, struct fuse_file_info *fileInfo) {
    printf(GREEN "ExampleFS::Read_buf, path=%s, size=%zu" RESET "\n", path, size);

    struct fuse_bufvec *src;

    (void) path;

    src = (fuse_bufvec*) malloc(sizeof(struct fuse_bufvec));
    if (src == NULL)
        return -ENOMEM;

    *src = FUSE_BUFVEC_INIT(size);

    int fd = open("QFuse", O_RDONLY);

    src->buf[0].flags = (fuse_buf_flags) (FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK);
    src->buf[0].fd = fd;// fileInfo->fh;
    src->buf[0].pos = off;

    *bufp = src;

    return 0;
}

int ExampleFS::Readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fileInfo) {
    printf(GREEN "ExampleFS::Readdir" RESET "\n");
    (void) offset;
    (void) fileInfo;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, hello_path + 1, NULL, 0);
    return 0;
}


void * ExampleFS::Init(struct fuse_conn_info *conn) {
    printf(GREEN "ExampleFS::Init" RESET "\n");
    return 0;
}

int ExampleFS::Getxattr(const char *path, const char *name, char *value, size_t size) {
    printf(GREEN "getxattr(path=%s, name=%s, size=%d" RESET "\n", path, name, (int)size);
    return 0;
}

int ExampleFS::Opendir(const char *path, struct fuse_file_info *fileInfo) {
    printf(GREEN "opendir(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Statfs(const char *path, struct statvfs *statInfo) {
    printf(GREEN "statfs(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Readlink(const char *path, char *link, size_t size) {
    printf(GREEN "readlink(path=%s, link=%s, size=%d)" RESET "\n", path, link, (int)size);
    return 0;
}

int ExampleFS::Mknod(const char *path, mode_t mode, dev_t dev) {
    printf(GREEN "mknod(path=%s, mode=%d)" RESET "\n", path, mode);
    return 0;
}

int ExampleFS::Mkdir(const char *path, mode_t mode) {
    printf(GREEN "**mkdir(path=%s, mode=%d)" RESET "\n", path, (int)mode);
    return 0;
}

int ExampleFS::Unlink(const char *path) {
    printf(GREEN "unlink(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Rmdir(const char *path) {
    printf(GREEN "rmkdir(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Symlink(const char *path, const char *link) {
    printf(GREEN "symlink(path=%s, link=%s)" RESET "\n", path, link);
    return 0;
}

int ExampleFS::Rename(const char *path, const char *newpath) {
    printf(GREEN "rename(path=%s, newPath=%s)" RESET "\n", path, newpath);
    return 0;
}

int ExampleFS::Link(const char *path, const char *newpath) {
    printf(GREEN "link(path=%s, newPath=%s)" RESET "\n", path, newpath);
    return 0;
}

int ExampleFS::Chmod(const char *path, mode_t mode) {
    printf(GREEN "chmod(path=%s, mode=%d)" RESET "\n", path, mode);
    return 0;
}

int ExampleFS::Chown(const char *path, uid_t uid, gid_t gid) {
    printf(GREEN "chown(path=%s, uid=%d, gid=%d)" RESET "\n", path, (int)uid, (int)gid);
    return 0;
}

int ExampleFS::Truncate(const char *path, off_t newSize) {
    printf(GREEN "truncate(path=%s, newSize=%d" RESET "\n", path, (int)newSize);
    return 0;
}

int ExampleFS::Utime(const char *path, struct utimbuf *ubuf) {
    printf(GREEN "utime(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    printf(GREEN "write(path=%s, size=%d, offset=%d)" RESET "\n", path, (int)size, (int)offset);
    return 0;
}

int ExampleFS::Flush(const char *path, struct fuse_file_info *fileInfo) {
    printf(GREEN "flush(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Release(const char *path, struct fuse_file_info *fileInfo) {
    printf(GREEN "release(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Fsync(const char *path, int datasync, struct fuse_file_info *fi) {
    printf(GREEN "fsync(path=%s, datasync=%d" RESET "\n", path, datasync);
    return 0;
}

int ExampleFS::Setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    printf(GREEN "setxattr(path=%s, name=%s, value=%s, size=%d, flags=%d)" RESET "\n",
           path, name, value, (int)size, flags);
    return 0;
}



int ExampleFS::Listxattr(const char *path, char *list, size_t size) {
    printf(GREEN "listxattr(path=%s, size=%d)" RESET "\n", path, (int)size);
    return 0;
}

int ExampleFS::Removexattr(const char *path, const char *name) {
    printf(GREEN "removexattry(path=%s, name=%s)" RESET "\n", path, name);
    return 0;
}

int ExampleFS::Releasedir(const char *path, struct fuse_file_info *fileInfo) {
    printf(GREEN "releasedir(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Fsyncdir(const char *path, int datasync, struct fuse_file_info *fileInfo) {
    printf(GREEN "fsyncdir(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Truncate(const char *path, off_t offset, struct fuse_file_info *fileInfo) {
    printf(GREEN "truncate(path=%s, offset=%d)" RESET "\n", path, (int)offset);
    return 0;
}

int ExampleFS::Access(const char *path, int mode) {
    printf(GREEN "access(path=%s)" RESET "\n", path);
    return 0;
}

int ExampleFS::Fgetattr(const char * path, struct stat * statbuf, struct fuse_file_info *fileInfo) {
    printf(GREEN "Fgetattr(path=%s)" RESET "\n", path);
    return 0;
}
