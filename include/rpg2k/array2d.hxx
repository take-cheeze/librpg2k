#ifndef _INC_RPG2K__ARRAY2D_HXX_
#define _INC_RPG2K__ARRAY2D_HXX_

#include <boost/ptr_container/ptr_map.hpp>
#include "descriptor.hxx"


namespace rpg2k
{
	class binary;

	namespace structure
	{
		class array1d;
		class descriptor;
		class element;

		typedef boost::ptr_multimap<unsigned, array1d> base_of_array2d;

		class array2d : public base_of_array2d
		{
		private:
			element* const this_;
		protected:
			array2d();
			void init(std::istream& f);
			bool is_invalid_array2d(binary const& b);
		public:
			array2d(array2d const& src);

			array2d(array_define_type const& info);
			array2d(array_define_type const& info, std::istream& s);
			array2d(array_define_type const& info, binary const& b);

			array2d(element& e);
			array2d(element& e, std::istream& s);
			array2d(element& e, binary const& b);

			array2d const& operator =(array2d const& src);

			array1d& operator [](unsigned index);
			array1d const& operator [](unsigned index) const;

			bool exists(unsigned index) const;
			bool exists(unsigned index1, unsigned index2) const;

			array_define_type const& array_define;

			bool is_element() const { return this_ != NULL; }
			element& to_element() const;

			unsigned count() const;
			size_t serialized_size() const;
			std::ostream& serialize(std::ostream& s) const;
		}; // class array2d
	} // namespace structure
} // namespace rpg2k

#endif // _INC_RPG2K__ARRAY2D_HXX_
