cmake_minimum_required(VERSION 3.16)
project(P2Pal VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Widgets Test)

set(SOURCES
    src/main.cpp
    src/p2pal.cpp
    src/network.cpp
    src/gossip.cpp
    src/message.cpp
    src/peer.cpp
    src/gui.cpp
)

set(HEADERS
    src/p2pal.h
    src/network.h
    src/gossip.h
    src/message.h
    src/peer.h
    src/gui.h
)

add_executable(P2Pal ${SOURCES} ${HEADERS})

target_link_libraries(P2Pal PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Widgets
)

# Tests
enable_testing()

add_executable(test_message tests/test_message.cpp src/message.cpp)
target_link_libraries(test_message PRIVATE Qt6::Core Qt6::Test)
add_test(NAME TestMessage COMMAND test_message)

add_executable(test_network tests/test_network.cpp src/network.cpp)
target_link_libraries(test_network PRIVATE Qt6::Core Qt6::Network Qt6::Test)
add_test(NAME TestNetwork COMMAND test_network)

add_executable(test_gossip tests/test_gossip.cpp src/gossip.cpp src/message.cpp)
target_link_libraries(test_gossip PRIVATE Qt6::Core Qt6::Network Qt6::Test)
add_test(NAME TestGossip COMMAND test_gossip)

add_executable(test_gui tests/test_gui.cpp src/gui.cpp)
target_link_libraries(test_gui PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Test)
add_test(NAME TestGui COMMAND test_gui)

# Create a script for launching multiple instances
file(WRITE ${CMAKE_BINARY_DIR}/run_instances.sh
"#!/bin/bash
./P2Pal --port 5000 &
./P2Pal --port 5001 &
./P2Pal --port 5002 &
./P2Pal --port 5003 &
echo \"Started 4 instances of P2Pal\"
")

file(CHMOD ${CMAKE_BINARY_DIR}/run_instances.sh
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)