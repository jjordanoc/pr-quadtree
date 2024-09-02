cmake_minimum_required(VERSION 3.26)
project(quadtree)

set(CMAKE_CXX_STANDARD 17)

#SET(CMAKE_CXX_FLAGS_DEBUG "-g")

add_executable(quadtree
        Particle.h
        QuadTree.h
        Point.h
        Rect.h
        main.cpp
        QuadTree.cpp
        Particle.cpp)
