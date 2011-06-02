#ifndef _INC_RPG2K__COMMAND_TABLE_HXX_
#define _INC_RPG2K__COMMAND_TABLE_HXX_

#include "context.hxx"
#include "singleton.hxx"

#include <boost/unordered_map.hpp>


namespace rpg2k
{
	namespace model
	{
		class command_table : public singleton<command_table const>
		{
			friend class singleton<command_table const>;
		protected:
			command_table();
		public:
			context::command_type operator [](unsigned code) const;
		private:
			typedef boost::unordered_map<unsigned, context::command_type> table_type;
			table_type table_;
		}; // class command_table
	} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__COMMAND_TABLE_HXX_
