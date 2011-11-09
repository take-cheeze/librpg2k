#ifndef _INC_RPG2K__ARRAY1D_WRAPPER_HXX_
#define _INC_RPG2K__ARRAY1D_WRAPPER_HXX_

#include "array1d.hxx"
#include "element.hxx"


namespace rpg2k
{
	namespace structure
	{
		class sound : public array1d
		{
		public:
			string const& filename() const { return (*this)[1]; }
			int volume () const { return (*this)[3]; }
			int tempo  () const { return (*this)[4]; }
			int balance() const { return (*this)[5]; }

			operator string() const { return filename(); }
		}; // class sound

		class music : public sound
		{
		public:
			int fade_in_time() const { return (*this)[2]; }
		}; // class music

		class event_state : public array1d
		{
		public:
			int map_id() const { return (*this)[11]; }
			int x() const { return (*this)[12]; }
			int y() const { return (*this)[13]; }
			vec2 position() const { return vec2(x(), y()); }

			string const& char_set() const { return (*this)[73]; }
			int char_set_pos() const { return (*this)[74]; }
			rpg2k::char_set_dir::type char_set_dir() const { return rpg2k::char_set_dir::type((*this)[75].to<int>()); }
			rpg2k::char_set_pat::type char_set_pat() const { return rpg2k::char_set_pat::MIDDLE; }

			rpg2k::char_set_dir::type event_dir() const { return rpg2k::char_set_dir::type((*this)[21].to<int>()); }
			rpg2k::char_set_dir::type  talk_dir() const { return rpg2k::char_set_dir::type((*this)[22].to<int>()); }

			// char_set_dir char_set_dir() const;
		}; // class event_state
	} // namespace structure
} // namespace rpg2k

#endif // _INC_RPG2K__ARRAY1D_WRAPPER_HXX_
