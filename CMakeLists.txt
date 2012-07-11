CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT("rpg2k" CXX C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_module)

# output path
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

find_package(RPG2k REQUIRED)

ADD_DEFINITIONS(${RPG2k_DEFINITIONS})
INCLUDE_DIRECTORIES(${RPG2k_INCLUDE_DIRS})
LINK_DIRECTORIES(${RPG2k_LIBRARY_DIRS})

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src SRCS)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${RPG2k_LIBRARIES})
ADD_DEPENDENCIES(${PROJECT_NAME} "pugixml")

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
