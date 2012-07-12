#ifndef _INC_RPG2K__DESCRIPTOR_HXX_
#define _INC_RPG2K__DESCRIPTOR_HXX_

#include "define.hxx"
#include "singleton.hxx"

#include <boost/bimap.hpp>
#include <boost/noncopyable.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>


namespace rpg2k
{
	class binary;

	namespace structure
	{
		class array1d;
		class array2d;
		class ber_enum;
		class element;
		class event;

    typedef std::vector<int32_t> int32_array;
    typedef std::vector<int16_t> int16_array;
    typedef std::vector<int8_t> int8_array;

		class descriptor;
		typedef boost::ptr_unordered_map<unsigned, descriptor> array_define_type;
		typedef boost::unordered_map<string, int> array_table_type;

#define PP_basic_types \
	(int) \
	(bool) \
	(double) \

#define PP_rpg2k_types \
	(array1d) \
	(array2d) \
	(ber_enum) \
	(binary) \
	(event) \
	(string) \

#define PP_array_types \
  (int32_array) \
	(int16_array) \
	(int8_array) \

#define PP_all_types PP_basic_types PP_rpg2k_types PP_array_types

		class element_type : public singleton<element_type const>
		{
			friend class singleton<element_type const>;
		public:
#define PP_enum(r, data, elem) BOOST_PP_CAT(elem, data),
			enum type { BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_all_types) };
#undef PP_enum
			type to_enum(string const& name) const;
			string const& to_string(type e) const;
		private:
			element_type();

			typedef boost::bimap<type, string> table;
			table table_;
		}; // class element_type

		class descriptor
		{
		public:
			descriptor(descriptor const& src);

			descriptor(element_type::type t);
			descriptor(element_type::type t, string const& val);
			descriptor(element_type::type t
			, unique_ptr<array_define_type>::type def
			, unique_ptr<array_table_type>::type table);

			~descriptor();

#define PP_cast_operator(r, data, elem) operator elem const&() const;
			BOOST_PP_SEQ_FOR_EACH(PP_cast_operator, , PP_basic_types)
#undef PP_cast_operator
			operator string const&() const;
			operator unsigned const&() const
			{
				return reinterpret_cast<unsigned const&>(this->operator int const&());
			}
			array_define_type const& array_define() const;
			array_table_type const& array_table() const { return *array_table_; }
      string const& index_to_name(int idx) const;

			string const& type_name() const;
			element_type::type const type;

			bool has_default() const { return has_default_; }
		private:
			bool const has_default_;
			union {
#define PP_enum(r, data, elem) elem BOOST_PP_CAT(elem, data);
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum
				string const* string_;
				boost::ptr_unordered_map<unsigned, descriptor>* array_define;
			} impl_;

			boost::scoped_ptr<array_table_type> const array_table_;
		}; // class descriptor
	} // namespace structure
} // namespace rpg2k

#endif // _INC_RPG2K__DESCRIPTOR_HXX_
