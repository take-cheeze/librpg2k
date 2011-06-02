#include "rpg2k/event.hxx"
#include "rpg2k/stream.hxx"

#include <boost/foreach.hpp>
#include <boost/range/irange.hpp>


namespace rpg2k {
	namespace structure {
		instruction::instruction()
		: code(-1)
		{}
		instruction::instruction(std::istream& s)
		{
			code = stream::read_ber(s);
			nest = stream::read_ber(s);

			binary b;
			stream::read_with_size(s, b);
			string_argument.assign(reinterpret_cast<char const*>(b.data()), b.size());

			int const arg_num = stream::read_ber(s);

			this->resize(arg_num, VARIABLE_DEFAULT);
			BOOST_FOREACH(int32_t& i, *this) {
				i = stream::read_ber(s);
			}
		}

		size_t instruction::serialized_size() const
		{
			unsigned ret =
				stream::ber_size(code) + stream::ber_size(nest) +
				stream::ber_size(string_argument.size()) +
				string_argument.size() +
				stream::ber_size(this->size());
			BOOST_FOREACH(int32_t const& i, *this) {
				ret += stream::ber_size(i);
			}
			return ret;
		}
		std::ostream& instruction::serialize(std::ostream& s) const
		{
			stream::write_ber(s, code);
			stream::write_ber(s, nest);
			stream::write_with_size(s, string_argument);
			stream::write_ber(s, this->size());
			BOOST_FOREACH(int32_t const& i, *this) {
				stream::write_ber(s, i);
			}
			return s;
		}

		event::event(binary const& b)
		{
			io::stream<io::array_source> s(b.source());
			init(s);
		}

		void event::init(std::istream& s)
		{
			while(!stream::is_eof(s)) {
				this->push_back(instruction(s));
				if(this->back().code == 12110) { // check for label
					if(!label.insert(std::make_pair(this->back()[0], this->size() - 1)).second) {
						rpg2k_assert(false);
					}
				}
			}
		}

		size_t event::serialized_size(unsigned const offset) const
		{
			unsigned ret = 0;
			BOOST_FOREACH(const_iterator i, boost::irange(this->begin() + offset, this->end())) {
				ret += i->serialized_size();
			}
			return ret;
		}
		std::ostream& event::serialize(std::ostream& s) const
		{
			BOOST_FOREACH(value_type const& i, *this) { i.serialize(s); }
			return s;
		}
	} // namespace structure
} // namespace rpg2k
