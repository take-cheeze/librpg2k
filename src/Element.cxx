#include "rpg2k/Array1D.hxx"
#include "rpg2k/Array1DWrapper.hxx"
#include "rpg2k/Array2D.hxx"
#include "rpg2k/Debug.hxx"
#include "rpg2k/Element.hxx"
#include "rpg2k/Event.hxx"
#include "rpg2k/Stream.hxx"

#include <stdexcept>


#if RPG2K_ANALYZE_AT_DESTRUCTOR
	#if RPG2K_ONLY_ANALYZE_NON_DEFINED_ELEMENT
		#define ANALYZE_SELF() \
			if(exists() && isDefined()) debug::Tracer::printTrace(*this, true)
	#else
		#define ANALYZE_SELF() \
			if(exists()) debug::Tracer::printTrace(*this, true)
	#endif
#else
	#define ANALYZE_SELF()
#endif

namespace rpg2k
{
	namespace structure
	{
		template<class T>
		bool checkSerialize(T const& result, Binary const& src)
		{
			Binary serialized = serialize(result);
			if(src != serialized) {
				debug::Tracer::printBinary(src, cerr);
				cerr << endl;
				debug::Tracer::printBinary(serialized, cerr);
				cerr << endl;

				return false;
			}
			return true;
		}

		BerEnum::BerEnum(std::istream& s)
		{
			init(s);
		}
		BerEnum::BerEnum(Binary const& b)
		{
			io::stream<io::array_source> s(b.source());
			init(s);
			rpg2k_assert(stream::isEOF(s));
		}

		void BerEnum::init(std::istream& s)
		{
			this->resize(stream::readBER(s) + 1);
			for(auto& i : *this) { i = stream::readBER(s); }
		}

		size_t BerEnum::serializedSize() const
		{
			unsigned ret = stream::berSize(size() - 1);
			for(auto const& i : *this) ret += stream::berSize(i);
			return ret;
		}
		std::ostream& BerEnum::serialize(std::ostream& s) const
		{
			stream::writeBER(s, this->size() - 1);
			for(auto const& i : *this) stream::writeBER(s, i);
			return s;
		}

		std::ostream& Element::serialize(std::ostream& s) const
		{
			if(isDefined()) switch(descriptor_->type()) {
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						return impl_.BOOST_PP_CAT(elem, data)->serialize(s);
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2kTypes)
				#undef PP_enum

				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						return (Binary() = impl_.BOOST_PP_CAT(elem, data)).serialize(s);
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum

				default: rpg2k_assert(false); return s;
			} else { return impl_.Binary_->serialize(s); }
		}
		size_t Element::serializedSize() const
		{
			if(isDefined()) switch(descriptor_->type()) {
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						return impl_.BOOST_PP_CAT(elem, data)->serializedSize();
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2kTypes)
				#undef PP_enum

				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): { \
						Binary bin; \
						bin = impl_.BOOST_PP_CAT(elem, data); \
						return bin.size(); \
					}
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum

