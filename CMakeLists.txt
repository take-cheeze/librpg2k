CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT("rpg2kLib" CXX C)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib)

SET(rpg2kLib_PATH ${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/rpg2kLib.cmake)

ADD_DEFINITIONS(${rpg2kLib_DEFINITIONS})
INCLUDE_DIRECTORIES(${rpg2kLib_INCLUDE_DIRS})
LINK_DIRECTORIES(${rpg2kLib_LIBRARY_DIRS})

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src SRCS)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${rpg2kLib_LIBRARIES})

# test
enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# gtest-config
find_program(GTEST_CONFIG "gtest-config")
if(NOT GTEST_CONFIG)
	message(SEND_ERROR "gtest-config not found")
else()
	message(STATUS "gtest-config found")
endif()
# cxx_flags
exec_program(${GTEST_CONFIG} ARGS "--cppflags --cxxflags" OUTPUT_VARIABLE GTEST_CXX_FLAGS)
LIST(APPEND CMAKE_CXX_FLAGS ${GTEST_CXX_FLAGS})
# link_flags
exec_program(${GTEST_CONFIG} ARGS "--ldflags --libs" OUTPUT_VARIABLE GTEST_LINK_FLAGS)
list(APPEND LINK_FLAGS ${GTEST_LINK_FLAGS})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/test CXX_TEST_SOURCES)
foreach(i ${CXX_TEST_SOURCES})
	string(REGEX REPLACE ".*/\([a-z_]*\).cxx" "\\1" exe ${i})
	add_executable(test_${exe} ${i})
	target_link_libraries(test_${exe}
		${rpg2kLib_LIBRARIES}
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

# pugixml
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/pugixml/scripts)
ADD_DEPENDENCIES(${PROJECT_NAME} "pugixml")
