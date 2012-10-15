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
	std::ostream& operator <<(std::ostream& os, string const& str)
	{
		return(os << str.to_system().c_str());
	}
	std::ostream& operator <<(std::ostream& os, system_string const& str)
	{
		return(os << str.c_str());
	}

	char_set_dir::type to_char_set_dir(event_dir::type const dir)
	{
		switch(dir) {
			case event_dir::DOWN : return char_set_dir::DOWN ;
			case event_dir::LEFT : return char_set_dir::LEFT ;
			case event_dir::RIGHT: return char_set_dir::RIGHT;
			case event_dir::UP   : return char_set_dir::UP   ;
			default: return char_set_dir::DOWN;
		}
	}
	event_dir::type to_event_dir(char_set_dir::type const key)
	{
		switch(key) {
			case char_set_dir::UP   : return event_dir::UP   ;
			case char_set_dir::LEFT : return event_dir::LEFT ;
			case char_set_dir::RIGHT: return event_dir::RIGHT;
			case char_set_dir::DOWN : return event_dir::DOWN ;
			default: return event_dir::DOWN;
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

	system_string string::to_system() const
	{
		return encode::instance().to_system(*this);
	}
	std::ostream& string::serialize(std::ostream& os) const
	{
		return os.write(this->c_str(), this->size());
	}
	string system_string::to_rpg2k() const
	{
		return encode::instance().to_rpg2k(*this);
	}

	bool binary::is_ber() const
	{
		if(!size() || ((size() > (sizeof(uint32_t) * 8) / stream::BER_BIT + 1))) return false;

		const_reverse_iterator it = std::vector<uint8_t>::rbegin();
		if(*it > stream::BER_SIGN) return false;

		while(++it < std::vector<uint8_t>::rend()) if(*it < stream::BER_SIGN) return false;

		return true;
	}
	bool binary::is_string() const
	{
		BOOST_FOREACH(uint8_t const i, *this) if(std::iscntrl(i)) return false;
		try {
			string(reinterpret_cast<char const*>(this->data()), this->size()).to_system();
			return true;
		} catch(debug::analyze_exception const&) { return false; }
	}

	binary::operator int() const
	{
		rpg2k_assert(is_ber());

		namespace io = boost::iostreams;
		io::stream<io::array_source> s(this->source());
		return rpg2k_integer(stream::read_ber(s));
	}
	binary::operator bool() const
	{
		rpg2k_assert(size() == sizeof(bool));
		switch(static_cast<int>(*this)) {
			case false: return false;
			case true : return true ;
			default: rpg2k_assert(false);
		}
		return false;
	}
	binary::operator double() const
	{
		rpg2k_assert(size() == sizeof(double));
		return *(reinterpret_cast<double const*>(this->data()));
	}

	binary& binary::operator =(int const num)
	{
		this->resize(stream::ber_size(num));
		namespace io = boost::iostreams;
		io::stream<io::array_sink> s(this->sink());
		stream::write_ber(s, rpg2k_integer(num));
		return *this;
	}
	binary& binary::operator =(bool const b)
	{
		resize(sizeof(bool));
		(*this)[0] = b;
		return *this;
	}
	binary& binary::operator =(double const d)
	{
		resize(sizeof(double));
		*((double*)this->data()) = d;
		return *this;
	}

	size_t binary::serialized_size() const { return this->size(); }
	std::ostream& binary::serialize(std::ostream& s) const
	{
		return s.write(reinterpret_cast<char const*>(this->data()), this->size());
	}
} // namespace rpg2k
