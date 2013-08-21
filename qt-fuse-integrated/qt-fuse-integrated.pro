TEMPLATE = app
TARGET = 
INCLUDEPATH += .
QT += network
DEFINES += "FUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64"
LIBS += -lfuse 
DEPENDPATH += .
QMAKE_CXXFLAGS += -Wall -fpermissive

# Input
SOURCES += main.cpp QFuse.cpp wrap.cc MyDaemon.cpp examplefs.cc
HEADERS += QFuse.hh wrap.hh MyDaemon.hh examplefs.hh
