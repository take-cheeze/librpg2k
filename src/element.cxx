#include "rpg2k/array1d.hxx"
#include "rpg2k/array1d_wrapper.hxx"
#include "rpg2k/array2d.hxx"
#include "rpg2k/debug.hxx"
#include "rpg2k/element.hxx"
#include "rpg2k/event.hxx"
#include "rpg2k/stream.hxx"

#include <boost/foreach.hpp>

#include <stdexcept>


namespace rpg2k {
	namespace structure {
		template<class T>
		bool check_serialize(T const& result, binary const& src)
		{
			binary serialized = serialize(result);
			if(src != serialized) {
				debug::tracer::print_binary(src, cerr);
				cerr << endl;
				debug::tracer::print_binary(serialized, cerr);
				cerr << endl;

				return false;
			}
			return true;
		}

		ber_enum::ber_enum(std::istream& s)
		{
			init(s);
		}
		ber_enum::ber_enum(binary const& b)
		{
			io::stream<io::array_source> s(b.source());
			init(s);
			rpg2k_assert(stream::is_eof(s));
		}

		void ber_enum::init(std::istream& s)
		{
			this->resize(stream::read_ber(s) + 1);
			BOOST_FOREACH(value_type& i, *this) i = stream::read_ber(s);
		}

		size_t ber_enum::serialized_size() const
		{
			unsigned ret = stream::ber_size(size() - 1);
			BOOST_FOREACH(value_type const& i, *this) ret += stream::ber_size(i);
			return ret;
		}
		std::ostream& ber_enum::serialize(std::ostream& s) const
		{
			stream::write_ber(s, this->size() - 1);
			BOOST_FOREACH(value_type const& i, *this) stream::write_ber(s, i);
			return s;
		}

		std::ostream& element::serialize(std::ostream& s) const
		{
			if(is_defined()) switch(descriptor_->type) {
				#define PP_enum(r, data, elem) \
					case element_type::BOOST_PP_CAT(elem, data): \
						return impl_.BOOST_PP_CAT(elem, data)->serialize(s);
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types)
				#undef PP_enum

				#define PP_enum(r, data, elem) \
					case element_type::BOOST_PP_CAT(elem, data): \
						return (binary() = impl_.BOOST_PP_CAT(elem, data)).serialize(s);
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
				#undef PP_enum

				default: rpg2k_assert(false); return s;
			} else { return impl_.binary_->serialize(s); }
		}
		size_t element::serialized_size() const
		{
			if(is_defined()) switch(descriptor_->type) {
				#define PP_enum(r, data, elem) \
					case element_type::BOOST_PP_CAT(elem, data): \
						return impl_.BOOST_PP_CAT(elem, data)->serialized_size();
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types)
				#undef PP_enum

				#define PP_enum(r, data, elem) \
					case element_type::BOOST_PP_CAT(elem, data): { \
						binary bin; \
						bin = impl_.BOOST_PP_CAT(elem, data); \
						return bin.size(); \
					}
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
				#undef PP_enum

				default: rpg2k_assert(false); return 0;
			} else { return impl_.binary_->serialized_size(); }
		}

		void element::init()
		{
			exists_ = false;

			if(descriptor_->has_default()) switch(descriptor_->type) {
#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = this->definition(); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum

#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = new elem(*this); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (array1d)(array2d))
#undef PP_enum

#define PP_enum_no_default(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = new elem(); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum_no_default, _, (ber_enum)(binary)(event)(string))

				default: rpg2k_analyze_assert(false); break;
			} else switch(descriptor_->type) {
				BOOST_PP_SEQ_FOR_EACH(PP_enum_no_default, _, (ber_enum)(binary)(event)(string))
#undef PP_enum_no_default

#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = elem(); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum

				default: rpg2k_analyze_assert(false); break;
			}
		}
		void element::init(binary const& b)
		{
			exists_ = true;

			if(is_defined()) switch(descriptor_->type) {
#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = new elem(*this, b); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (array1d)(array2d))
#undef PP_enum

#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = b; \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum

#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = new elem(b); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (ber_enum)(binary)(event)(string))
#undef PP_enum
			} else { impl_.binary_ = new binary(b); }
		}
		void element::init(std::istream& s)
		{
			exists_ = true;

			switch(descriptor_->type) {
#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = new elem(*this, s); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (array1d)(array2d))
#undef PP_enum

				case element_type::ber_enum_:
					impl_.ber_enum_ = new ber_enum(s);
					break;
				default: rpg2k_analyze_assert(false); break;
			}
		}

		element::element()
		: descriptor_(NULL), owner_(NULL)
		, index_of_array1d_(NOT_ARRAY), index_of_array2d_(NOT_ARRAY)
		{
			init();
		}
		element::element(element const& e)
		: descriptor_(e.descriptor_)
		, exists_(e.exists_), owner_(e.owner_)
		, index_of_array1d_(e.index_of_array1d_), index_of_array2d_(e.index_of_array2d_)
		{
			if(is_defined()) switch(descriptor_->type) {
#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = new elem(*e.impl_.BOOST_PP_CAT(elem, data)); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types)
#undef PP_enum

#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		impl_.BOOST_PP_CAT(elem, data) = definition().has_default()? definition() : elem(); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum
			}
		}

		element::element(descriptor const& info)
		: descriptor_(&info), owner_(NULL)
		, index_of_array1d_(NOT_ARRAY), index_of_array2d_(NOT_ARRAY)
		{
			init();
		}
		element::element(descriptor const& info, binary const& b)
		: descriptor_(&info), owner_(NULL)
		, index_of_array1d_(NOT_ARRAY), index_of_array2d_(NOT_ARRAY)
		{
			init(b);

			rpg2k_analyze_assert(check_serialize(*this, b));
		}
		element::element(descriptor const& info, std::istream& s)
		: descriptor_(&info), owner_(NULL)
		, index_of_array1d_(NOT_ARRAY), index_of_array2d_(NOT_ARRAY)
		{
			init(s);
		}

		element::element(array1d& owner, unsigned index)
		: descriptor_(
			owner.array_define.find(index) != owner.array_define.end()
				? owner.array_define.find(index)->second : NULL)
		, owner_(&owner.to_element()), index_of_array1d_(index)
		, index_of_array2d_(owner.is_array2d()? int(owner.index()) : NOT_ARRAY)
		{
			init();
		}
		element::element(array1d& owner, unsigned index, binary const& b)
		: descriptor_(
			owner.array_define.find(index) != owner.array_define.end()
				? owner.array_define.find(index)->second : NULL)
		, owner_(&owner.to_element()), index_of_array1d_(index)
		, index_of_array2d_(owner.is_array2d()? int(owner.index()) : NOT_ARRAY)
		{
			init(b);

			rpg2k_analyze_assert(check_serialize(*this, b));
		}

		element::~element()
		{
			if(exists()) debug::tracer::print_trace(*this, true);

			if(is_defined()) switch(descriptor_->type) {
#define PP_enum(r, data, elem) \
	case element_type::BOOST_PP_CAT(elem, data): \
		delete impl_.BOOST_PP_CAT(elem, data); \
		break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types)
#undef PP_enum
#define PP_enum(r, data, elem) case element_type::BOOST_PP_CAT(elem, data):
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum
					break;
				default: rpg2k_assert(false); break;
			} else { delete impl_.binary_; }
		}

		element& element::operator =(element const& src)
		{
			if(is_defined()) switch(descriptor_->type) {
				#define PP_enum(r, data, elem) \
					case element_type::BOOST_PP_CAT(elem, data): \
						(*impl_.BOOST_PP_CAT(elem, data)) = (*src.impl_.BOOST_PP_CAT(elem, data)); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types)
				#undef PP_enum

				#define PP_enum(r, data, elem) \
					case element_type::BOOST_PP_CAT(elem, data): \
						impl_.BOOST_PP_CAT(elem, data) = src.impl_.BOOST_PP_CAT(elem, data); \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
				#undef PP_enum
				default: rpg2k_assert(false); break;
			} else (*impl_.binary_) = (*src.impl_.binary_);

			return *this;
		}

		binary element::serialize() const
		{
			return structure::serialize(*this);
		}

		element const& element::owner() const
		{
			rpg2k_assert(has_owner());
			return *owner_;
		}
		element& element::owner()
		{
			rpg2k_assert(has_owner());
			return *owner_;
		}

		void element::substantiate()
		{
			if(descriptor_ && descriptor_->has_default()) switch(descriptor_->type) {
				#define PP_enum(r, data, elem) \
					case element_type::BOOST_PP_CAT(elem, data): \
						if((impl_.BOOST_PP_CAT(elem, data)) == static_cast<elem const&>(*descriptor_)) { \
							exists_ = false; \
							return; \
						} \
						break;
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
				#undef PP_enum

				#define PP_enum(r, data, elem) case element_type::BOOST_PP_CAT(elem, data):
				BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types)
				#undef PP_enum
					break;
				default: rpg2k_assert(false);
			}

			exists_ = true;

			if(has_owner()) {
				owner_->substantiate();

				if(owner().definition().type == element_type::array2d_) {
					(*owner().impl_.array2d_)[index_of_array2d()].substantiate();
				}
			}
		}

		descriptor const& element::definition() const
		{
			rpg2k_assert(is_defined());
			return *descriptor_;
		}

