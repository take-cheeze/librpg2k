#ifndef _INC__RPG2K__MODEL__ELEMENT_HPP
#define _INC__RPG2K__MODEL__ELEMENT_HPP

#include <vector>

#include "Descriptor.hxx"
#include "Structure.hxx"


namespace rpg2k
{
	namespace structure
	{
		class Element
		{
		private:
			enum { NOT_ARRAY = -1, };
			Descriptor const* const descriptor_;

			bool exists_;

			Element* const owner_;
			int const indexOfArray1D_;
			int const indexOfArray2D_;

			union {
				#define PP_types(r, data, elem) elem* BOOST_PP_CAT(elem, _);
				BOOST_PP_SEQ_FOR_EACH(PP_types, , PP_rpg2kTypes)
				#undef PP_types
				#define PP_types(r, data, elem) elem BOOST_PP_CAT(elem, _);
				BOOST_PP_SEQ_FOR_EACH(PP_types, , PP_basicTypes)
				#undef PP_types
			} impl_;

			void init();
			void init(Binary const& b);
			void init(std::istream& s);
		public:
			Element();
			Element(Element const& e);
			Element(Descriptor const& info);
			Element(Descriptor const& info, Binary const& b);
			Element(Descriptor const& info, std::istream& s);

			Element(Array1D const& owner, unsigned index);
			Element(Array1D const& owner, unsigned index , Binary const& b);

			bool isDefined() const { return descriptor_ != NULL; }
			bool hasOwner() const { return owner_ != NULL; }
			bool exists() const { return exists_; }

			~Element();

			Descriptor const& descriptor() const;

			size_t serializedSize() const;
			std::ostream& serialize(std::ostream& s) const;
			Binary serialize() const;

			#define PP_castOperator(r, data, elem) \
				operator elem const&() const; \
				operator elem&(); \
				elem& BOOST_PP_CAT(to_, elem)() { return to<elem>(); } \
				elem const& BOOST_PP_CAT(to_, elem)() const { return to<elem>(); }
			BOOST_PP_SEQ_FOR_EACH(PP_castOperator, , PP_basicTypes)
			#undef PP_castOperator

			#define PP_castOperator(r, data, elem) \
				operator elem const&() const; \
				operator elem&(); \
				elem& BOOST_PP_CAT(to, elem)() { return to<elem>(); } \
				elem const& BOOST_PP_CAT(to, elem)() const { return to<elem>(); }
			BOOST_PP_SEQ_FOR_EACH(PP_castOperator, , PP_rpg2kTypes)
			#undef PP_castOperator

			#define PP_castOperator(r, data, elem) \
				operator elem const&() const { return reinterpret_cast<elem const&>(toArray1D()); } \
				operator elem&() { return reinterpret_cast<elem&>(toArray1D()); } \
				elem& BOOST_PP_CAT(to, elem)() { return to<elem>(); } \
				elem const& BOOST_PP_CAT(to, elem)() const { return to<elem>(); }
			BOOST_PP_SEQ_FOR_EACH(PP_castOperator, , (Music)(Sound)(EventState))
			#undef PP_castOperator

			operator unsigned&() { return reinterpret_cast<unsigned&>(to<int>()); }
			operator unsigned const&() const { return reinterpret_cast<unsigned const&>(to<int>()); }

			void substantiate();

			template<typename T>
			T& to() { return static_cast<T&>(*this); }
			template<typename T>
			T const& to() const { return static_cast<T const&>(*this); }
			template<typename T>
			void assign(T const& src) 
			{
				this->substantiate();
				static_cast<T&>(*this) = src;
			}
			template<typename T>
			T const& operator =(T const& src) { this->assign(src); return static_cast<T&>(*this); }

			Element& operator =(Element const& src);

			Element& owner();
			Element const& owner() const;

			unsigned indexOfArray1D() const {
				rpg2k_assert(indexOfArray1D_ != NOT_ARRAY);
				return indexOfArray1D_;
			}
			unsigned indexOfArray2D() const {
				rpg2k_assert(indexOfArray2D_ != NOT_ARRAY);
				return indexOfArray2D_;
			}
		}; // class Element

		template< > inline
		unsigned const& Element::operator =(unsigned const& num) { (*this) = int(num); return *this; }
		template< > inline
		unsigned const& Element::to<unsigned>() const { return reinterpret_cast< unsigned const& >(to<int>()); }
		template< > inline
		unsigned& Element::to<unsigned>() { return reinterpret_cast< unsigned& >(to<int>()); }

		class BerEnum : public std::vector<unsigned>
		{
		protected:
			void init(std::istream& s);
		public:
			BerEnum() {}
			BerEnum(std::istream& s);
			BerEnum(Binary const& b);

			size_t serializedSize() const;
			std::ostream& serialize(std::ostream& s) const;
		}; // class BerEnum
	} // namespace structure
} // namespace rpg2k

#endif
