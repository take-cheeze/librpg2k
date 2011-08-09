CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT("rpg2k" CXX C)

# pugixml
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/pugixml/scripts)

# glsl_type
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/glsl_type)

# output path
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# rpg2k variables
SET(rpg2k_PATH ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/rpg2k.cmake)

ADD_DEFINITIONS(${rpg2k_DEFINITIONS})
INCLUDE_DIRECTORIES(${rpg2k_INCLUDE_DIRS})
LINK_DIRECTORIES(${rpg2k_LIBRARY_DIRS})

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src SRCS)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${rpg2k_LIBRARIES})
ADD_DEPENDENCIES(${PROJECT_NAME} "pugixml")

# precompiled header
include(${rpg2k_PATH}/PrecompiledHeader.cmake)
file(GLOB_RECURSE all_header "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hxx")
create_precompiled_header(CXX
  "${CMAKE_CURRENT_SOURCE_DIR}/include/rpg2k.pch" ${all_header})
add_dependencies(${PROJECT_NAME} rpg2k.pch)

# test
enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# gtest-config
find_program(GTEST_CONFIG "gtest-config")
if(NOT GTEST_CONFIG)
  message(FATAL_ERROR "gtest-config not found")
else()
  message(STATUS "gtest-config found")
endif()
# cxx_flags
execute_process(
  COMMAND ${GTEST_CONFIG} "--cppflags" "--cxxflags"
  OUTPUT_VARIABLE GTEST_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
LIST(APPEND CMAKE_CXX_FLAGS ${GTEST_CXX_FLAGS})
# link_flags
execute_process(
  COMMAND ${GTEST_CONFIG} "--ldflags" "--libs"
  OUTPUT_VARIABLE GTEST_LINK_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
list(APPEND LINK_FLAGS ${GTEST_LINK_FLAGS})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/test CXX_TEST_SOURCES)
foreach(i ${CXX_TEST_SOURCES})
  string(REGEX REPLACE ".*/\([a-z_]*\).cxx" "\\1" exe ${i})
  add_executable(test_${exe} ${i})
  target_link_libraries(test_${exe}
    ${rpg2k_LIBRARIES}
    ${libs}
    ${GTEST_BOTH_LIBRARIES}
    )
  add_test(
    NAME ${exe}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_${exe}
    )
  add_dependencies(test_${exe} ${PROJECT_NAME})
endforeach()
