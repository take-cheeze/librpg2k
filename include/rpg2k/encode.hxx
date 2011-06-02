#ifndef _INC_RPG2K__ENCODE_HXX_
#define _INC_RPG2K__ENCODE_HXX_

#include "define.hxx"
#include "singleton.hxx"

#include <iconv.h>


namespace rpg2k
{
	class encode : public singleton<encode>
	{
		friend class singleton<encode>;
	private:
		enum { BUF_SIZE = 1024, };

		static char const* SYSTEM_ENCODE;
		static char const* RPG2K_ENCODE;

		::iconv_t to_system_, to_rpg2k_;
		std::string system_encode_;
	protected:
		encode();
		~encode();

		static std::string convert_string(std::string const& src, iconv_t cd);
		static iconv_t open_converter(char const* to, char const* from);
	public:
		system_string to_system(string const& src) const { return convert_string(src, to_system_); }
		string to_rpg2k(system_string const& src) const { return convert_string(src, to_rpg2k_); }

		std::string const& system_encoding() const { return system_encode_; }
	}; // class encode
} // namespace rpg2k

#endif // _INC_RPG2K__ENCODE_HXX_
