APP_NAME = SachesiCascades

CONFIG += qt warn_on cascades10 c++11
QT += network

include(config.pri)

DEFINES += BLACKBERRY
LIBS += -lbbsystem -lbbdata
QMAKE_CXX_FLAGS += -std=c++0x
QMAKE_C_FLAGS += -std=c++0x