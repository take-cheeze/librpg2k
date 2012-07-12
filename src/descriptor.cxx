#include "rpg2k/debug.hxx"
#include "rpg2k/descriptor.hxx"
#include "rpg2k/model.hxx"
#include "rpg2k/stream.hxx"
#include "rpg2k/structure.hxx"

#include <boost/bind.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/range/algorithm/find_if.hpp>


namespace rpg2k
{
	namespace structure
	{
		element_type::element_type()
		{
#define PP_insert(r, data, elem) \
	table_.insert(table::value_type(BOOST_PP_CAT(elem, data), BOOST_PP_STRINGIZE(elem)));
			BOOST_PP_SEQ_FOR_EACH(PP_insert, _, PP_all_types)
#undef PP_insert
		}
		element_type::type element_type::to_enum(string const& name) const
		{
			table::right_map::const_iterator it = table_.right.find(name);
			if(it != table_.right.end()) { return it->second; }
      else {
        if(model::define_loader::instance().is_array(name)) {
          return array1d_;
        } else {
          assert(false);
          return binary_;
        }
      }
		}
		string const& element_type::to_string(element_type::type const e) const
		{
			table::left_map::const_iterator it = table_.left.find(e);
			rpg2k_assert(it != table_.left.end());
			return it->second;
		}

		descriptor::descriptor(descriptor const& src)
		: type(src.type), has_default_(src.has_default_)
		, array_table_(src.array_table_.get() ? new array_table_type(*src.array_table_) : NULL)
		{
			if(has_default_) switch(type) {
#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = src.impl_.BOOST_PP_CAT(elem, data); \
		return;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum
				case element_type::array1d_:
				case element_type::array2d_:
					impl_.array_define
					= new boost::ptr_unordered_map<unsigned, descriptor>(*src.impl_.array_define);
					break;
				default: rpg2k_assert(false); break;
			}
		}
		descriptor::descriptor(element_type::type const t)
		: type(t), has_default_(false)
		{}
		descriptor::descriptor(element_type::type const t, string const& val)
		: type(t), has_default_(true)
		{
			switch(this->type) {
				case element_type::string_:
					if(
						(val.size() > 2) &&
						(*val.begin() == '\"') && (*val.rbegin() == '\"')
					) impl_.string_ = new string(val.data() + 1, val.size() - 2);
					else impl_.string_ = new string(val);
					break;
#define PP_enum(r, d, elem) \
	case element_type::BOOST_PP_CAT(elem, d): { \
		io::stream<io::array_source> s(io::array_source(val.data(), val.size())); \
		s >> std::boolalpha >> (impl_.BOOST_PP_CAT(elem, d)); \
	} break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum
				default: rpg2k_assert(false); break;
			}
		}
		descriptor::descriptor(element_type::type const t
		, unique_ptr<array_define_type>::type def
		, unique_ptr<array_table_type>::type table)
		: type(t), has_default_(true)
		, array_table_(table.release())
		{
			rpg2k_assert((type == element_type::array1d_) || (type == element_type::array2d_));
			impl_.array_define = def.release();
		}

		descriptor::~descriptor()
		{
			if(has_default_) switch(type) {
				case element_type::string_:
					delete impl_.string_;
					break;
				case element_type::array1d_:
				case element_type::array2d_:
					delete impl_.array_define;
					break;
#define PP_enum(r, data, elem) \
					case element_type::BOOST_PP_CAT(elem, data):
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum
					break;
				default: rpg2k_assert(false); break;
			}
		}

#define PP_cast_operator(r, data, elem) \
	descriptor::operator elem const&() const \
	{ \
		rpg2k_assert(this->type == element_type::BOOST_PP_CAT(elem, data)); \
		return impl_.BOOST_PP_CAT(elem, data); \
	}
		BOOST_PP_SEQ_FOR_EACH(PP_cast_operator, _, PP_basic_types)
#undef PP_cast_operator

		descriptor::operator string const&() const
		{
			rpg2k_assert(this->type == element_type::string_);
			return *impl_.string_;
		}
		array_define_type const& descriptor::array_define() const
		{
			rpg2k_assert((this->type == element_type::array1d_)
			|| (this->type == element_type::array2d_));
			return *impl_.array_define;
		}

		string const& descriptor::type_name() const
		{
			return element_type::instance().to_string(type);
		}

    static bool is_value(std::pair<string, int> const& p, int v) {
      return(p.second == v);
    }

    string const& descriptor::index_to_name(int idx) const {
      array_table_type::const_iterator it =
        boost::find_if(array_table(), boost::bind(is_value, _1, idx));
      rpg2k_assert(it != array_table().end());
      return it->first;
    }
  } // namespace structure
} // namespace rpg2k
