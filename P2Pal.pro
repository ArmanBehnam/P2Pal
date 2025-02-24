QT += core gui network widgets testlib

TARGET = P2Pal
TEMPLATE = app

SOURCES += src/main.cpp \
           src/p2pal.cpp \
           src/network.cpp \
           src/gossip.cpp \
           src/message.cpp \
           src/peer.cpp \
           src/gui.cpp

HEADERS += src/p2pal.h \
           src/network.h \
           src/gossip.h \
           src/message.h \
           src/peer.h \
           src/gui.h

TESTS += tests/test_message.cpp \
         tests/test_gossip.cpp \
         tests/test_network.cpp \
         tests/test_gui.cpp

CONFIG += c++17