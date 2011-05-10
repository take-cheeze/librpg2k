#include "rpg2k/Debug.hxx"
#include "rpg2k/Stream.hxx"

#include <boost/array.hpp>


namespace rpg2k
{
	namespace stream
	{
		unsigned berSize(unsigned num)
		{
			unsigned ret = 0;
			do {
				ret++;
				num >>= BER_BIT;
			} while(num);
			return ret;
		}

		bool isEOF(std::istream& is)
		{
			bool const ret = (is.get() == EOF);
			is.unget();
			is.clear();
			return ret;
		}

		String readHeader(std::istream& is)
		{
			is.seekg(0, std::ios_base::beg);
			Binary buf;
			readWithSize(is, buf);
			return static_cast<String>(buf);
		}
		std::ostream& writeHeader(std::ostream& os, String const& header)
		{
			os.seekp(0, std::ios_base::beg);
			return writeWithSize(os, header);
		}

		bool checkHeader(std::istream& is,  String const& header)
		{
			return(readHeader(is) == header);
		}

		std::ostream& writeBER(std::ostream& os, unsigned num)
		{
			// BER output buffer
			boost::array<uint8_t, (sizeof(num) * CHAR_BIT) / BER_BIT + 1> buff;
			size_t const size = berSize(num);
			int index = size;
		// set data
			buff[--index] = num & BER_MASK; // BER terminator
			num >>= BER_BIT;
			while(num) {
				buff[--index] = (num & BER_MASK) | BER_SIGN;
				num >>= BER_BIT;
			}
		// write data
			return os.write(reinterpret_cast<char const*>(buff.data()), size);
		}
		unsigned readBER(std::istream& is)
		{
			unsigned ret = 0;
			uint8_t data;
		// extract
			do {
				is.read(reinterpret_cast<char*>(&data), 1);
				ret = (ret << BER_BIT) | (data & BER_MASK);
			} while(data >= BER_SIGN);
		// result
			return ret;
		}

		std::istream& readWithSize(std::istream& is, Binary& dst)
		{
			dst.resize(readBER(is));
			return is.read(reinterpret_cast<char*>(dst.data()), dst.size());
		}
	} // namespace structure
} // namespace rpg2k