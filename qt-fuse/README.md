# todo
- remove doWork()
- REWRITE examplefs.cc::Read_buf

# what is this?
qt-fuse is a lightweight example for how to use FUSE and Qt together. 

NOTE: command line argument parsing in FUSE, using fuse_parse_cmdline(..) seems to require
      calling fuse_mount(..) and fuse_loop()/fuse_loop_mt() although that _can't_ be working since
      each such function, using perror(..), will print an error.
      since i have added more debug messages compared to hello.c/hello_ll.c, running 
      'qt-fuse --help' seems to be broken - which is NOT the case! 
      when most of my debug messages:
       - the colored ones
       - the perror(..) lines 
      are muted or removed, it works like in the hello.c or hello_ll.c example. 

# features
 * POSIX signal handling, so that CTRL+C from a shell can be used to shutdown the program gracefully
   - this is the primary reason why fuse_main(..) can't be used since we have to wiretapp the POSIX 
     signal handlers to make them available in the Qt context: 
        QFuse.cpp - QFuse::shutDown() is called when a POSIX signal arrives, doing a clean shutdown
 * FUSE runs in its own FUSE-eventloop (fuse_loop/fuse_loop_mt). either of the two loops, depending if '-s' is passed is
   started from a seperate QThread in QFuse.cpp, thus the Qt eventloop and the FUSE eventloop are isolated from each other
 * pressing CTRL+C will shutdown the FUSE-eventloop, unmounts the FS and cleanly exists qt-fuse
 * 'fusermount -u dst' also shutsdown the FUSE-eventloop and will also cleanly exit the qt-fuse
 * if 'dst' is still mounted, say after qt-fuse crashed, qt-fuse will unmount it automatically on program start
 * this implementation features the hello.c minimal read only FUSE FS coming with the fuse library source
   - there exists only one file called 'hello'
   - and it contains "Hello World!"

# dependencies
 * Qt 4.x
 * FUSE library 2.9.x

# how to compile
 * qmake
 * make
 
# how to use 

## start the software (from a shell)
  mkdir dst
  ./qt-fuse 

or

usage:
  ./qt-fuse --help  
example: 
  ./qt-fuse dst -d -s

## exit the daemon
on the shell you started qt-fuse from, hit:
 - hit CTRL+C 

or use:
 - kill `pidof qt-fuse`

both commands will shutdown qt-fuse gracefully.

in case it crashes, shoot the process with:
  kill -9 `pidof qt-fuse`

afterwards remove the mount:
  fusermount -u dst
 
# documentation
* http://harmattan-dev.nokia.com/docs/platform-api-reference/xml/daily-docs/libqt4/unix-signals.html
* https://code.google.com/p/fuse-examplefs/ 
* http://cinwell.wordpress.com/2012/02/07/how-fuse-works-lib-level/
 
# license
this codebase, respecting each subprojects license of course, is licensed - WTFPL – Do What the Fuck You Want to Public License -> http://www.wtfpl.net/

 this repository consists of 3 merged code bases:
  - Qt hello world example 
    - main.cpp
  - Qt posix signals example 
    - main.cpp
  - FUSE hello.c example 
    - examplefs.hh, examplefs.cc
  - FUSE example from https://code.google.com/p/fuse-examplefs/ 
    - wrap.cc, wrap.hh

the code was put together by me, joachim schiele <js@lastlog.de> and i hope you can use it for whatever you have to do.

# QA
- check that the qt-fuse client meets all FUSE QA requirements
 - [x] check for errors when doing fuse function calls
 - [x] check all printf/cout/qDebug lines
 - [x] check all perror lines -> deactivated
 - [x] check that all required functions are called on shutdown
 - [x] make clean; make (fix problematic warnings)
  
# thanks
thanks to:
 - cbreak#qt@irc.freenode.net for help to get the aboutToQuit stuff working
 - peppe#qt@irc.freenode.net for help with converting QCoreApplication::arguments() to argv
 
 
