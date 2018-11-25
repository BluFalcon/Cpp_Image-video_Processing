#-------------------------------------------------
#
# Project created by QtCreator 2015-06-12T03:21:26
#
#-------------------------------------------------


#//g++ -Wall openCV_example.cpp -o camcpp `pkg-config --cflags --libs opencv` -L/usr/local/cuda-6.5/lib64


QT

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = my_focus_try
TEMPLATE = app


CONFIG += link_pkgconfig
PKGCONFIG += opencv

SOURCES += main.cpp \
    la_function.cpp

LIBS += -L"/usr/local/cuda-6.5/lib64"


LIBS += -L"/usr/src/flycapture/lib" -lflycapture${D}


INCLUDEPATH += /usr/include/flycapture/

HEADERS += \
    la_function.h \
