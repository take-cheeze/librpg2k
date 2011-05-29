#include <boost/preprocessor/stringize.hpp>
#include <gtest/gtest.h>
#include <rpg2k/descriptor.hxx>


TEST(ElementType, Compare)
{
	using rpg2k::structure::ElementType;

	#define PP_enum(r, data, elem) \
		ASSERT_EQ( ElementType::BOOST_PP_CAT(elem, data), ElementType::instance().toEnum(BOOST_PP_STRINGIZE(elem)) ); \
		ASSERT_EQ( BOOST_PP_STRINGIZE(elem), ElementType::instance().toString(ElementType::BOOST_PP_CAT(elem, data)) );
	BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_allTypes)
	#undef PP_enum
}
