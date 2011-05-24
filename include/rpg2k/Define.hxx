#ifndef _INC__RPG2K__DEFINES_HPP
#define _INC__RPG2K__DEFINES_HPP

#include <iosfwd>
#include <string>

#include "Config.hxx"
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

	class SystemString;
	class String : public std::string
	{
	public:
		explicit String() : std::string() {}
		String(std::string const& str) : std::string(str) {}
		String(std::string const& str, size_t pos, size_t n = npos) : std::string(str, pos, n) {}
		String(char const* s, size_t n) : std::string(s, n) {}
		String(char const* s) : std::string(s) {}
		String(size_t n, char c) : std::string(n, c) {}
		template<class InputIterator>
		String (InputIterator begin, InputIterator end) : std::string(begin, end) {}

		SystemString toSystem() const;

		std::ostream& serialize(std::ostream& os) const;
		size_t serializedSize() const { return this->size(); }
	}; // class String
	class SystemString : public std::string
	{
	public:
		explicit SystemString() : std::string() {}
		SystemString(std::string const& str) : std::string(str) {}
		SystemString(std::string const& str, size_t pos, size_t n = npos) : std::string(str, pos, n) {}
		SystemString(char const* s, size_t n) : std::string(s, n) {}
		SystemString(char const* s) : std::string(s) {}
		SystemString(size_t n, char c) : std::string(n, c) {}
		template<class InputIterator>
		SystemString (InputIterator begin, InputIterator end) : std::string(begin, end) {}

		String toRPG2k() const;
	}; // class SystemString

	std::ostream& operator <<(std::ostream& os, String const& str);
	std::ostream& operator <<(std::ostream& os, SystemString const& str);

	namespace
	{
		ivec2 const SCREEN_SIZE(320, 240);

		ivec2 const SCROLL_SIZE(16, 8);

		ivec2 const FACE_SIZE(48, 48);
		ivec2 const CHIP_SIZE(16, 16);
		ivec2 const CHAR_SIZE(24, 32);

		ivec2 const MONEY_WINDOW_SIZE(88, 32);

		char const* const AUDIO_OFF = "(OFF)";

		SystemString const PATH_SEPR = "/";
	} // anonymous namespace

	enum {
		FRAME_PER_SECOND = 60,

		MEMBER_MAX = 4,

		ID_MIN = 1,
		DATA_BASE_MAX = 5000,

		MAP_UNIT_MAX = 9999, SAVE_DATA_MAX = 15,

		CHIP_REPLACE_MAX = 144,

		ITEM_MIN = 0, ITEM_MAX = 99,

		EV_STACK_MAX = 1000,
		EV_STEP_MAX = 10000,

		INVALID_ID = 0,
		CONDITION_NORMAL = 0,

		EQ_PARAM_NUM = 4,

		FADEIN_MIN =  0, FADEIN_MAX = 10000,
		VOLUME_MIN =  0, VOLUME_MAX =   100,
		 TEMPO_MIN = 50,  TEMPO_MAX =   150,
		BALANCE_LEFT = 0, BALANCE_CENTER = 50, BALANCE_RIGHT = 100,
		FADE_IN_DEF =   0,
		VOLUME_DEF  = 100,
		TEMPO_DEF   = 100,
		BALANCE_DEF =  50,

		PARAM_MAX = 999, PARAM_MIN = 1,

		VAR_DEF_VAL = 0,
		SWITCH_DEF_VAL = false,

		#if (defined(RPG2000) || defined(RPG2000_VALUE))
			VAR_MAX =  999999, VAR_MIN = -999999,

			EXP_MAX = 999999, EXP_MIN = 0,
			EXP_DEF_VAL = 30,

			LV_MAX = 50, LV_MIN = 1,

			CHAR_HP_MAX = 999, CHAR_HP_MIN = 1,
			MP_MIN = 0,

			#if defined(RPG2000)
				ENEMY_HP_MAX = 9999,
				PICTURE_NUM = 20,
			#elif defined(RPG2000_VALUE)
				ENEMY_HP_MAX = 99999,
				PICTURE_NUM = 50,
			#endif
			ENEMY_HP_MIN = 1,
		#elif defined(RPG2003)
			VAR_MAX =  9999999, VAR_MIN = -9999999,

			EXP_MAX = 9999999, EXP_MIN = 0,
			EXP_DEF_VAL = 300,

			LV_MAX = 99, LV_MIN = 1,

			CHAR_HP_MAX = 9999, CHAR_HP_MIN = 1,
			ENEMY_HP_MAX = 99999, ENEMY_HP_MIN = 1,

			PICTURE_NUM = 50,
		#else
			#error "unknown rpg2k version"
		#endif

		MONEY_MIN = 0, MONEY_MAX = VAR_MAX,

		ID_PARTY = 10001,
		ID_BOAT  = 10002, ID_SHIP = 10003, ID_AIRSHIP = 10004,
		ID_THIS  = 10005,
	};

	struct ChipSet { enum type { BEGIN, LOWER = BEGIN, UPPER, END, }; };
	struct CharSet
	{
		struct Dir { enum type { BEGIN, UP = BEGIN, RIGHT, DOWN, LEFT, END }; };
		struct Pat { enum type { BEGIN, LEFT = BEGIN, MIDDLE, RIGHT, END }; };
	};

	struct EventDir { enum type { DOWN = 2, LEFT = 4, RIGHT = 6, UP = 8, }; };
	struct EventMove { enum type {
		FIXED, RANDOM_MOVE, UP_DOWN, LEFT_RIGHT,
		TO_PARTY, FROM_PARTY, MANUAL_MOVE,
	}; };
	struct EventStart { enum type {
		BEGIN,
		KEY_ENTER = BEGIN, PARTY_TOUCH, EVENT_TOUCH,
		AUTO, PARALLEL, CALLED,
		END,
	}; };
	struct EventPriority { enum type {
		BEGIN,
		BELOW = BEGIN, CHAR, ABOVE,
		END,
	}; };

	struct Equip { enum type {
		BEGIN,
		WEAPON = BEGIN, SHIELD, ARMOR, HELMET, OTHER,
		END,
	}; };
	struct Item { enum type {
		BEGIN,
		NORMAL = BEGIN,
		WEAPON, SHIELD, ARMOR, HELMET, ACCESSORY,
		MEDICINE, BOOK, SEED, SPECIAL, SWITCH,
		END,
	}; };

	struct Param { enum type {
		BEGIN,
		HP = BEGIN, MP,
		ATTACK, GAURD, MIND, SPEED,
		END,
	}; };

	struct Skill { enum type {
		BEGIN,
		NORMAL = BEGIN,
		TELEPORT, ESCAPE,
		SWITCH,
		END,
	}; };

	struct Window { enum type { NORMAL, NO_FRAME, }; };
	struct Wallpaper { enum type { ZOOM, TILE, }; };

	CharSet::Dir::type toCharSetDir(EventDir::type dir);
	EventDir::type toEventDir(CharSet::Dir::type key);

	unsigned random();
	unsigned random(unsigned ax);
	int random(int in, int ax);

	template<typename T> bool within(T a, T v, T b) { return ((a <= v) && (v < b)); }
	template<typename T> bool within(T v, T b) { return ::rpg2k::within(T(0), v, b); }
	template<typename T> T limit(T min, T v, T max) { return ((v <= min)? min : (max < v)? max : v); }
	template<typename T> T limit(T v, T max) { return ::rpg2k::limit(T(0), v, max); }

	inline bool isEvent(unsigned const evID)
	{
		return !rpg2k::within<unsigned>(ID_PARTY, evID, ID_AIRSHIP+1);
	}

	struct Action
	{
		typedef unsigned type;
		struct Move { enum {
			UP = 0, RIGHT, DOWN, LEFT,
			RIGHT_UP, RIGHT_DOWN, LEFT_DOWN, LEFT_UP,
			RANDOM, TO_PARTY, FROM_PARTY, A_STEP,
		}; };
		struct Face { enum {
			UP = Move::A_STEP + 1, RIGHT, DOWN, LEFT,
		}; };
		struct Turn { enum {
			RIGHT_90 = Face::LEFT + 1, LEFT_90, OPPOSITE,
			RIGHT_OR_LEFT_90, RANDOM, TO_PARTY, OPPOSITE_OF_PARTY,
		}; };
		enum {
			HALT = Turn::OPPOSITE_OF_PARTY + 1,
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
	}; // struct Action

	struct Face { enum type { GOTHIC, MINCHO, }; };
	namespace font
	{
		enum Color {
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
	struct hash<rpg2k::String> : public hash<std::string> {};
}

#endif
