project(PathFinderCA)

cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(AMXConfig)
include(AddSAMPPlugin)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

add_definitions (-DPLUGIN_NAME="${PROJECT_NAME}")

add_samp_plugin(${PROJECT_NAME}
  main.cpp
  main.h
  controller.cpp
  controller.h
  path.cpp
  path.h
  thread.cpp
  thread.h
  mutex.cpp
  mutex.h
  data.h
  lib/Invoke/Invoke.cpp
  lib/Invoke/Invoke.h
  SDK/amxplugin.cpp
  SDK/amxplugin2.cpp
  SDK/plugin.h
  SDK/plugincommon.h
  SDK/amx/amx.h
  SDK/amx/amx2.h
  SDK/amx/getch.c
  SDK/amx/getch.h
  SDK/amx/sclinux.h
)