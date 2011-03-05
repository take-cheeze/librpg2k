#include <gtest/gtest.h>
#include <rpg2k/Descriptor.hxx>


TEST(ElementType, Compare)
{
	using rpg2k::structure::ElementType;

	#define PP_enum(VAL) \
		ASSERT_EQ( ElementType::VAL##_, ElementType::instance().toEnum(#VAL) ); \
		ASSERT_EQ( #VAL, ElementType::instance().toString(ElementType::VAL##_) );
	PP_allTypes(PP_enum)
	#undef PP_enum
}
