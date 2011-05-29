#include "rpg2k/array1d_wrapper.hxx"
#include "rpg2k/debug.hxx"
#include "rpg2k/element.hxx"
#include "rpg2k/encode.hxx"
#include "rpg2k/stream.hxx"

#include <boost/foreach.hpp>


namespace 
{
	typedef int32_t rpg2k_integer;
} // namespace

namespace rpg2k
{
	std::ostream& operator <<(std::ostream& os, String const& str)
	{
		return(os << str.toSystem().c_str());
	}
	std::ostream& operator <<(std::ostream& os, SystemString const& str)
	{
		return(os << str.c_str());
	}

	CharSet::Dir::type toCharSetDir(EventDir::type const dir)
	{
		switch(dir) {
			case EventDir::DOWN : return CharSet::Dir::DOWN ;
			case EventDir::LEFT : return CharSet::Dir::LEFT ;
			case EventDir::RIGHT: return CharSet::Dir::RIGHT;
			case EventDir::UP   : return CharSet::Dir::UP   ;
			default: return CharSet::Dir::DOWN;
		}
	}
	EventDir::type toEventDir(CharSet::Dir::type const key)
	{
		switch(key) {
			case CharSet::Dir::UP   : return EventDir::UP   ;
			case CharSet::Dir::LEFT : return EventDir::LEFT ;
			case CharSet::Dir::RIGHT: return EventDir::RIGHT;
			case CharSet::Dir::DOWN : return EventDir::DOWN ;
			default: return EventDir::DOWN;
		}
	}

	/*
	 * xor shift random number generator
	 * from http://ja.wikipedia.org/wiki/Xorshift
	 */
	unsigned random()
	{
		static unsigned x=123456789, y=362436069, z=521288629, w=88675123;

		unsigned t = (x^(x << 11));
		x=y; y=z; z=w;
		return (w = (w^(w >> 19)) ^ (t^(t >> 8)));
	}
	unsigned random(unsigned const max)
	{
		return (random() % max);
	}
	int random(int const min, int const max)
	{
		return (random(max - min + 1) + min);
	}

	SystemString String::toSystem() const
	{
		return Encode::instance().toSystem(*this);
	}
	std::ostream& String::serialize(std::ostream& os) const
	{
		return os.write(this->c_str(), this->size());
	}
	String SystemString::toRPG2k () const
	{
		return Encode::instance().toRPG2k(*this);
	}

	bool Binary::isBER() const
	{
		if(!size() || ((size() > (sizeof(uint32_t) * CHAR_BIT) / stream::BER_BIT + 1))) return false;

		const_reverse_iterator it = std::vector<uint8_t>::rbegin();
		if(*it > stream::BER_SIGN) return false;

		while(++it < std::vector<uint8_t>::rend()) if(*it < stream::BER_SIGN) return false;

		return true;
	}
	bool Binary::isString() const
	{
		BOOST_FOREACH(uint8_t const i, *this) if(std::iscntrl(i)) return false;
		try {
			String(reinterpret_cast<char const*>(this->data()), this->size()).toSystem();
			return true;
		} catch(debug::AnalyzeException const&) { return false; }
	}

	Binary::operator int() const
	{
		rpg2k_assert(isBER());

		namespace io = boost::iostreams;
		io::stream<io::array_source> s(this->source());
		return rpg2k_integer(stream::readBER(s));
	}
	Binary::operator bool() const
	{
		rpg2k_assert(size() == sizeof(bool));
		switch(static_cast<int>(*this)) {
			case false: return false;
			case true : return true ;
			default: rpg2k_assert(false);
		}
		return false;
	}
	Binary::operator double() const
	{
		rpg2k_assert(size() == sizeof(double));
		return *(reinterpret_cast<double const*>(this->data()));
	}

	Binary& Binary::operator =(int const num)
	{
		this->resize(stream::berSize(num));
		namespace io = boost::iostreams;
		io::stream<io::array_sink> s(this->sink());
		stream::writeBER(s, rpg2k_integer(num));
		return *this;
	}
	Binary& Binary::operator =(bool const b)
	{
		resize(sizeof(bool));
		(*this)[0] = b;
		return *this;
	}
	Binary& Binary::operator =(double const d)
	{
		resize(sizeof(double));
		*((double*)this->data()) = d;
		return *this;
	}

	size_t Binary::serializedSize() const { return this->size(); }
	std::ostream& Binary::serialize(std::ostream& s) const
	{
		return s.write(reinterpret_cast<char const*>(this->data()), this->size());
	}
} // namespace rpg2k
