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


static void *fuse_thread(void *arg)
{
    if(arg) {}

    if (fs.multithreaded) {
        std::cout << YELLOW << __FUNCTION__ << ": fuse_loop_mt()" << RESET << std::endl;
        if (fuse_loop_mt(fs.fuse) < 0) {
            perror("problem in fuse_loop_mt");
            fs.failed = 1;
        }
        std::cout << YELLOW << __FUNCTION__ << " exiting fuse_loop() now" << RESET << std::endl;
    } else {
        std::cout << YELLOW << __FUNCTION__ << ": fuse_loop()" << RESET << std::endl;
        if (fuse_loop(fs.fuse) < 0) {
            perror("problem in fuse_loop");
            fs.failed = 1;
        }
        std::cout << YELLOW << __FUNCTION__ << " exiting fuse_loop_mt() now" << RESET << std::endl;
    }

    // this call will shutdown the qcoreapplication via the qfuse object
    QMetaObject::invokeMethod(QCoreApplication::instance(), "aboutToQuit", Qt::QueuedConnection);

    return NULL;
}


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
}


QFuse::~QFuse() {
//     qDebug().nospace() << YELLOW << __FUNCTION__ << RESET;
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

    // registers the operations
    // calls either the single-threaded or the multi-threaded event loop
    // fuse_loop() or fuse_loop_mt()
    if(pthread_create(&fs.pid, NULL, fuse_thread, NULL) != 0) {
//         perror("pthread_create");
        goto err_unmount;
    }
    qDebug().nospace() << YELLOW << __FUNCTION__ << " fuse server up and running ;-)" << RESET;

    return;

err_unmount:
    fuse_unmount(fs.mountpoint, fs.ch);
    qDebug().nospace() << RED << QString("fuse error: Unable to mount FUSE on directory %1").arg(fs.mountpoint).toStdString().c_str() << RESET;
err_out:
    fs.running = 0;
    emit sigShutDownComplete();
    return;
}


int QFuse::shutDown() {
    if (!fs.running) {
//           qDebug().nospace() << YELLOW << __FUNCTION__ << " already called, won't be executed twice" << RESET;
        return 0;
    }
    fs.running = 0;

    qDebug().nospace() << YELLOW << __FUNCTION__ << " fuse_session_exit" << RESET;
    fuse_session_exit (fuse_get_session(fs.fuse));

    qDebug().nospace() << YELLOW << __FUNCTION__ << " fuse_unmount()" << RESET;
    fuse_unmount(fs.mountpoint, fs.ch);

    qDebug().nospace() << YELLOW << __FUNCTION__ << " calling pthread_join()" << RESET;
    pthread_join(fs.pid, NULL);

    fs.fuse = NULL;
    memset(&fs, 0, sizeof(fs));

    qDebug().nospace() << YELLOW << __FUNCTION__ << " emit sigShutDownComplete()" << RESET;
    emit sigShutDownComplete();

    return 0;
}








