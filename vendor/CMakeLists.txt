project(dialog)

cmake_minimum_required(VERSION 2.8)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(AMXConfig)
include(AddSAMPPlugin)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/
  ${CMAKE_CURRENT_SOURCE_DIR}/SDK
  ${CMAKE_CURRENT_SOURCE_DIR}/SDK/amx
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/Invoke
)

add_definitions (-DPLUGIN_NAME="${PROJECT_NAME}")

# SDK
file (GLOB CA_Source
	${CMAKE_CURRENT_SOURCE_DIR}/SDK/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/SDK/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/SDK/amx/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/SDK/amx/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Invoke/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Invoke/*.h
)

add_library(ColAndreas SHARED ${CA_Source})