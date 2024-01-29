cmake_minimum_required(VERSION 3.5)

project(Experiments LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Experiments main.cpp
    myclass.cpp
    myclass.hpp)

include(GNUInstallDirs)
install(TARGETS Experiments
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
