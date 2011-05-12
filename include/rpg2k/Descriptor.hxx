#ifndef _INC__RPG2K__STRUCTURE__DESCRIPTOR_HPP
#define _INC__RPG2K__STRUCTURE__DESCRIPTOR_HPP

#include "Define.hxx"
#include "Singleton.hxx"

#include <unordered_map>

#include <boost/bimap.hpp>
#include <boost/noncopyable.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>
#include <boost/scoped_ptr.hpp>


namespace rpg2k
{
	class Binary;

	namespace structure
	{
		class Array1D;
		class Array2D;
		class BerEnum;
		class Element;
		class Event;

		class EventState;
		class Music;
		class Sound;

		class Descriptor;
		typedef boost::ptr_unordered_map<unsigned, Descriptor> ArrayDefineType;
		typedef ArrayDefineType const& ArrayDefine;
		typedef std::unique_ptr<ArrayDefineType> ArrayDefinePointer;

		#define PP_basicTypes \
			(int) \
			(bool) \
			(double) \

		#define PP_rpg2kTypes \
			(Array1D) \
			(Array2D) \
			(BerEnum) \
			(Binary) \
			(String) \
			(Event) \

		#define PP_allTypes PP_basicTypes PP_rpg2kTypes

		class ElementType : public ConstSingleton<ElementType>
		{
			friend class ConstSingleton<ElementType>;
		public:
			#define PP_enum(r, data, elem) BOOST_PP_CAT(elem, data),
			enum Enum { BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_allTypes) };
			#undef PP_enum
			Enum toEnum(String const& name) const;
			String const& toString(Enum e) const;
		private:
			ElementType();

			typedef boost::bimap<Enum, String> Table;
			Table table_;
		}; // class ElementType
		class Descriptor
		{
		public:
			Descriptor(Descriptor const& src);
			Descriptor(String const& type);
			Descriptor(String const& type, String const& val);

			typedef std::unordered_map<String, int> ArrayTable;
			Descriptor(String const& type
			, ArrayDefinePointer def, std::unique_ptr<ArrayTable> table);

			~Descriptor();

			#define PP_castOperator(r, data, elem) \
				operator elem const&() const;
			BOOST_PP_SEQ_FOR_EACH(PP_castOperator, , PP_basicTypes)
			#undef PP_castOperator
			operator String const&() const;
			operator unsigned const&() const
			{
				return reinterpret_cast<unsigned const&>(static_cast<int const&>(*this));
			}
			ArrayDefine arrayDefine() const;
			ArrayTable const& arrayTable() const { return *arrayTable_; }

			String const& typeName() const;
			ElementType::Enum type() const { return type_; }

			bool hasDefault() const { return hasDefault_; }
		private:
			ElementType::Enum const type_;
			bool const hasDefault_;

			union {
				#define PP_enum(r, data, elem) elem BOOST_PP_CAT(elem, data);
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum
				String const* String_;
				boost::ptr_unordered_map<unsigned, Descriptor>* arrayDefine;
			} impl_;

			boost::scoped_ptr<ArrayTable> const arrayTable_;
		}; // class Descriptor
	} // namespace structure
} // namespace rpg2k

#endif // _INC__RPG2K__STRUCTURE__DESCRIPTOR_HPP
