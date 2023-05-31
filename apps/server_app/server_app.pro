QT       += core gui
QT		 += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
#QMAKE_LFLAGS += -Wl,--no-as-needed
#QMAKE_CXXFLAGS += _GLIBCXX_USE_CXX11_ABI=0
#INCLUDEPATH += /home/kerosene/Downloads/torch/libtorch/include/
INCLUDEPATH += /home/kerosene/Downloads/onnxruntime-linux-x64-1.14.1/include
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    messageprocessor.cpp \
    myclassifier.cpp \
    myservermain.cpp

HEADERS += \
    messageheaders.h \
    messageprocessor.h \
    myclassifier.h \
    myservermain.h

FORMS += \
    myservermain.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
unix {
#LIBS += -L/home/kerosene/Downloads/torch/libtorch/lib  -ltorch_cpu -ltorch  -lc10   \
#        -ltorch \
#        -ltorch_global_deps
LIBS += -L/home/kerosene/Downloads/onnxruntime-linux-x64-1.14.1/lib -lonnxruntime
}
