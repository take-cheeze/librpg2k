#include <utility>

#include "rpg2k/command.hxx"
#include "rpg2k/command_table.hxx"


namespace rpg2k
{
	namespace model
	{
		command_table::command_table()
		{
#define PP_insert(r, data, elem) \
	table_.insert(table_type::value_type(elem, &context::command<elem>));
			BOOST_PP_SEQ_FOR_EACH(PP_insert, , PP_all_context_command)
#undef PP_insert
		}

		context::command_type command_table::operator [](unsigned const code) const
		{
			table_type::const_iterator i = table_.find(code);
			rpg2k_assert(i != table_.end());
			return i->second;
		}
	} // namespace model
} // namespace rpg2k
