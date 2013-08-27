#include "QFuse.hh"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QMetaObject>
#include <QTimer>
#include "ColorCodes.hh"
#include <iostream>

#include "wrap.hh"
#include <fuse/fuse_lowlevel.h>
#include <string.h>


static struct fuse_server {
    pthread_t pid;
    struct fuse *fuse;
    struct fuse_chan *ch;
    int failed;
    int running;
    char *mountpoint;
    int multithreaded;
    int foreground;
} fs;


//FIXME hacky to duplicated the struct declaration also found in fuse_kernel.h
struct fuse_in_header {
    uint32_t	len;
    uint32_t	opcode;
    uint64_t	unique;
    uint64_t	nodeid;
    uint32_t	uid;
    uint32_t	gid;
    uint32_t	pid;
    uint32_t	padding;
};


QFuse::QFuse(QObject* parent) : QObject(parent) {
    // init the fs struct
    memset(&fs, 0, sizeof(fs));
    fs.running = 1;

    // init fusefs_oper struct
    memset(&fusefs_oper, 0, sizeof(fusefs_oper));

    fusefs_oper.init = wrap_init;
    fusefs_oper.getattr = wrap_getattr;
    fusefs_oper.readlink = wrap_readlink;
    fusefs_oper.read = wrap_read;
    fusefs_oper.opendir = wrap_opendir;
    fusefs_oper.readdir = wrap_readdir;

    /*
    fusefs_oper.open = wrap_open;
    fusefs_oper.getdir = NULL;
    fusefs_oper.mknod = wrap_mknod;
    fusefs_oper.mkdir = wrap_mkdir;
    fusefs_oper.unlink = wrap_unlink;
    fusefs_oper.rmdir = wrap_rmdir;
    fusefs_oper.symlink = wrap_symlink;
    fusefs_oper.rename = wrap_rename;
    fusefs_oper.link = wrap_link;
    fusefs_oper.chmod = wrap_chmod;
    fusefs_oper.chown = wrap_chown;
    fusefs_oper.truncate = wrap_truncate;
    fusefs_oper.utime = wrap_utime;
    fusefs_oper.write = wrap_write;
    fusefs_oper.statfs = wrap_statfs;
    fusefs_oper.flush = wrap_flush;
    fusefs_oper.release = wrap_release;
    fusefs_oper.fsync = wrap_fsync;
    fusefs_oper.setxattr = wrap_setxattr;
    fusefs_oper.getxattr = wrap_getxattr;
    fusefs_oper.listxattr = wrap_listxattr;
    fusefs_oper.removexattr = wrap_removexattr;
    fusefs_oper.releasedir = wrap_releasedir;
    fusefs_oper.fsyncdir = wrap_fsyncdir;
    fusefs_oper.access = wrap_access;
    fusefs_oper.fgetattr = wrap_fgetattr;
    */
    socket = new QLocalSocket(this);

    connect(socket, SIGNAL(readyRead()),
            this, SLOT(readSocket()));
    connect(socket, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(displayError(QLocalSocket::LocalSocketError)));
}


QFuse::~QFuse() {
//     qDebug().nospace() << YELLOW << __FUNCTION__ << RESET;
    delete in;
    delete socket;
}


void QFuse::doWork() {
    QStringList l = QCoreApplication::arguments();
    argc = l.size();
    argv = new char*[argc];
    unsigned index = 0;
    foreach (const QString s, l) {
        QByteArray b = QFile::encodeName(s);
        char* tmp = b.data();
        char* z = (char*) malloc(strlen(tmp)+1);
        memcpy(z, tmp, strlen(tmp)+1);
        argv[index] = z;
//         std::cout << "'" << BOLDYELLOW << argv[index] << RESET << "'" << std::endl;
        index++;
    }

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    int res;

    // parses command line options (mountpoint, -s, foreground(which is ignored) and -d as well as other fuse specific parameters)
    res = fuse_parse_cmdline(&args, &fs.mountpoint, &fs.multithreaded, &fs.foreground);
    if (res == -1) {
        std::cout << RED << __FUNCTION__ << " error on fuse_parse_cmdline" << RESET << std::endl;
        goto err_out;
    }

    std::cout << YELLOW << "mountpoint: " << fs.mountpoint << RESET << std::endl;

    set_rootdir(realpath(fs.mountpoint, NULL));

    //to remove leftovers from previous crashes
    fuse_unmount(fs.mountpoint, fs.ch);

    fs.ch = fuse_mount(fs.mountpoint, &args);
    if(!fs.ch) {
//         perror("fuse_mount");
        goto err_out;
    }

    qDebug().nospace() <<  YELLOW << "fuse_mount worked" << RESET;

    fs.fuse = fuse_new(fs.ch, &args, &fusefs_oper, sizeof(fusefs_oper), NULL);
    fuse_opt_free_args(&args);
    if(!fs.fuse) {
//         perror("fuse_new");
        goto err_unmount;
    }

    qDebug().nospace() << YELLOW << __FUNCTION__ << " fuse_new worked" << RESET;

    in = new QDataStream(socket);
    in->setVersion(QDataStream::Qt_4_6);

    //FIXME hacky as hell!
    socket->setSocketDescriptor(fuse_chan_fd(fuse_session_next_chan(fuse_get_session(fs.fuse), NULL)));

    return;

err_unmount:
    fuse_unmount(fs.mountpoint, fs.ch);
    qDebug().nospace() << RED << QString("fuse error: Unable to mount FUSE on directory %1").arg(fs.mountpoint).toStdString().c_str() << RESET;
err_out:
    qDebug().nospace() << RED << "err_out" << RESET;
    fs.running = 0;
    QMetaObject::invokeMethod(QCoreApplication::instance(), "aboutToQuit", Qt::QueuedConnection);

    return;
}


