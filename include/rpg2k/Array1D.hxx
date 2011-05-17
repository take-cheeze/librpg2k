#ifndef _INC__RPG2K__MODEL__ARRAY_1D_HPP
#define _INC__RPG2K__MODEL__ARRAY_1D_HPP

#include <map>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include "Descriptor.hxx"
#include "Structure.hxx"


namespace rpg2k
{
	namespace structure
	{
		class Array2D;
		class Element;
		class Music;
		class EventState;
		class Sound;

		typedef boost::ptr_unordered_map<unsigned, Element> BaseOfArray1D;

		class Array1D : public BaseOfArray1D
		{
		private:
			ArrayDefine arrayDefine_;

			Element* const this_;

			bool exists_;
			Array2D* const owner_;
			unsigned const index_;

			enum { ARRAY_1D_END = 0, };
		protected:
			Array1D();

			void init(std::istream& s);
		public:
			Array1D(Array1D const& array);

			Array1D(ArrayDefine info);
			Array1D(ArrayDefine info, std::istream& s);
			Array1D(ArrayDefine info, std::istream& s, size_t size);
			Array1D(ArrayDefine info, Binary const& b);

			Array1D(Element& e);
			Array1D(Element& e, std::istream& s);
			Array1D(Element& e, std::istream& s, size_t size);
			Array1D(Element& e, Binary const& b);

			Array1D(Array2D& owner, unsigned index);
			Array1D(Array2D& owner, unsigned index, std::istream& f);

			Array1D const& operator =(Array1D const& src);

			bool isArray2D() const { return owner_ != NULL; }
			unsigned const& index() const;
			Array2D& owner();
			Array2D const& owner() const;

			Element& operator [](unsigned index);
			Element const& operator [](unsigned index) const;

			Element& operator [](char const* index);
			Element const& operator [](char const* index) const;

			bool exists() const;
			bool exists(unsigned index) const;

			unsigned count() const;
			size_t serializedSize() const;
			std::ostream& serialize(std::ostream& s) const;

			void substantiate();

			ArrayDefine arrayDefine() const { return arrayDefine_; }

			bool isElement() const;
			Element& toElement() const;
		}; // class Array1D
	} // namespace structure
} // namespace rpg2k

#endif
