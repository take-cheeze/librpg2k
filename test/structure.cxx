#include <boost/preprocessor/stringize.hpp>
#include <gtest/gtest.h>
#include <rpg2k/descriptor.hxx>


TEST(ElementType, Compare)
{
	using rpg2k::structure::element_type;
	element_type const& elem_type = element_type::instance();

	#define PP_enum(r, data, elem) \
		ASSERT_EQ(element_type::BOOST_PP_CAT(elem, data), elem_type.to_enum(BOOST_PP_STRINGIZE(elem))); \
		ASSERT_EQ(BOOST_PP_STRINGIZE(elem), elem_type.to_string(element_type::BOOST_PP_CAT(elem, data)));
	BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_all_types)
	#undef PP_enum
}
