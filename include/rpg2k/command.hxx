#ifndef _INC_RPG2K__COMMAND_HXX_
#define _INC_RPG2K__COMMAND_HXX_

#include "context.hxx"


namespace rpg2k
{
	namespace model
	{
		#define PP_all_context_command \
			(0) \
			(10) \
			\
			(11210) \
			\
			(11310) \
			(11320) \
			(11330) \

#define PP_context_command_proto(r, data, elem) \
	template<> void context::command<elem>(structure::instruction const& inst);
		BOOST_PP_SEQ_FOR_EACH(PP_context_command_proto, , PP_all_context_command)
#undef PP_context_command_proto
	} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__COMMAND_HXX_
