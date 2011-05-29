#include "rpg2k/context.hxx"
#include "rpg2k/project.hxx"


namespace rpg2k
{
	namespace model
	{
		Context::Context(Project& p, unsigned evID, EventStart t)
		: owner_(p)
		, eventID_(evID), type_(t)
		{
		}
		void Context::ret() 
		{
			event_.pop();
		}
	} // namespace model
} // namespace rpg2k