#define PP_cast_operator(r, data, elem) \
	element::operator elem const&() const \
	{ \
		rpg2k_assert(this->is_defined()); \
		rpg2k_assert(descriptor_->type == element_type::BOOST_PP_CAT(elem, data)); \
		rpg2k_assert(impl_.BOOST_PP_CAT(elem, data)); \
		return *impl_.BOOST_PP_CAT(elem, data); \
	} \
	element::operator elem&() \
	{ \
		rpg2k_assert(this->is_defined()); \
		rpg2k_assert(descriptor_->type == element_type::BOOST_PP_CAT(elem, data)); \
		rpg2k_assert(impl_.BOOST_PP_CAT(elem, data)); \
		return *impl_.BOOST_PP_CAT(elem, data); \
	}
		BOOST_PP_SEQ_FOR_EACH(PP_cast_operator, _, PP_rpg2k_types)
#undef PP_cast_operator

#define PP_cast_operator(r, data, elem) \
	element::operator elem const&() const \
	{ \
		rpg2k_assert(this->is_defined()); \
		rpg2k_assert(descriptor_->type == element_type::BOOST_PP_CAT(elem, data)); \
		rpg2k_assert(this->exists_ || descriptor_->has_default()); \
		return impl_.BOOST_PP_CAT(elem, data); \
	} \
	element::operator elem&() \
	{ \
		rpg2k_assert(this->is_defined()); \
		rpg2k_assert(descriptor_->type == element_type::BOOST_PP_CAT(elem, data)); \
		rpg2k_assert(this->exists_ || descriptor_->has_default()); \
		return impl_.BOOST_PP_CAT(elem, data); \
	}
		BOOST_PP_SEQ_FOR_EACH(PP_cast_operator, _, PP_basic_types)
#undef PP_cast_operator
	} // namespace structure
} // namespace rpg2k
