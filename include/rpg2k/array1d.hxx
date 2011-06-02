#ifndef _INC_RPG2K__ARRAY1D_HXX_
#define _INC_RPG2K__ARRAY1D_HXX_

#include <map>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include "descriptor.hxx"
#include "structure.hxx"


namespace rpg2k
{
	namespace structure
	{
		class array2d;
		class element;

		typedef boost::ptr_unordered_map<unsigned, element> base_of_array1d;

		class array1d : public base_of_array1d
		{
		private:
			element* const this_;

			bool exists_;
			array2d* const owner_;
			unsigned const index_;
		protected:
			array1d();

			void init(std::istream& s);
		public:
			enum { END_OF_ARRAY1D = 0, };

			array1d(array1d const& array);

			array1d(element& e);
			array1d(element& e, std::istream& s);
			array1d(element& e, binary const& b);

			array1d(array2d& owner, unsigned index);
			array1d(array2d& owner, unsigned index, std::istream& f);

			array1d const& operator =(array1d const& src);

			bool is_array2d() const { return owner_ != NULL; }
			unsigned const& index() const;
			array2d& owner();
			array2d const& owner() const;

			element& operator [](unsigned index);
			element const& operator [](unsigned index) const;

			element& operator [](char const* index);
			element const& operator [](char const* index) const;

			bool exists() const;
			bool exists(unsigned index) const;

			unsigned count() const;
			size_t serialized_size() const;
			std::ostream& serialize(std::ostream& s) const;

			void substantiate();

			array_define_type const& array_define;

			bool is_element() const;
			element& to_element();
			element const& to_element() const;

			static bool sort_function(element const* l, element const* r);
		}; // class array1d
	} // namespace structure
} // namespace rpg2k

#endif //_INC_RPG2K__ARRAY1D_HXX_
