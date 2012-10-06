cmake_minimum_required(VERSION 2.6)

project("rpg2k" CXX C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_module)

# output path
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

find_package(RPG2k REQUIRED)

add_definitions(${RPG2k_DEFINITIONS})
include_directories(${RPG2k_INCLUDE_DIRS})
link_directories(${RPG2k_LIBRARY_DIRS})

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx)
add_library(${PROJECT_NAME} STATIC ${SRCS})
target_link_libraries(${PROJECT_NAME} ${RPG2k_LIBRARIES})
add_dependencies(${PROJECT_NAME} "pugixml")

# picojson
file(
  DOWNLOAD "https://github.com/take-cheeze/picojson/raw/master/picojson.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/picojson.h")

# test
enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
find_package(Threads)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/test CXX_TEST_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ntfmt NTFMT_TEST_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ntfmt/test NTFMT_SAMPLE_SOURCES)
foreach(i ${CXX_TEST_SOURCES} ${NTFMT_TEST_SOURCES} ${NTFMT_SAMPLE_SOURCES})
  get_filename_component(exe ${i} NAME_WE)
  add_executable(test_${exe} ${i})
  target_link_libraries(test_${exe}
    ${RPG2k_LIBRARIES} ${libs}
    ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  add_test(
    NAME ${exe}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/TestGame
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_${exe}
    --gtest_output=xml:${EXECUTABLE_OUTPUT_PATH}/test_${target}.xml
    --gtest_color=yes)
  add_dependencies(test_${exe} ${PROJECT_NAME})
endforeach()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tool TOOLS)
foreach(i ${TOOLS})
  get_filename_component(exe ${i} NAME_WE)
  add_executable(${exe} ${i})
  target_link_libraries(${exe} ${RPG2k_LIBRARIES} ${libs})
  add_dependencies(${exe} ${PROJECT_NAME})
endforeach()
