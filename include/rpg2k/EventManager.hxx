#ifndef _INC_RPG2K__EVENT_MANAGER_HXX_
#define _INC_RPG2K__EVENT_MANAGER_HXX_

#include "Context.hxx"
#include "Define.hxx"
#include "Node.hxx"

#include <boost/ptr_container/ptr_map.hpp>


namespace rpg2k
{
	namespace model
	{
		class Project;

		class EventManager : public Node
		{
		public:
			EventManager(Project& proj);
		private:
			Project& project_;

			size_t commandCount_;

			typedef boost::ptr_multimap<EventStart, Context, std::greater<EventStart> > ContextList;
			ContextList contextList_;

			void updateThis();
		};
	} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__EVENT_MANAGER_HXX_
