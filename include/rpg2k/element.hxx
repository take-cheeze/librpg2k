#ifndef _INC_RPG2K__ELEMENT_HXX_
#define _INC_RPG2K__ELEMENT_HXX_

#include <vector>

#include "descriptor.hxx"
#include "structure.hxx"


namespace picojson {
  class value;
} // namespace picojson

namespace rpg2k
{
	namespace structure
	{
		class event_state;
		class music;
		class sound;

		class element
		{
		private:
			static int const NOT_ARRAY = -1;
			descriptor const* const descriptor_;

			bool exists_;

			element* const owner_;
			int const index_of_array1d_;
			int const index_of_array2d_;

			union {
#define PP_types(r, data, elem) elem* BOOST_PP_CAT(elem, data);
				BOOST_PP_SEQ_FOR_EACH(PP_types, _, PP_rpg2k_types PP_array_types)
#undef PP_types
#define PP_types(r, data, elem) elem BOOST_PP_CAT(elem, data);
				BOOST_PP_SEQ_FOR_EACH(PP_types, _, PP_basic_types)
#undef PP_types
			} impl_;

			void init();
			void init(binary const& b);
			void init(std::istream& s);
		public:
			element();
			element(element const& e);

			element(descriptor const& info);
			element(descriptor const& info, std::istream& s);
			element(descriptor const& info, binary const& b);

			element(array1d& owner, unsigned index);
			element(array1d& owner, unsigned index , binary const& b);

			~element();

			bool is_defined() const { return descriptor_ != NULL; }
			descriptor const& definition() const;

			bool has_owner() const { return owner_ != NULL; }
			element& owner();
			element const& owner() const;

			bool exists() const { return exists_; }
			void substantiate();

			size_t serialized_size() const;
			std::ostream& serialize(std::ostream& s) const;
			binary serialize() const;

#if RPG2K_IS_CLANG
			template<class T>
			T& to() { return this->operator T&(); }
			template<class T>
			T const& to() const { return this->operator T const&(); }
#else
			template<class T>
			T& to() { return static_cast<T&>(*this); }
			template<class T>
			T const& to() const { return static_cast<T const&>(*this); }
#endif

#define PP_cast_operator(r, data, elem) \
	operator elem const&() const; \
	operator elem&(); \
	elem& BOOST_PP_CAT(to_, elem)() { return to<elem>(); } \
	elem const& BOOST_PP_CAT(to_, elem)() const { return to<elem>(); }
			BOOST_PP_SEQ_FOR_EACH(PP_cast_operator, , PP_all_types)
#undef PP_cast_operator

#define PP_cast_operator(r, data, elem) \
	operator elem const&() const { return reinterpret_cast<elem const&>(to_array1d()); } \
	operator elem&() { return reinterpret_cast<elem&>(to_array1d()); } \
	elem& BOOST_PP_CAT(to_, elem)() { return to<elem>(); } \
	elem const& BOOST_PP_CAT(to_, elem)() const { return to<elem>(); }
			BOOST_PP_SEQ_FOR_EACH(PP_cast_operator, , (music)(sound)(event_state))
#undef PP_cast_operator

			operator unsigned&() { return reinterpret_cast<unsigned&>(to<int>()); }
			operator unsigned const&() const { return reinterpret_cast<unsigned const&>(to<int>()); }

			template<typename T>
			void assign(T const& src)
			{
				this->substantiate();
				static_cast<T&>(*this) = src;
			}
      void assign(picojson::value const& v);

			template<typename T>
			T const& operator =(T const& src) { this->assign(src); return static_cast<T&>(*this); }
			element& operator =(element const& src);

			unsigned index_of_array1d() const {
				rpg2k_assert(index_of_array1d_ != NOT_ARRAY);
				return index_of_array1d_;
			}
			unsigned index_of_array2d() const {
				rpg2k_assert(index_of_array2d_ != NOT_ARRAY);
				return index_of_array2d_;
			}

      picojson::value to_json() const;
    }; // class element

		template<> inline
		unsigned const& element::operator =(unsigned const& num) { (*this) = int(num); return *this; }
		template<> inline
		unsigned const& element::to<unsigned>() const { return reinterpret_cast< unsigned const& >(to<int>()); }
		template<> inline
		unsigned& element::to<unsigned>() { return reinterpret_cast< unsigned& >(to<int>()); }

		class ber_enum : public std::vector<unsigned>
		{
		protected:
			void init(std::istream& s);
		public:
			ber_enum() {}
			ber_enum(std::istream& s);
			ber_enum(binary const& b);

			size_t serialized_size() const;
			std::ostream& serialize(std::ostream& s) const;
		}; // class ber_enum
	} // namespace structure
} // namespace rpg2k

#endif // _INC_RPG2K__ELEMENT_HXX_
