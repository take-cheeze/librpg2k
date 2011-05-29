#ifndef _INC__STRUCTURE__SPECIAL_ARRAY1D_HPP
#define _INC__STRUCTURE__SPECIAL_ARRAY1D_HPP

#include "array1d.hxx"
#include "element.hxx"


namespace rpg2k
{
	namespace structure
	{
		class Sound : public Array1D
		{
		public:
			String const& fileName() const { return (*this)[1]; }
			int volume () const { return (*this)[3]; }
			int tempo  () const { return (*this)[4]; }
			int balance() const { return (*this)[5]; }

			operator String() const { return fileName(); }
		}; // class Sound

		class Music : public Sound
		{
		public:
			int fadeInTime() const { return (*this)[2]; }
		}; // class Music

		class EventState : public Array1D
		{
		public:
			int mapID() const { return (*this)[11]; }
			int x() const { return (*this)[12]; }
			int y() const { return (*this)[13]; }
			vec2 position() const { return vec2(x(), y()); }

			String const& charSet() const { return (*this)[73]; }
			int charSetPos() const { return (*this)[74]; }
			CharSet::Dir::type charSetDir() const { return CharSet::Dir::type((*this)[75].to<int>()); }
			CharSet::Pat::type charSetPat() const { return CharSet::Pat::MIDDLE; }

			CharSet::Dir::type eventDir() const { return CharSet::Dir::type((*this)[21].to<int>()); }
			CharSet::Dir::type  talkDir() const { return CharSet::Dir::type((*this)[22].to<int>()); }

			// CharSetDir charSetDir() const;
		}; // class EventState
	} // namespace structure
} // namespace rpg2k

#endif // _INC__STRUCTURE__SPECIAL_ARRAY1D_HPP
