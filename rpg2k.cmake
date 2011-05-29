# sets value
# - ${rpg2k_DEFINITIONS}  - use it like "ADD_DEFINITIONS(${rpg2k_DEFINITIONS})"
# - ${rpg2k_INCLUDE_DIRS} - use it like "INCLUDE_DIRECTORIES(${rpg2k_INCLUDE_DIRS})"
# - ${rpg2k_LIBRARIES}    - use it like "TARGET_LINK_LIBRARIES(${TARGET} ${rpg2k_LIBRARIES})"
# - ${rpg2k_LIBRARY_DIRS} - use it like "LINK_DIRECTORIES(${rpg2k_LIBRARY_DIRS})"

IF(NOT DEFINED rpg2k_PATH)
	LIST(APPEND rpg2k_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rpg2k)
ENDIF()

# DEBUG define
IF(CMAKE_BUILD_TYPE MATCHES "Debug")
	LIST(APPEND rpg2k_DEFINITIONS -D RPG2K_DEBUG=1)
ELSE()
	LIST(APPEND rpg2k_DEFINITIONS -D RPG2K_DEBUG=0)
ENDIF()

# Boost
FIND_PACKAGE(Boost REQUIRED)

IF((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
	LIST(APPEND rpg2k_DEFINITIONS
		"-Wall" "-Wextra"
		"-Wno-unused-variable" "-Wno-unused-parameter"
	)
ENDIF()

LIST(APPEND rpg2k_INCLUDE_DIRS
	"${Boost_INCLUDE_DIRS}"
	"${rpg2k_PATH}/include"
	"${rpg2k_PATH}/glsl_type/include"
	"${rpg2k_PATH}/ntfmt"
	"${rpg2k_PATH}/pugixml/src"
)
LIST(APPEND rpg2k_LIBRARIES
	"${Boost_LIBRARIES}"
	"pugixml"
	"rpg2k"
)
LIST(APPEND rpg2k_LIBRARY_DIRS
	"${rpg2k_PATH}/lib"
	"${rpg2k_PATH}/pugixml/scripts"
)
IF(APPLE)
	LIST(APPEND rpg2k_LIBRARY_DIRS "/usr/local/lib" "/opt/local/lib")
	LIST(APPEND rpg2k_LIBRARIES "iconv")
ENDIF()
