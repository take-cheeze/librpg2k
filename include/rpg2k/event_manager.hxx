#ifndef _INC_RPG2K__EVENT_MANAGER_HXX_
#define _INC_RPG2K__EVENT_MANAGER_HXX_

#include "context.hxx"
#include "define.hxx"
#include "node.hxx"

#include <boost/ptr_container/ptr_map.hpp>


namespace rpg2k
{
	namespace model
	{
		class project;

		class event_manager : public node
		{
		public:
			event_manager(project& proj);
		private:
			project& project_;

			size_t command_count_;

			typedef boost::ptr_multimap<event_start::type, context, std::greater<event_start::type> > context_list;
			context_list context_list_;

			void update_this();
		};
	} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__EVENT_MANAGER_HXX_
