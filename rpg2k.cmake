#sets value
#- ${rpg2k_DEFINITIONS}  - use it like "ADD_DEFINITIONS(${rpg2k_DEFINITIONS})"
#- ${rpg2k_INCLUDE_DIRS} - use it like "INCLUDE_DIRECTORIES(${rpg2k_INCLUDE_DIRS})"
#- ${rpg2k_LIBRARIES}    - use it like "TARGET_LINK_LIBRARIES(${TARGET} ${rpg2k_LIBRARIES})"
#- ${rpg2k_LIBRARY_DIRS} - use it like "LINK_DIRECTORIES(${rpg2k_LIBRARY_DIRS})"

if(NOT rpg2k_PATH)
  set(rpg2k_PATH ${CMAKE_CURRENT_SOURCE_DIR}/librpg2k)
endif()

# DEBUG define
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
  LIST(APPEND rpg2k_DEFINITIONS -DRPG2K_DEBUG=1)
ELSE()
  LIST(APPEND rpg2k_DEFINITIONS -DRPG2K_DEBUG=0)
ENDIF()

# Boost
FIND_PACKAGE(Boost REQUIRED)

# set warning flags
IF((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  LIST(APPEND rpg2k_DEFINITIONS -Wall -Wextra)
ENDIF()

# set endianness
include(TestBigEndian)
test_big_endian(is_big_endian)
if(is_big_endian)
  LIST(APPEND rpg2k_DEFINITIONS
    -DRPG2K_IS_BIG_ENDIAN=1  -DRPG2K_IS_LITTLE_ENDIAN=0)
else()
  LIST(APPEND rpg2k_DEFINITIONS
    -DRPG2K_IS_BIG_ENDIAN=0  -DRPG2K_IS_LITTLE_ENDIAN=1)
endif()

LIST(APPEND rpg2k_INCLUDE_DIRS
  ${Boost_INCLUDE_DIRS}
  ${rpg2k_PATH}/include
  ${rpg2k_PATH}/glsl_type/include
  ${rpg2k_PATH}/ntfmt
  ${rpg2k_PATH}/pugixml/src)
LIST(APPEND rpg2k_LIBRARIES
  rpg2k
  pugixml
  ${Boost_LIBRARIES})
LIST(APPEND rpg2k_LIBRARY_DIRS
  ${rpg2k_PATH}/lib
  ${rpg2k_PATH}/pugixml/scripts)
find_path(rpg2k_ICONV_INCLUDE_DIR "iconv.h")
if(NOT rpg2k_ICONV_INCLUDE_DIR)
  message(FATAL_ERROR "iconv not found")
else()
  message(STATUS "iconv found")
endif()
LIST(APPEND rpg2k_INCLUDE_DIRS ${rpg2k_ICONV_INCLUDE_DIR})
find_library(rpg2k_ICONV_LIBRARY "iconv")
LIST(APPEND rpg2k_LIBRARIES ${rpg2k_ICONV_LIBRARY})

include(${rpg2k_PATH}/PrecompiledHeader.cmake)
precompiled_header_definition(
  CXX rpg2k_DEFINITIONS "${rpg2k_PATH}/include/rpg2k.pch")
