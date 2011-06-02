#include <cstdlib>

#include "rpg2k/debug.hxx"
#include "rpg2k/encode.hxx"

#include <boost/array.hpp>


namespace rpg2k
{
	#if RPG2K_IS_WINDOWS
		char const* encode::SYSTEM_ENCODE = "CP932"; // "Windows-31J"
	#else
		char const* encode::SYSTEM_ENCODE = "UTF-8";
	#endif
	char const* encode::RPG2K_ENCODE = "CP932"; // "Shift_JIS";

	encode::encode()
	{
		system_encode_ = SYSTEM_ENCODE;

		/*
		 * geting system encoding name from "LANG" env
		 * works only on unix systems(as I know)
		 */
#define get_lang() std::getenv("LANG")
		if(get_lang()) {
			std::string const lang_str = get_lang();
			std::size_t const pos = lang_str.find('.');
			if(pos != std::string::npos) {
				system_encode_ = lang_str.substr(pos + 1);
				// std::cout << sysencode_ << std::endl;
			}
		}
#undef get_lang

		to_system_ = open_converter(system_encode_.c_str(), RPG2K_ENCODE);
		to_rpg2k_  = open_converter(RPG2K_ENCODE, system_encode_.c_str());
	}
	encode::~encode()
	{
		if(::iconv_close(to_system_) == -1) rpg2k_assert(false);
		if(::iconv_close(to_rpg2k_) == -1) rpg2k_assert(false);

		to_system_ = to_rpg2k_ = NULL;
	}

	iconv_t encode::open_converter(char const* to, char const* from)
	{
		::iconv_t ret = ::iconv_open(to, from);
		rpg2k_assert(ret != ::iconv_t(-1));
		return ret;
	}

	std::string encode::convert_string(std::string const& src, iconv_t cd)
	{
		boost::array<char, BUF_SIZE> iconv_buf;
		::size_t iconv_out_size = BUF_SIZE, iconv_in_size  = src.size();
		char* iconv_out = iconv_buf.data();
		#if RPG2K_IS_PSP && !defined(_LIBICONV_H)
			char const* iconv_in  = src.c_str();
		#else
			char* iconv_in  = const_cast<char*>(src.c_str());
		#endif

		std::string ret;
		while(iconv_in_size > 0) {
			if(::iconv(cd, &iconv_in, &iconv_in_size, &iconv_out, &iconv_out_size) == (::size_t) -1) {
				rpg2k_analyze_assert(false);
			}
			ret.append(iconv_buf.data(), BUF_SIZE-iconv_out_size);
			iconv_out_size = BUF_SIZE;
		}
		return ret;
	}
} // namespace rpg2k
