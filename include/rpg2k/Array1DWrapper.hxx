#ifndef _INC__STRUCTURE__SPECIAL_ARRAY1D_HPP
#define _INC__STRUCTURE__SPECIAL_ARRAY1D_HPP

#include "Array1D.hxx"
#include "Element.hxx"


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
			Vector2D position() const { return Vector2D(x(), y()); }

			String const& charSet() const { return (*this)[73]; }
			int charSetPos() const { return (*this)[74]; }
			CharSet::Dir charSetDir() const { return CharSet::Dir((*this)[75].to<int>()); }
			CharSet::Pat charSetPat() const { return CharSet::Pat::MIDDLE; }

			CharSet::Dir eventDir() const { return CharSet::Dir((*this)[21].to<int>()); }
			CharSet::Dir  talkDir() const { return CharSet::Dir((*this)[22].to<int>()); }

			// CharSetDir charSetDir() const;
		}; // class EventState
	} // namespace structure
} // namespace rpg2k

#endif // _INC__STRUCTURE__SPECIAL_ARRAY1D_HPP