				default: rpg2k_assert(false); return 0;
			} else { return impl_.Binary_->serializedSize(); }
		}

		void Element::init()
		{
			exists_ = false;

			if(descriptor_->hasDefault()) switch(descriptor_->type()) {
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = this->descriptor(); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum

				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = new elem(*this); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (Array1D)(Array2D))
				#undef PP_enum

				#define PP_enumNoDefault(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = new elem(); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enumNoDefault, _, (BerEnum)(Binary)(Event)(String))

				default: rpg2k_analyze_assert(false); break;
			} else switch(descriptor_->type()) {
					BOOST_PP_SEQ_FOR_EACH(PP_enumNoDefault, _, (BerEnum)(Binary)(Event)(String))
				#undef PP_enumNoDefault

				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = elem(); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum

				default: rpg2k_analyze_assert(false); break;
			}
		}
		void Element::init(Binary const& b)
		{
			exists_ = true;

			if(isDefined()) switch(descriptor_->type()) {
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = new elem(*this, b); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (Array1D)(Array2D))
				#undef PP_enum

				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = b; \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum

				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = new elem(b); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (BerEnum)(Binary)(Event)(String))
				#undef PP_enum
			} else { impl_.Binary_ = new Binary(b); }
		}
		void Element::init(std::istream& s)
		{
			exists_ = true;

			switch(descriptor_->type()) {
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = new elem(*this, s); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (Array1D)(Array2D))
				#undef PP_enum

				case ElementType::BerEnum_:
					impl_.BerEnum_ = new BerEnum(s);
					break;
				default: rpg2k_analyze_assert(false); break;
			}
		}

		Element::Element()
		: descriptor_(NULL), owner_(NULL), indexOfArray1D_(NOT_ARRAY), indexOfArray2D_(NOT_ARRAY)
		{
			init();
		}
		Element::Element(Element const& e)
		: descriptor_(e.descriptor_)
		, exists_(e.exists_), owner_(e.owner_)
		, indexOfArray1D_(e.indexOfArray1D_), indexOfArray2D_(e.indexOfArray2D_)
		{
			if(isDefined()) switch(descriptor_->type()) {
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = new elem(*e.impl_.BOOST_PP_CAT(elem, data)); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2kTypes)
				#undef PP_enum
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = descriptor().hasDefault()? descriptor() : elem(); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum
			}
		}
		Element::Element(Descriptor const& info)
		: descriptor_(&info), owner_(NULL), indexOfArray1D_(NOT_ARRAY), indexOfArray2D_(NOT_ARRAY)
		{
			init();
		}
		Element::Element(Descriptor const& info, Binary const& b)
		: descriptor_(&info), owner_(NULL), indexOfArray1D_(NOT_ARRAY), indexOfArray2D_(NOT_ARRAY)
		{
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_analyze_assert(instance_.checkSerialize(b));
			#endif

			init(b);
		}
		Element::Element(Descriptor const& info, std::istream& s)
		: descriptor_(&info), owner_(NULL), indexOfArray1D_(NOT_ARRAY), indexOfArray2D_(NOT_ARRAY)
		{
			init(s);
		}
		Element::Element(Descriptor const& info, std::istream& stream, size_t const size)
		: descriptor_(&info), owner_(NULL), indexOfArray1D_(NOT_ARRAY), indexOfArray2D_(NOT_ARRAY)
		{
			io::stream<stream::istream_range_source>
				s(stream::istream_range_source(stream, size));
			init(s);
		}

		Element::Element(Array1D const& owner, unsigned index)
		: descriptor_(
			owner.arrayDefine().find(index) != owner.arrayDefine().end()
				? owner.arrayDefine().find(index)->second : NULL)
		, owner_(&owner.toElement()), indexOfArray1D_(index)
		, indexOfArray2D_(owner.isArray2D()? owner.index() : int(NOT_ARRAY))
		{
			init();
		}
		Element::Element(Array1D const& owner, unsigned const index, std::istream& stream, size_t const size)
		: descriptor_(
			owner.arrayDefine().find(index) != owner.arrayDefine().end()
				? owner.arrayDefine().find(index)->second : NULL)
		, owner_(&owner.toElement()), indexOfArray1D_(index)
		, indexOfArray2D_(owner.isArray2D()? owner.index() : int(NOT_ARRAY))
		{
			io::stream<stream::istream_range_source>
				s(stream::istream_range_source(stream, size));
			init(s);
		}
		Element::Element(Array1D const& owner, unsigned index, Binary const& b)
		: descriptor_(
			owner.arrayDefine().find(index) != owner.arrayDefine().end()
				? owner.arrayDefine().find(index)->second : NULL)
		, owner_(&owner.toElement()), indexOfArray1D_(index)
		, indexOfArray2D_(owner.isArray2D()? owner.index() : int(NOT_ARRAY))
		{
			#if RPG2K_CHECK_AT_CONSTRUCTOR
				rpg2k_analyze_assert(instance_.checkSerialize(b));
			#endif

			init(b);
		}

		Element::~Element()
		{
			ANALYZE_SELF();

			if(isDefined()) switch(descriptor_->type()) {
				#define PP_enum(r, data, elem) case ElementType::BOOST_PP_CAT(elem, data): delete impl_.BOOST_PP_CAT(elem, data); break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2kTypes)
				#undef PP_enum
				#define PP_enum(r, data, elem) case ElementType::BOOST_PP_CAT(elem, data):
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum
					break;
				default: rpg2k_assert(false); break;
			} else { delete impl_.Binary_; }
		}

		Element& Element::operator =(Element const& src)
		{
			if(isDefined()) switch(descriptor_->type()) {
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						(*impl_.BOOST_PP_CAT(elem, data)) = (*src.impl_.BOOST_PP_CAT(elem, data)); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2kTypes)
				#undef PP_enum
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = src.impl_.BOOST_PP_CAT(elem, data); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum
				default: rpg2k_assert(false); break;
			} else (*impl_.Binary_) = (*src.impl_.Binary_);

			return *this;
		}

		Binary Element::serialize() const
		{
			return structure::serialize(*this);
		}

		Element const& Element::owner() const
		{
			rpg2k_assert(hasOwner());
			return *owner_;
		}
		Element& Element::owner()
		{
			rpg2k_assert(hasOwner());
			return *owner_;
		}

		void Element::substantiate()
		{
			if(descriptor_ && descriptor_->hasDefault()) switch(descriptor_->type()) {
				#define PP_enum(r, data, elem) \
					case ElementType::BOOST_PP_CAT(elem, data): \
						if((impl_.BOOST_PP_CAT(elem, data)) == static_cast<elem const&>(*descriptor_)) { \
							exists_ = false; \
							return; \
						} \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basicTypes)
				#undef PP_enum

				#define PP_enum(r, data, elem) case ElementType::BOOST_PP_CAT(elem, data):
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2kTypes)
				#undef PP_enum
					break;
				default: rpg2k_assert(false);
			}

			exists_ = true;

			if(hasOwner()) {
				owner_->substantiate();

				if(owner().descriptor().type() == ElementType::Array2D_) {
					(*owner().impl_.Array2D_)[indexOfArray2D()].substantiate();
				}
			}
		}

		Descriptor const& Element::descriptor() const
		{
			rpg2k_assert(isDefined());
			return *descriptor_;
		}

		#define PP_castOperator(r, data, elem) \
			Element::operator elem const&() const \
			{ \
				rpg2k_assert(this->isDefined()); \
				rpg2k_assert(descriptor_->type() == ElementType::BOOST_PP_CAT(elem, data)); \
				rpg2k_assert(impl_.BOOST_PP_CAT(elem, data)); \
				return *impl_.BOOST_PP_CAT(elem, data); \
			} \
			Element::operator elem&() \
			{ \
				rpg2k_assert(this->isDefined()); \
				rpg2k_assert(descriptor_->type() == ElementType::BOOST_PP_CAT(elem, data)); \
				rpg2k_assert(impl_.BOOST_PP_CAT(elem, data)); \
				return *impl_.BOOST_PP_CAT(elem, data); \
			}
		BOOST_PP_SEQ_FOR_EACH(PP_castOperator, _, PP_rpg2kTypes)
		#undef PP_castOperator

		#define PP_castOperator(r, data, elem) \
			Element::operator elem const&() const \
			{ \
				rpg2k_assert(this->isDefined()); \
				rpg2k_assert(descriptor_->type() == ElementType::BOOST_PP_CAT(elem, data)); \
				rpg2k_assert(this->exists_ || descriptor_->hasDefault()); \
				return impl_.BOOST_PP_CAT(elem, data); \
			} \
			Element::operator elem&() \
			{ \
				rpg2k_assert(this->isDefined()); \
				rpg2k_assert(descriptor_->type() == ElementType::BOOST_PP_CAT(elem, data)); \
				rpg2k_assert(this->exists_ || descriptor_->hasDefault()); \
				return impl_.BOOST_PP_CAT(elem, data); \
			}
		BOOST_PP_SEQ_FOR_EACH(PP_castOperator, _, PP_basicTypes)
		#undef PP_castOperator
	} // namespace structure
} // namespace rpg2k
