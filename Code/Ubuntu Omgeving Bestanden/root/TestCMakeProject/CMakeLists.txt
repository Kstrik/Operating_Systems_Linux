cmake_minimum_required(VERSION 2.8)

project(TestCMakeProject)

add_executable(apphellocmake hellocmake.c)

install(TARGETS apphellocmake DESTINATION bin)
