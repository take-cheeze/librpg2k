# sets value
# - ${rpg2kLib_DEFINITIONS}  - use it like "ADD_DEFINITIONS(${rpg2kLib_DEFINITIONS})"
# - ${rpg2kLib_INCLUDE_DIRS} - use it like "INCLUDE_DIRECTORIES(${rpg2kLib_INCLUDE_DIRS})"
# - ${rpg2kLib_LIBRARIES}    - use it like "TARGET_LINK_LIBRARIES(${TARGET} ${rpg2kLib_LIBRARIES})"
# - ${rpg2kLib_LIBRARY_DIRS} - use it like "LINK_DIRECTORIES(${rpg2kLib_LIBRARY_DIRS})"

IF(NOT DEFINED rpg2kLib_PATH)
	SET(rpg2kLib_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rpg2kLib)
ENDIF()

# DEBUG define
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
	SET(rpg2kLib_DEFINITIONS -D RPG2K_DEBUG=1)
ELSE()
	SET(rpg2kLib_DEFINITIONS -D RPG2K_DEBUG=0)
ENDIF()

# Boost
FIND_PACKAGE(Boost REQUIRED)

LIST(APPEND rpg2kLib_DEFINITIONS
	"-Wall" "-Wextra" "-Werror"
)
SET(rpg2kLib_INCLUDE_DIRS
	"${Boost_INCLUDE_DIRS}"
	"${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
	"${rpg2kLib_PATH}/include"
)
SET(rpg2kLib_LIBRARIES
	"${Boost_LIBRARIES}"
	"pugixml"
	"rpg2kLib"
)
SET(rpg2kLib_LIBRARY_DIRS
	"${CMAKE_CURRENT_SOURCE_DIR}/pugixml/scripts"
	"${rpg2kLib_PATH}/lib"
)
IF(APPLE)
	LIST(APPEND rpg2kLib_LIBRARY_DIRS "/opt/local/lib")
	LIST(APPEND rpg2kLib_LIBRARIES "iconv")
ENDIF()
