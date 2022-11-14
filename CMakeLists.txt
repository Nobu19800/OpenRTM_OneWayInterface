cmake_minimum_required(VERSION 3.1)

set(target OneWayInterface)
project(${target} CXX)

include("${PROJECT_SOURCE_DIR}/cmake/utils.cmake")

find_package(OpenRTM REQUIRED)

add_subdirectory(idl)

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/idl)
add_definitions(${OPENRTM_CFLAGS})
link_directories(${OPENRTM_LIBRARY_DIRS})

add_library(${target} SHARED ${target}.cpp OneWayInPortConsumer.cpp OneWayInPortConsumer.h OneWayInPortProvider.cpp OneWayInPortProvider.h ${ALL_IDL_SRCS})
target_link_libraries(${target} ${OPENRTM_LIBRARIES})
target_include_directories(${target} SYSTEM PRIVATE ${OPENRTM_INCLUDE_DIRS})
set_target_properties(${target} PROPERTIES PREFIX "")

if(NOT TARGET ALL_IDL_TGT)
	add_custom_target(ALL_IDL_TGT)
endif(NOT TARGET ALL_IDL_TGT)
set_source_files_properties(${ALL_IDL_SRCS} PROPERTIES GENERATED 1)
add_dependencies(${target} ALL_IDL_TGT)