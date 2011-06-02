#include "rpg2k/debug.hxx"
#include "rpg2k/stream.hxx"

#include <boost/array.hpp>


namespace rpg2k
{
	namespace stream
	{
		unsigned ber_size(unsigned num)
		{
			unsigned ret = 0;
			do {
				ret++;
				num >>= BER_BIT;
			} while(num);
			return ret;
		}

		bool is_eof(std::istream& is)
		{
			bool const ret = (is.get() == EOF);
			is.unget();
			is.clear();
			return ret;
		}

		string read_header(std::istream& is)
		{
			is.seekg(0, std::ios_base::beg);
			binary buf;
			read_with_size(is, buf);
			return static_cast<string>(buf);
		}
		std::ostream& write_header(std::ostream& os, string const& header)
		{
			os.seekp(0, std::ios_base::beg);
			return write_with_size(os, header);
		}

		bool check_header(std::istream& is,  string const& header)
		{
			return(read_header(is) == header);
		}

		std::ostream& write_ber(std::ostream& os, unsigned num)
		{
			// BER output buffer
			boost::array<uint8_t, (sizeof(num) * 8) / BER_BIT + 1> buf;
			size_t const size = ber_size(num);
			int index = size;
		// set data
			buf[--index] = num & BER_MASK; // BER terminator
			num >>= BER_BIT;
			while(num) {
				buf[--index] = (num & BER_MASK) | BER_SIGN;
				num >>= BER_BIT;
			}
		// write data
			return os.write(reinterpret_cast<char const*>(buf.data()), size);
		}
		unsigned read_ber(std::istream& is)
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

		std::istream& read_with_size(std::istream& is, binary& dst)
		{
			dst.resize(read_ber(is));
			return is.read(reinterpret_cast<char*>(dst.data()), dst.size());
		}
	} // namespace structure
} // namespace rpg2k
