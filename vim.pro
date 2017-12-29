TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../Vim/vi.cpp


QMAKE_CXXFLAGS += -O0



LIBS += -lboost_system-mt

INCLUDEPATH += /usr/local/Cellar/boost/1.64.0_1/include
LIBS += -L/usr/local/Cellar/boost/1.64.0_1/lib

QMAKE_CFLAGS += -std=c++14 -fpermissive
QMAKE_CXXFLAGS += -std=c++14 -pthread  -fpermissive
QMAKE_LFLAGS += -std=c++14  -fpermissive

DISTFILES += \
    ../Vim/vim-5ca46d1e8afdc0b30b25fdf8f69f868b33a16241.txt.1