//FIXME also need to include
//                     - splice copy stuff
//                     - multithreaded
void QFuse::readSocket() {
//     qDebug() << BOLDCYAN << __FUNCTION__ << "bytes available: " << socket->bytesAvailable() << RESET;

    struct fuse_session *se = fuse_get_session(fs.fuse);
    struct fuse_chan *ch = fuse_session_next_chan(se, NULL);
    size_t bufsize = fuse_chan_bufsize(ch);

    fuse_buf fbuf;
    //FIXME make this static or alloc this only if != NULL once
    fbuf.mem = (char *) malloc(bufsize);
    if (!fbuf.mem) {
        fprintf(stderr, "fuse: failed to allocate read buffer\n");
        return;
    }
    fbuf.size = bufsize;

    //FIXME also need to include splice copy stuff
    //FIXME if either debug messages is uncommented, the program behaves differently and produces this error:
    //FIXME    fuse: copy from pipe: Illegal seek
    //FIXME interestingly this error condition can be inverted, so when QDataStream is declared/defined like this:
    //FIXME    QDataStream in(socket);
    //FIXME    in.setVersion(QDataStream::Qt_4_0);
    //FIXME both qDebug statements below have to be commented in order to _NOT_ produce the error

    qDebug() << BOLDCYAN << __FUNCTION__ << "sizeof(struct fuse_in_header): " <<  sizeof(struct fuse_in_header) << RESET;
//     qDebug() << BOLDCYAN << __FUNCTION__ << "bufsize: " << bufsize << RESET;

    // copied from fuse_loop.c
    while((unsigned) socket->bytesAvailable() >= sizeof(struct fuse_in_header)) {
        fbuf.size = socket->bytesAvailable();
        int res = in->readRawData((char*)fbuf.mem, bufsize);
        //FIXME needs better error handling!

        if ((size_t) res < sizeof(struct fuse_in_header)) {
            fprintf(stderr, "short read on fuse device\n");
            free(fbuf.mem);
            return; //return -EIO;
        } else {
//         qDebug() << BOLDCYAN << __FUNCTION__ << "res: " << res << RESET;
//             qDebug() << BOLDCYAN << __FUNCTION__ << QString("read %1 bytes from fd").arg(res).toStdString().c_str() << fuse_chan_fd(ch) << RESET;
            //FIXME should fuse_session_process_buf be used instead?
            fuse_session_process_buf(se, &fbuf, ch);
        }
    }
//     qDebug() << BOLDCYAN << __FUNCTION__ << "still socket->bytesAvailable(): " << socket->bytesAvailable() << RESET;
    free(fbuf.mem);
}


//FIXME is this code used??
void QFuse::displayError(QLocalSocket::LocalSocketError s) {
    //FIXME this code is hacky
    qDebug() << __FUNCTION__ << RED << "LocalSocketError: " << s << RESET;
}


int QFuse::shutDown() {
    // this must be called only once
    static int guard = 0;

    if (guard == 1) {
        qDebug().nospace() << YELLOW << __FUNCTION__ << " already called, won't be executed twice" << RESET;
        return 0;
    }

    guard=1;

    if (fs.running) {
        qDebug().nospace() << YELLOW << __FUNCTION__ << " fuse_session_exit" << RESET;
        fuse_session_exit (fuse_get_session(fs.fuse));

        qDebug().nospace() << YELLOW << __FUNCTION__ << " fuse_unmount()" << RESET;
        fuse_unmount(fs.mountpoint, fs.ch);

        qDebug().nospace() << YELLOW << __FUNCTION__ << " calling pthread_join()" << RESET;
        pthread_join(fs.pid, NULL);

        fs.fuse = NULL;
        memset(&fs, 0, sizeof(fs));
    }

    qDebug().nospace() << YELLOW << __FUNCTION__ << " emit sigShutDownComplete()" << RESET;
    emit sigShutDownComplete();

    return 0;
}







