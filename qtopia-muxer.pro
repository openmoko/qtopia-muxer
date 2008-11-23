TEMPLATE = app
QT = core

INCLUDEPATH += src/
DEPENDPATH  += src/


# Qtopia source code
HEADERS += \
    src/gsm0710_p.h \
    src/qgsm0710multiplexer.h \
    src/qserialiodevice.h \
    src/qserialiodevice_p.h \
    src/qserialiodevicemultiplexer.h \
    src/qserialport.h
SOURCES += \
    src/gsm0710.c \
    src/qgsm0710multiplexer.cpp \
    src/qserialiodevice.cpp \
    src/qserialiodevicemultiplexer.cpp \
    src/qserialport.cpp

# Wrapper for FSO
SOURCES += \
    src/main.cpp
