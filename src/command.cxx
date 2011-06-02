#include "rpg2k/command.hxx"


namespace rpg2k
{
	namespace model
	{
		#define PP_context_command_proto(CODE) \
			template<> void context::command<CODE>(structure::instruction const&)

		PP_context_command_proto(0) // event terminater
		{
			this->ret();
		}
		PP_context_command_proto(10) // branch terminater
		{
		}

		PP_context_command_proto(11210) // render battle animation on field
		{
			// TODO
		}

		PP_context_command_proto(11310) // change party's transparancy state
		{
			// TODO
		}
		PP_context_command_proto(11320) // flash effect to event
		{
			// TODO
		}
		PP_context_command_proto(11330) // set event action
		{
			// TODO
		}

		#undef PP_context_command_proto
	} // namespace model
} // namespace rpg2k
