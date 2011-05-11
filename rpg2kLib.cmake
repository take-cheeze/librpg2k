# sets value
# - ${rpg2kLib_DEFINITIONS}  - use it like "ADD_DEFINITIONS(${rpg2kLib_DEFINITIONS})"
# - ${rpg2kLib_INCLUDE_DIRS} - use it like "INCLUDE_DIRECTORIES(${rpg2kLib_INCLUDE_DIRS})"
# - ${rpg2kLib_LIBRARIES}    - use it like "TARGET_LINK_LIBRARIES(${TARGET} ${rpg2kLib_LIBRARIES})"
# - ${rpg2kLib_LIBRARY_DIRS} - use it like "LINK_DIRECTORIES(${rpg2kLib_LIBRARY_DIRS})"

IF(NOT DEFINED rpg2kLib_PATH)
	LIST(APPEND rpg2kLib_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rpg2kLib)
ENDIF()

# DEBUG define
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
	LIST(APPEND rpg2kLib_DEFINITIONS -D RPG2K_DEBUG=1)
ELSE()
	LIST(APPEND rpg2kLib_DEFINITIONS -D RPG2K_DEBUG=0)
ENDIF()

# Boost
FIND_PACKAGE(Boost REQUIRED)

IF(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	LIST(APPEND rpg2kLib_DEFINITIONS "-Wall -Wextra -Werror")
	LIST(APPEND rpg2kLib_LIBRARIES "-static-libgcc")
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	LIST(APPEND rpg2kLib_DEFINITIONS "-Wall -Wextra -Werror")
ENDIF()
LIST(APPEND rpg2kLib_INCLUDE_DIRS
	"${Boost_INCLUDE_DIRS}"
	"${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
	"${rpg2kLib_PATH}/include"
	"${rpg2kLib_PATH}/ntfmt"
)
LIST(APPEND rpg2kLib_LIBRARIES
	"${Boost_LIBRARIES}"
	"pugixml"
	"rpg2kLib"
)
LIST(APPEND rpg2kLib_LIBRARY_DIRS
	"${CMAKE_CURRENT_SOURCE_DIR}/pugixml/scripts"
	"${rpg2kLib_PATH}/lib"
)
IF(APPLE)
	LIST(APPEND rpg2kLib_LIBRARY_DIRS "/usr/local/lib" "/opt/local/lib")
	LIST(APPEND rpg2kLib_LIBRARIES "iconv")
ENDIF()
