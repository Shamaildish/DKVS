cmake_minimum_required(VERSION 3.17)
project(DKVS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories(.)

add_executable(DKVS
        Client.cpp
        Client.h
        Connection.cpp
        Connection.h
        ConssistingHash.cpp
        ConssistingHash.h
        Defines
        Function.cpp
        Function.h
        Hash.cpp
        Hash.h
        LoadBalancer.cpp
        LoadBalancer.h
        main.cpp
        Server.cpp
        Server.h FileHandler.cpp FileHandler.h)
