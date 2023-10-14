cmake_minimum_required(VERSION 3.27)

add_executable(main main.cpp)

find_package(SFMF 2.6.0 COMPONENTS graphic REQUIRED)

target_link_libraries(main PUBLIC sfml-graphic)