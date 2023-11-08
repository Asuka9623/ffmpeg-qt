TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        audiooutput.cpp \
        avframequeue.cpp \
        avpacketqueue.cpp \
        decodethread.cpp \
        demuxthread.cpp \
        log.cpp \
        main.cpp


INCLUDEPATH += $$PWD/include

LIBS += $$PWD/lib/avcodec.lib  \
        $$PWD/lib/avdevice.lib  \
    $$PWD/lib/avfilter.lib  \
    $$PWD/lib/avformat.lib  \
    $$PWD/lib/avutil.lib  \
    $$PWD/lib/postproc.lib  \
    $$PWD/lib/swresample.lib  \
    $$PWD/lib/swscale.lib  \

HEADERS += \
    audiooutput.h \
    avframequeue.h \
    avpacketqueue.h \
    decodethread.h \
    demuxthread.h \
    log.h \
    queue.h \
    thread.h


INCLUDEPATH += $$PWD/SDL2/include

LIBS += -L$$PWD/SDL2/lib/lSDL2
LIBS += -L$$PWD/SDL2/lib/lSDL2main
LIBS += -L$$PWD/SDL2/lib/lSDL2test
