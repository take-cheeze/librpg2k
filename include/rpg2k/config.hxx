#ifndef _INC_RPG2K__CONFIG_HXX_
#define _INC_RPG2K__CONFIG_HXX_

// Apple
#if(defined(__APPLE__) && defined(__MACH__))
	#include <TargetConditionals.h>

	#define RPG2K_IS_MAC_OS_X (defined(TARGET_OS_MAC) && TARGET_OS_MAC)
	#define RPG2K_IS_IPHONE (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
	#define RPG2K_IS_IPHONE_SIMULATOR (defined(TARGET_IPHONES_IMULATOR) && TARGET_IPHONE_SIMULATOR)

	#if (!defined(RPG2K_IS_BIG_ENDIAN) && !defined(RPG2K_IS_LITTLE_ENDIAN))
		#if defined(TARGET_RT_LITTLE_ENDIAN)
			#define RPG2K_IS_LITTLE_ENDIAN TARGET_RT_LITTLE_ENDIAN
		#endif
		#if defined(TARGET_RT_BIG_ENDIAN)
			#define RPG2K_IS_BIGENDIAN TARGET_RT_BIG_ENDIAN
		#endif
	#endif
#else
	#define RPG2K_IS_MAC_OS_X 0
	#define RPG2K_IS_IPHONE 0
	#define RPG2K_IS_IPHONE_SIMULATOR 0
#endif

// PSP
#define RPG2K_IS_PSP defined(PSP)

// Windows
#define RPG2K_IS_WINDOWS (\
		defined(WIN32) || defined(WIN32) || \
		defined(WIN64) || defined(WIN64) \
	)
#define RPG2K_IS_LINUX defined(__linux)
#define RPG2K_IS_UNIX  defined(__unix__)

// compiler things
#define RPG2K_IS_GCC defined(__GNUC__)
#define RPG2K_IS_CLANG defined(__clang__)
#if (RPG2K_IS_GCC || RPG2K_IS_CLANG)
	#define RPG2K_USE_RTTI defined(GXX_RTTI)
#endif

#define RPG2K_IS_MSVC defined(_MSC_VER)
#if RPG2K_IS_MSVC
	#default RPG2K_USE_RTTI defined(CPP_RTTI)
#endif


// default values
/*
 * by default RTTI is enabled
 */
#if !defined(RPG2K_USE_RTTI)
	#define RPG2K_USE_RTTI 1
#endif
/*
 * by default endian is little endian
 */
#if(!defined(RPG2K_IS_BIG_ENDIAN) && !defined(RPG2K_IS_LITTLE_ENDIAN))
	#define RPG2K_IS_BIG_ENDIAN 0
	#define RPG2K_IS_LITTLE_ENDIAN 1
#endif

/*
 * set rpg maker version
 * TODO: enable changing at runtime
 */
#if !(defined(RPG2000) || defined(RPG2000_VALUE) || defined(RPG2003))
	// #define RPG2000
	#define RPG2000_VALUE
	// #define RPG2003
#endif

// Debug options
#if RPG2K_DEBUG
#else
	/*
	 * used at <cassert>
	 * disables assert
	 * NDEBUG -> "NO DEBUG"
	 */
	#if !defined(NDEBUG)
		#define NDEBUG
	#endif
#endif

#include <cassert>
#define rpg2k_assert(exp) assert(exp)

/*
 * Game data Directory
 */
#if RPG2K_IS_IPHONE || RPG2K_IS_MAC_OS_X
	#define GAME_DATA_PATH "/user/Media/Photos/RPG2000/yoake"
	#define GAME_FIND_PATH "/user/Media/Photos/RPG2000/"
#elif RPG2K_IS_WINDOWS
	#define GAME_DATA_PATH "C:/Documents and settings/gr/My Documents/My Dropbox/yoake"
	#define GAME_FIND_PATH "D:/ASCII/RPG2000/"
#else
	#define GAME_DATA_PATH "./yoake"
	#define GAME_FIND_PATH "./"
#endif

#endif // _INC_RPG2K__CONFIG_HXX_
