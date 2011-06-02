#ifndef _INC_RPG2K__EVENT_HXX_
#define _INC_RPG2K__EVENT_HXX_

#include "define.hxx"

#include <deque>
#include <map>
#include <memory>
#include <vector>


namespace rpg2k
{
	class binary;

	namespace structure
	{
		class instruction : public std::vector<int32_t>
		{
		public:
			instruction();
			instruction(std::istream& s);

			unsigned code, nest;
			string string_argument;

			size_t serialized_size() const;
			std::ostream& serialize(std::ostream& s) const;
		}; // class instruction

		class element;
		class descriptor;

		class event : public std::deque<instruction>
		{
		protected:
			void init(std::istream& s);
		public:
			event() {}
			explicit event(binary const& b);

			void resize(unsigned num);

			size_t serialized_size(unsigned offset = 0) const;
			std::ostream& serialize(std::ostream& s) const;

			typedef std::map<unsigned, unsigned> label_table_type;
			label_table_type label;
		}; // class event
	} // namespace structure
} //o namespace rpg2k

#endif // _INC_RPG2K__EVENT_HXX_
