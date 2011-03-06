#include <cstdlib>

#include "Debug.hxx"
#include "Encode.hxx"

#include <boost/array.hpp>


namespace rpg2k
{
	#if RPG2K_IS_WINDOWS
		char const* Encode::SYSTEM_ENCODE = "CP932"; // "Windows-31J"
	#else
		char const* Encode::SYSTEM_ENCODE = "UTF-8";
	#endif
	char const* Encode::RPG2K_ENCODE = "CP932"; // "Shift_JIS";

	Encode::Encode()
	{
		sysEncode_ = SYSTEM_ENCODE;

		/*
		 * geting system encoding name from "LANG" env
		 * works only on unix systems(as I know)
		 */
		#define getlang() getenv("LANG")
		if(getlang()) {
			eastl::string const langStr = getlang();
			std::size_t const pos = langStr.find('.');
			if(pos != eastl::string::npos) {
				sysEncode_ = langStr.substr(pos + 1);
				// std::cout << sysEncode_ << std::endl;
			}
		}
		#undef getlang

		toSystem_ = openConverter(sysEncode_.c_str(), RPG2K_ENCODE);
		toRPG2k_  = openConverter(RPG2K_ENCODE, sysEncode_.c_str());
	}
	Encode::~Encode()
	{
		if(::iconv_close(toSystem_) == -1) rpg2k_assert(false);
		if(::iconv_close(toRPG2k_) == -1) rpg2k_assert(false);

		toSystem_ = toRPG2k_ = NULL;
	}

	iconv_t Encode::openConverter(char const* to, char const* from)
	{
		::iconv_t ret = ::iconv_open(to, from);
		rpg2k_assert(ret != ::iconv_t(-1));
		return ret;
	}

	eastl::string Encode::convertString(eastl::string const& src, iconv_t cd)
	{
		boost::array<char, BUFF_SIZE> iconvBuff;
		::size_t iconvOutSize = BUFF_SIZE, iconvInSize  = src.size();
		char* iconvOut = iconvBuff.data();
		#if RPG2K_IS_PSP && !defined(_LIBICONV_H)
			char const* iconvIn  = src.c_str();
		#else
			char* iconvIn  = const_cast<char*>(src.c_str());
		#endif

		eastl::string ret;
		while(iconvInSize > 0) {
			if(::iconv(cd, &iconvIn, &iconvInSize, &iconvOut, &iconvOutSize) == (::size_t) -1) {
				rpg2k_analyze_assert(false);
			}
			ret.append(iconvBuff.data(), BUFF_SIZE-iconvOutSize);
			iconvOutSize = BUFF_SIZE;
		}
		return ret;
	}
} // namespace rpg2k
