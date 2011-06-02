#ifndef _INC_RPG2K__STREAM_HXX_
#define _INC_RPG2K__STREAM_HXX_

#include "structure.hxx"

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>

#include <iosfwd>


namespace rpg2k {
	namespace structure {
		namespace io = boost::iostreams;

		template<class T>
		binary serialize(T const& src)
		{
			binary ret(src.serialized_size());
			io::stream<io::array_sink> s(ret.sink());
			src.serialize(s);

			return ret;
		}
	}

	namespace stream {
		enum {
			BER_BIT  = (8-1),
			BER_SIGN = 0x01 << BER_BIT,
			BER_MASK = BER_SIGN - 1,
		};
		unsigned ber_size(unsigned num);

		std::ios_base::openmode const INPUT_FLAG
		= std::ios_base::in  | std::ios_base::binary;
		std::ios_base::openmode const OUTPUT_FLAG
		= std::ios_base::out | std::ios_base::binary | std::ios_base::trunc;

		bool check_header(std::istream& is, string const& header);
		string read_header(std::istream& is);
		std::ostream& write_header(std::ostream& os, string const& header);

		unsigned read_ber(std::istream& is);
		std::ostream& write_ber(std::ostream& os, unsigned val);

		std::istream& read_with_size(std::istream& is, binary& dst);
		template<class T>
		std::ostream& write_with_size(std::ostream& os, T const& val)
		{
			return val.serialize(write_ber(os, val.serialized_size()));
		}

		bool is_eof(std::istream& is);
	} // namespace structure
} // namespace rpg2k

#endif // _INC_RPG2K__STREAM_HXX_
