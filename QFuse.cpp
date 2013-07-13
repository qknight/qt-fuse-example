// parts of this code were adapted from fuse_server.c; a very good example on how
// to use the fuse api ;-) -> thanks to Mike Shal <marfey@gmail.com>

#include "QFuse.hh"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDir>
#include <QDebug>


#include "wrap.hh"

// FIXME don't make this hardcoded
#define TUP_MNT "dst"

// FIXME don't make this parameters hardcoded
int argc = 2;
char *argv[] = {"qfuse", "-d"};


static struct fuse_server {
    pthread_t pid;
    struct fuse *fuse;
    struct fuse_chan *ch;
    int failed;
} fs;


static void *fuse_thread(void *arg)
{
    if(arg) {}

    qDebug() << "creating fuse_loop() now";
// translated path: / to dst/
// getattr(dst/)
// fuse: error creating thread: Resource temporarily unavailable

    if(fuse_loop(fs.fuse) < 0) {
        perror("fuse_loop");
        fs.failed = 1;
    }
    qDebug() << "exiting fuse_loop() now";

    fuse_destroy(fs.fuse);
    return NULL;
}


QFuse::QFuse(QObject* parent) : QObject(parent) {
    fusefs_oper.getattr = wrap_getattr;
    fusefs_oper.open = wrap_open;
    fusefs_oper.read = wrap_read;
    fusefs_oper.readdir = wrap_readdir;
    fusefs_oper.init = wrap_init;
    fusefs_oper.opendir = wrap_opendir;
    fusefs_oper.getxattr = wrap_getxattr;
}

int QFuse::doWork() {
    set_rootdir(realpath(TUP_MNT, NULL));

    /* Need a garbage arg first to count as the process name */
//     if(fuse_opt_add_arg(&args, "nix-fuse") < 0) {
//         qDebug() << "error adding garbage arg";
//         return -1;
//     }

    // parses command line options (-d -s and -h)
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);


    fs.ch = fuse_mount(TUP_MNT, &args);
    if(!fs.ch) {
        perror("fuse_mount");
        goto err_out;
    }

    qDebug() << "fuse_mount worked";

    // installs signal handlers for INT, HUP, TERM and PIPE
    // FIXME SIGINT redirect coming from MyDaemon.cpp (made in main.cpp) should
    // also inform the fuse thread about what is going on


    // registers an exit handler to unmount the filesystem on program exit
    // creates a fuse handle

    fs.fuse = fuse_new(fs.ch, &args, &fusefs_oper, sizeof(fusefs_oper), NULL);
    fuse_opt_free_args(&args);
    if(!fs.fuse) {
        perror("fuse_new");
        goto err_unmount;
    }

    qDebug() << "fuse_new worked";

    // registers the operations
    // calls either the single-threaded or the multi-threaded event loop
    // fuse_loop() or fuse_loop_mt()
    if(pthread_create(&fs.pid, NULL, fuse_thread, NULL) != 0) {
        perror("pthread_create");
        goto err_unmount;
    }

    qDebug() << "fuse server up and running ;-)";

    // FIXME this code is here to keep the thread alive, just to make sure that
    // the variables used for the FUSE backend are not cleared
    while(1)
        sleep(1);

    return 0;

err_unmount:
    fuse_unmount(TUP_MNT, fs.ch);
err_out:
    fprintf(stderr, "tup error: Unable to mount FUSE on %s\n", TUP_MNT);
    return -1;
}


int QFuse::shutDown() {
    // FIXME i know that this code can't be called right now and i don't care
    // since i have other more important segfault problems at the moment
    qDebug() << __FUNCTION__;

    fuse_unmount(TUP_MNT, fs.ch);

//     fuse_session_exit(fs.fuse);
//     fuse_exit(fs.fuse);
//     fuse_destroy(fs.fuse);

    qDebug() << __FUNCTION__ << " calling pthread_join()";
    pthread_join(fs.pid, NULL);
    qDebug() << __FUNCTION__ << " pthread_joined(;-)";

    fs.fuse = NULL;
    memset(&fs, 0, sizeof(fs));

    emit sigShutDownComplete();

    return 0;
}





