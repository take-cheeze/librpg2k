#include "rpg2k/context.hxx"
#include "rpg2k/project.hxx"


namespace rpg2k {
	namespace model {
		context::context(project& p, unsigned ev_id, event_start::type t)
		: owner_(p)
		, event_id_(ev_id), type_(t)
		{}
		void context::ret() 
		{
			event_.pop();
		}
	} // namespace model
} // namespace rpg2k
