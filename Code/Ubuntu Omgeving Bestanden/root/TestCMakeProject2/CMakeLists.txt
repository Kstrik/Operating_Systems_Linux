cmake_minimum_required(VERSION 2.8)

project(TestCMakeProject)

add_library(apphellocmake2 STATIC hellocmake.c hellofunc.c)

install(TARGETS apphellocmake2 DESTINATION lib)
install(FILES include/hellofunc.h DESTINATION include)
