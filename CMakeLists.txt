CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT("rpg2kLib" CXX)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/rpg2kLib.cmake)
FIND_RPG2KLIB(${CMAKE_CURRENT_SOURCE_DIR})

# rpg2kLib code
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/rpg2k SRCS)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/rpg2k/define SRCS)
SET(SRCS ${SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/eastl_new.cxx)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})

ADD_DEPENDENCIES(${PROJECT_NAME} "EASTL")

# test
ENABLE_TESTING()

FUNCTION(CXX_TEST target libs)
	ADD_EXECUTABLE(test/${target} ${CMAKE_CURRENT_SOURCE_DIR}/test/${target}.cxx)
	TARGET_LINK_LIBRARIES(test/${target}
		${rpg2kLib_LIBRARIES}
		${libs}
		"gtest"
	)
	ADD_TEST(test/${target} test/${target})
	ADD_DEPENDENCIES(test/${target} ${PROJECT_NAME})

	SET(TEST_TARGET "${TEST_TARGET};test/${target}" PARENT_SCOPE)
ENDFUNCTION()

LINK_DIRECTORIES(/opt/local/lib)
CXX_TEST(encode "gtest_main")
CXX_TEST(model "gtest_main")
CXX_TEST(structure "gtest_main")

# borrowed from
# http://stackoverflow.com/questions/733475/cmake-ctest-make-test-doesnt-build-tests
ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${TEST_TARGET})
