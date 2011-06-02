#include <gtest/gtest.h>
#include <rpg2k/encode.hxx>

using rpg2k::encode;


TEST(Encode, Open)
{
	ASSERT_NO_THROW(encode::instance());
}
