#include <boost/array.hpp>
#include <gtest/gtest.h>
#include <rpg2k/Stream.hxx>

#include <algorithm>
#include <iostream>
#include <iterator>

#include <ctime>
#include <cstdlib>


namespace io = boost::iostreams;

TEST(BER, berSize)
{
	ASSERT_EQ(rpg2k::stream::berSize(0), 1);
	ASSERT_EQ(rpg2k::stream::berSize(128), 2);
	ASSERT_EQ(rpg2k::stream::berSize(16384), 3);
}

TEST(BER, ReadWrite)
{
	std::srand(std::time(NULL));

	boost::array<uint8_t, (sizeof(unsigned) * CHAR_BIT) / rpg2k::stream::BER_BIT + 1> buff;
	for(unsigned i = 0; i < 10; ++i) {
		unsigned const target = std::rand();

		io::stream<io::array_sink> os(io::array_sink(reinterpret_cast<char*>(buff.data()), buff.size()));
		rpg2k::stream::writeBER(os, target);

		/*
		std::cout << target << ": ";
		std::copy(buff.begin(), buff.begin() + rpg2k::stream::berSize(target), std::ostream_iterator<unsigned>(std::cout, ", "));
		std::cout << std::endl;
		*/

		io::stream<io::array_source> is(io::array_source(reinterpret_cast<char const*>(buff.data()), buff.size()));
		ASSERT_EQ(target, rpg2k::stream::readBER(is));
	}
}

