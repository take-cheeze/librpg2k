#ifndef _INC__RPG2K__STREAM_HPP
#define _INC__RPG2K__STREAM_HPP

#include "structure.hxx"

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>

#include <iosfwd>


namespace rpg2k
{
	namespace structure
	{
		namespace io = boost::iostreams;

		template<class T>
		Binary serialize(T const& src)
		{
			Binary ret(src.serializedSize());
			io::stream<io::array_sink> s(ret.sink());
			src.serialize(s);

			return ret;
		}
	}

	namespace stream
	{
		enum {
			BER_BIT  = (CHAR_BIT-1),
			BER_SIGN = 0x01 << BER_BIT,
			BER_MASK = BER_SIGN - 1,
		};
		unsigned berSize(unsigned num);

		std::ios_base::openmode const INPUT_FLAG
		= std::ios_base::in  | std::ios_base::binary;
		std::ios_base::openmode const OUTPUT_FLAG
		= std::ios_base::out | std::ios_base::binary | std::ios_base::trunc;

		bool checkHeader(std::istream& is, String const& header);
		String readHeader(std::istream& is);
		std::ostream& writeHeader(std::ostream& os, String const& header);

		unsigned readBER(std::istream& is);
		std::ostream& writeBER(std::ostream& os, unsigned val);

		std::istream& readWithSize(std::istream& is, Binary& dst);
		template<class T>
		std::ostream& writeWithSize(std::ostream& os, T const& val)
		{
			return val.serialize(writeBER(os, val.serializedSize()));
		}

		bool isEOF(std::istream& is);

		struct istream_range_source : public boost::iostreams::source {
			istream_range_source(std::istream& is, std::streamsize const s)
				: stream_(is), size_(s)
			{
			}
			istream_range_source(istream_range_source const& src)
				: stream_(src.stream_), size_(src.size_) {}
			std::streamsize read(char* o, std::streamsize const n)
			{
				std::streamsize const result = std::min(n, size_);
				size_ -= result;
				return (result != 0)? (stream_.read(o, result), result) : EOF;
			}
		private:
			std::istream& stream_;
			std::streamsize size_;
		};
	} // namespace structure
} // namespace rpg2k

#endif
