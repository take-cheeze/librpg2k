#ifndef _INC_RPG2K__CONTEXT_HXX_
#define _INC_RPG2K__CONTEXT_HXX_

#include <stack>

#include "define.hxx"


namespace rpg2k
{
	namespace structure
	{
		class event;
		class instruction;
	}

	namespace model
	{
		class project;

		class waiter
		{
		public:
			waiter() : count_(0) {}
			void update() { --count_; }
		private:
			size_t count_;
		};

		class context
		{
		public:
			context(project& p, unsigned ev_id, event_start::type t);

			typedef void (context::*command_type)(structure::instruction const&);
			template<unsigned Code>
			void command(structure::instruction const& inst);
		protected:
			void ret(); // return

		private:
			project& owner_;
			unsigned const event_id_;
			event_start::type const type_;
			waiter waiter_;
			struct event_stream
			{
				structure::event const* event;
				size_t pointer;
			};
			std::stack<event_stream> event_;
			struct loop_state
			{
				size_t nest, pointer;
			};
			std::stack<loop_state> loop_;
		}; // class context
	} // namespace model
} // namespace rpg2k

#endif // _INC_RPG2K__CONTEXT_HXX_
