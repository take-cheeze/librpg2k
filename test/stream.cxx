#include <boost/array.hpp>
#include <boost/range/irange.hpp>

#include <gtest/gtest.h>
#include <rpg2k/stream.hxx>

#include <algorithm>
#include <iostream>
#include <iterator>

#include <ctime>
#include <cstdlib>


namespace io = boost::iostreams;

TEST(BER, berSize)
{
	ASSERT_EQ(rpg2k::stream::ber_size(0), 1);
	ASSERT_EQ(rpg2k::stream::ber_size(128), 2);
	ASSERT_EQ(rpg2k::stream::ber_size(16384), 3);
}

TEST(BER, ReadWrite)
{
	std::srand(std::time(NULL));

	boost::array<uint8_t, (sizeof(unsigned) * 8) / rpg2k::stream::BER_BIT + 1> buf;
	BOOST_FOREACH(unsigned const& i, boost::irange(0, 10)) {
		unsigned const target = std::rand();
    (void)i;

		io::stream<io::array_sink> os(io::array_sink(reinterpret_cast<char*>(buf.data()), buf.size()));
		rpg2k::stream::write_ber(os, target);

		io::stream<io::array_source> is(io::array_source(reinterpret_cast<char const*>(buf.data()), buf.size()));
		ASSERT_EQ(target, rpg2k::stream::read_ber(is));
	}
}

