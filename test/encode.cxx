#include <gtest/gtest.h>
#include <rpg2k/encode.hxx>

using rpg2k::Encode;


TEST(Encode, Open)
{
	ASSERT_NO_THROW(Encode::instance());
}
