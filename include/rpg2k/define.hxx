#ifndef _INC_RPG2K__DEFINE_HXX_
#define _INC_RPG2K__DEFINE_HXX_

#include <iosfwd>
#include <string>

#include "config.hxx"
#include "vec.hxx"

#include <boost/checked_delete.hpp>
#include <boost/cstdint.hpp>
#include <boost/functional/hash.hpp>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>


namespace rpg2k
{
	using namespace cheeze;

	template<class T, class D = boost::checked_deleter<T> >
	struct unique_ptr
	{
		typedef boost::interprocess::unique_ptr<T, D> type;
	};

	class system_string;
	class string : public std::string
	{
	public:
		explicit string() : std::string() {}
		string(std::string const& str) : std::string(str) {}
		string(std::string const& str, size_t pos, size_t n = npos) : std::string(str, pos, n) {}
		string(char const* s, size_t n) : std::string(s, n) {}
		string(char const* s) : std::string(s) {}
		string(size_t n, char c) : std::string(n, c) {}
		template<class InputIterator>
		string (InputIterator begin, InputIterator end) : std::string(begin, end) {}

		system_string to_system() const;

		std::ostream& serialize(std::ostream& os) const;
		size_t serialized_size() const { return this->size(); }
	}; // class string
	class system_string : public std::string
	{
	public:
		explicit system_string() : std::string() {}
		system_string(std::string const& str) : std::string(str) {}
		system_string(std::string const& str, size_t pos, size_t n = npos) : std::string(str, pos, n) {}
		system_string(char const* s, size_t n) : std::string(s, n) {}
		system_string(char const* s) : std::string(s) {}
		system_string(size_t n, char c) : std::string(n, c) {}
		template<class InputIterator>
		system_string (InputIterator begin, InputIterator end) : std::string(begin, end) {}

		string to_rpg2k() const;
	}; // class system_string

	std::ostream& operator <<(std::ostream& os, string const& str);
	std::ostream& operator <<(std::ostream& os, system_string const& str);

	namespace
	{
		ivec2 const SCREEN_SIZE(320, 240);

		ivec2 const SCROLL_SIZE(16, 8);

		ivec2 const FACE_SIZE(48, 48);
		ivec2 const CHIP_SIZE(16, 16);
		ivec2 const CHAR_SIZE(24, 32);

		ivec2 const MONEY_WINDOW_SIZE(88, 32);

		char const* const AUDIO_OFF = "(OFF)";

		system_string const PATH_SEPR = "/";
	} // anonymous namespace

	enum {
		FRAME_PER_SECOND = 60,

		MEMBER_MAX = 4,

		ID_MIN = 1,
		DATABASE_MAX = 5000,

		MAP_UNIT_MAX = 9999, SAVE_DATA_MAX = 15,

		CHIP_REPLACE_MAX = 144,

		ITEM_MIN = 0, ITEM_MAX = 99,

		EVENT_STACK_MAX = 1000,
		EVENT_STEP_MAX = 10000,

		INVALID_ID = 0,
		CONDITION_NORMAL = 0,

		EQUIP_PARAM_NUM = 4,

		FADE_IN_MIN = 0, FADE_IN_MAX = 10000,
		VOLUME_MIN = 0, VOLUME_MAX = 100,
		TEMPO_MIN = 50, TEMPO_MAX = 150,
		BALANCE_LEFT = 0, BALANCE_CENTER = 50, BALANCE_RIGHT = 100,
		FADE_IN_DEFAULT = 0,
		VOLUME_DEFAULT = 100,
		TEMPO_DEFAULT = 100,
		BALANCE_DEFAULT = 50,

		PARAM_MAX = 999, PARAM_MIN = 1,

		VARIABLE_DEFAULT = 0,
		FLAG_DEFAULT = false,

#if(defined(RPG2000) || defined(RPG2000_VALUE))
		VARIABLE_MAX =  999999, VARIABLE_MIN = -999999,

		EXP_MAX = 999999, EXP_MIN = 0,
		EXP_DEFAULT = 30,

		LEVEL_MAX = 50, LEVEL_MIN = 1,

		CHAR_HP_MAX = 999, CHAR_HP_MIN = 1,
		MP_MIN = 0,

#	if defined(RPG2000)
		ENEMY_HP_MAX = 9999,
		PICTURE_MAX = 20,
#	elif defined(RPG2000VALUE)
		ENEMY_HP_MAX = 99999,
		PICTURE_MAX = 50,
#	endif
		ENEMY_HP_MIN = 1,
#elif defined(RPG2003)
		VARIABLE_MAXX =  9999999, VARIABLE_MINN = -9999999,

		EXP_MAX = 9999999, EXP_MIN = 0,
		EXP_DEFAULT = 300,

		LEVEL_MAX = 99, LEVEL_MIN = 1,

		CHAR_HP_MAX = 9999, CHAR_HP_MIN = 1,
		ENEMY_HP_MAX = 99999, ENEMY_HP_MIN = 1,

