#include "rpg2k/array1d.hxx"
#include "rpg2k/array2d.hxx"
#include "rpg2k/debug.hxx"
#include "rpg2k/descriptor.hxx"
#include "rpg2k/element.hxx"
#include "rpg2k/stream.hxx"

#include <boost/foreach.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm/count_if.hpp>


namespace rpg2k
{
	namespace structure
	{
		array2d::array2d(array2d const& src)
		: base_of_array2d(), this_(src.this_)
		, array_define(src.array_define)
		{
			binary const b = structure::serialize(src);
			io::stream<io::array_source> s(b.source());
			this->init(s);
		}

		array2d::array2d(array_define_type const& info)
		: this_(NULL), array_define(info)
		{}
		array2d::array2d(array_define_type const& info, std::istream& s)
		: this_(NULL), array_define(info)
		{
			init(s);
		}
		array2d::array2d(array_define_type const& info, binary const& b)
		: this_(NULL), array_define(info)
		{
			io::stream<io::array_source> s(b.source());

			if(is_invalid_array2d(b)) { return; }
			else { this->init(s); }
		}

		array2d::array2d(element& e)
			: this_(&e), array_define(e.definition().array_define())
		{}
		array2d::array2d(element& e, std::istream& s)
		: this_(&e), array_define(e.definition().array_define())
		{
			init(s);
		}
		array2d::array2d(element& e, binary const& b)
		: this_(&e), array_define(e.definition().array_define())
		{
			io::stream<io::array_source> s(b.source());

			if(is_invalid_array2d(b)) return;
			else init(s);
		}
		void array2d::init(std::istream& s)
		{
			BOOST_FOREACH(unsigned i, boost::irange(0u, stream::read_ber(s))) {
				(void)i;
				unsigned index = stream::read_ber(s);
				this->insert(index, new array1d(*this, index, s));
			}

			if(to_element().has_owner()) rpg2k_analyze_assert(stream::is_eof(s));
		}

	/*
	 *  Checking if the input binary is particular array2d.
	 *  Particular array2d is a binary data that starts with 512 byte data of
	 * all value that is 0xff.
	 *  _notice: Particular array2d's size is not 512 byte. I misunderstood it.
	 *  http://twitter.com/rgssws4m told me about this case.
	 */
		bool array2d::is_invalid_array2d(binary const& b)
		{
			enum { PARTICULAR_DATA_SIZE = 512 };
		// check the data size
			if(b.size() < PARTICULAR_DATA_SIZE) return false;
		// check the data inside binary
		// return true if it is particular array2d
			return (boost::count(b, 0xff) < PARTICULAR_DATA_SIZE)
				? false
				: debug::tracer::print_binary(b, clog), true;
		}

		element& array2d::to_element() const
		{
			rpg2k_assert(is_element());
			return *this_;
		}

		array2d const& array2d::operator =(array2d const& src)
		{
			base_of_array2d::operator =(src);
			return *this;
		}

		array1d& array2d::operator [](unsigned index)
		{
			iterator it = this->find(index);
			if(it != this->end()) return *it->second;
			else {
				return *insert(index, new array1d(*this, index))->second;
			}
		}
		array1d const& array2d::operator [](unsigned const index) const
		{
			const_iterator it = this->find(index);
			rpg2k_assert(it != this->end());
			return *it->second;
		}

		unsigned array2d::count() const
		{
			unsigned ret = 0;
			for(const_iterator i = begin(); i != end(); ++i) {
				if(i->second->exists()) ++ret;
			}
			return ret;
		}
		size_t array2d::serialized_size() const
		{
			unsigned ret = 0;

			ret += stream::ber_size(count());
			for(const_iterator i = this->begin(); i != this->end(); ++i) {
				if(!i->second->exists()) continue;

				ret += stream::ber_size(i->first);
				ret += i->second->serialized_size();
			}

			return ret;
		}
		std::ostream& array2d::serialize(std::ostream& s) const
		{
			stream::write_ber(s, count());
			for(const_iterator i = this->begin(); i != this->end(); ++i) {
				if(!i->second->exists()) continue;

				stream::write_ber(s, i->first);
				i->second->serialize(s);
			}
			return s;
		}

		bool array2d::exists(unsigned const index) const
		{
			const_iterator it = find(index);
			return((it != end()) && it->second->exists());
		}
		bool array2d::exists(unsigned index1, unsigned index2) const
		{
			array2d::const_iterator it1 = find(index1);
			if((it1 != end()) && it1->second->exists()) {
				array1d::const_iterator it2 = it1->second->find(index2);
				return((it2 != it1->second->end()) && it2->second->exists());
			}
			return false;
		}
	} // namespace structure
} // namespace rpg2k