		PICTURE_MAX = 50,
#else
#	error "unknown rpg2k version"
#endif

		MONEY_MIN = 0, MONEY_MAX = VARIABLE_MAX,

		ID_PARTY = 10001,
		ID_BOAT  = 10002, ID_SHIP = 10003, ID_AIRSHIP = 10004,
		ID_THIS  = 10005,
	};

	struct chip_set { enum type { BEGIN, LOWER = BEGIN, UPPER, END, }; };
	struct char_set_dir { enum type { BEGIN, UP = BEGIN, RIGHT, DOWN, LEFT, END }; };
	struct char_set_pat { enum type { BEGIN, LEFT = BEGIN, MIDDLE, RIGHT, END }; };

	struct event_dir { enum type { DOWN = 2, LEFT = 4, RIGHT = 6, UP = 8, }; };
	struct event_move { enum type {
		FIXED, RANDOM_MOVE, UP_DOWN, LEFT_RIGHT,
		TO_PARTY, FROM_PARTY, MANUAL_MOVE,
	}; };
	struct event_start { enum type {
		BEGIN,
		KEY_ENTER = BEGIN, PARTY_TOUCH, EVENT_TOUCH,
		AUTO, PARALLEL, CALLED,
		END,
	}; };
	struct event_priority { enum type { BEGIN, BELOW = BEGIN, CHAR, ABOVE, END, }; };

	struct equip { enum type {
		BEGIN,
		WEAPON = BEGIN, SHIELD, ARMOR, HELMET, OTHER,
		END,
	}; };
	struct item { enum type {
		BEGIN,
		NORMAL = BEGIN,
		WEAPON, SHIELD, ARMOR, HELMET, ACCESSORY,
		MEDICINE, BOOK, SEED, SPECIAL, SWITCH,
		END,
	}; };

	struct param { enum type {
		BEGIN,
		HP = BEGIN, MP,
		ATTACK, GAURD, MIND, SPEED,
		END,
	}; };

	struct skill { enum type {
		BEGIN,
		NORMAL = BEGIN,
		TELEPORT, ESCAPE,
		SWITCH,
		END,
	}; };

	struct window { enum type { NORMAL, NO_FRAME, }; };
	struct wallpaper { enum type { ZOOM, TILE, }; };

	char_set_dir::type to_char_set_dir(event_dir::type dir);
	event_dir::type to_event_dir(char_set_dir::type key);

	unsigned random();
	unsigned random(unsigned ax);
	int random(int in, int ax);

	template<typename T> bool within(T a, T v, T b) { return ((a <= v) && (v < b)); }
	template<typename T> bool within(T v, T b) { return ::rpg2k::within(T(0), v, b); }
	template<typename T> T limit(T min, T v, T max) { return ((v <= min)? min : (max < v)? max : v); }
	template<typename T> T limit(T v, T max) { return ::rpg2k::limit(T(0), v, max); }

	inline bool is_event(unsigned const ev_id)
	{
		return !rpg2k::within<unsigned>(ID_PARTY, ev_id, ID_AIRSHIP+1);
	}

	struct action
	{
		typedef unsigned type;
		struct move { enum {
			UP = 0, RIGHT, DOWN, LEFT,
			RIGHT_UP, RIGHT_DOWN, LEFT_DOWN, LEFT_UP,
			RANDOM, TO_PARTY, FROM_PARTY, A_STEP,
		}; };
		struct face { enum {
			UP = move::A_STEP + 1, RIGHT, DOWN, LEFT,
		}; };
		struct turn { enum {
			RIGHT_90 = face::LEFT + 1, LEFT_90, OPPOSITE,
			RIGHT_OR_LEFT_90, RANDOM, TO_PARTY, OPPOSITE_OF_PARTY,
		}; };
		enum {
			HALT = turn::OPPOSITE_OF_PARTY + 1,
			BEGIN_JUMP, END_JUMP,
			FIX_DIR, UNFIX_DIR,
			SPEED_UP, SPEED_DOWN,
			FREQ_UP, FREQ_DOWN,
			SWITCH_ON, SWITCH_OFF,
			CHANGE_CHAR_SET, PLAY_SOUND,
			BEGIN_SLIP, END_SLIP,
			STOP_ANIME, START_ANIME,
			TRANS_UP, TRANS_DOWN, // transparentcy
		};
	}; // struct action

	struct face { enum type { GOTHIC, MINCHO, }; };
	namespace font
	{
		enum color {
			NORMAL = 0, DISABLE = 3, ENABLE = 0, STATUS = 1,
			MONEY = 1,
			PARAM_UP = 2, PARAM_DOWN = 3, HP_LOW = 4, MP_LOW = 4,
		};

		static ivec2 const FULL_SIZE(12, 12);
		static ivec2 const HALF_SIZE(6, 12);

		typedef uint16_t const font_t;
	} // namespace font
} // namespace rpg2k

namespace boost
{
	template<>
	struct hash<rpg2k::string> : public hash<std::string> {};
}

#endif // _INC_RPG2K__DEFINE_HXX_
