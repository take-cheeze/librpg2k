#include "rpg2k/array1d.hxx"
#include "rpg2k/array1d_wrapper.hxx"
#include "rpg2k/array2d.hxx"
#include "rpg2k/debug.hxx"
#include "rpg2k/element.hxx"
#include "rpg2k/event.hxx"
#include "rpg2k/stream.hxx"

#include "picojson.h"

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <stdexcept>


namespace rpg2k {
namespace structure {
/*
  element(descriptor const& info, picojson::value const& v);
*/

void element::assign(picojson::value const& v) {
  rpg2k_assert(is_defined());

  switch(definition().type) {
    case element_type::string_:
      (*this) = string(v.get<std::string>());
      break;
    case element_type::int_:
      (*this) = int(v.get<double>());
      break;
    case element_type::double_:
      (*this) = v.get<double>();
      break;
    case element_type::bool_:
      (*this) = v.get<bool>();
      break;

    case element_type::array1d_: {
      array1d& self = to_array1d();
      BOOST_FOREACH(picojson::object::value_type const& i, v.get<picojson::object>()) {
        self[i.first.get().c_str()].assign(i.second);
      }
    } break;

    case element_type::array2d_: {
      array2d& self = to_array2d();
      BOOST_FOREACH(picojson::object::value_type const& i, v.get<picojson::object>()) {
        BOOST_FOREACH(picojson::object::value_type const& name,
                      i.second.get<picojson::object>())
        {
          self[boost::lexical_cast<int>(i.first.get())][name.first.get().c_str()].assign(name.second);
        }
      }
    } break;

    case element_type::event_: {
      picojson::array const& in = v.get<picojson::array>();
      event out;
      BOOST_FOREACH(picojson::value const& i, in) {
        instruction inst_out;

        inst_out.nest = i.get("nest").get<double>();
        inst_out.code = i.get("code").get<double>();
        inst_out.string_argument = string(i.get("string_argument").get<std::string>());

        BOOST_FOREACH(picojson::value const& inst_arg,
                      i.get("arguments").get<picojson::array>())
        {
          inst_out.push_back(inst_arg.get<double>());
        }

        out.push_back(inst_out);
      }
      (*this) = out;
    } break;

#define PP_enum(r, data, elem) \
      case element_type::BOOST_PP_CAT(elem, data): {             \
        picojson::array const& in = v.get<picojson::array>();    \
        elem out;                                                \
        out.reserve(in.size());                                  \
        BOOST_FOREACH(picojson::value const& i, in) {            \
          out.push_back(i.get<double>());                        \
        }                                                        \
        (*this) = out;                                           \
      } break;                                                   \

      BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (ber_enum)(binary) PP_array_types)
#undef PP_enum

    default: rpg2k_assert(false);
  }
}

static picojson::value array1d_to_json(array1d const& ary) {
  picojson::object ret;
  descriptor const& def = ary.to_element().definition();
  for(array1d::const_iterator i = ary.begin(); i != ary.end(); ++i) {
    ret[picojson::object_key(def.index_to_name(i->first).to_system())] = i->second->to_json();
  }
  return picojson::value(ret);
}

picojson::value element::to_json() const {
  if(is_defined()) switch(definition().type) {
      case element_type::string_:
        return picojson::value(to_string().to_system());
      case element_type::int_:
        return picojson::value(double(to_int()));
      case element_type::double_:
        return picojson::value(to_double());
      case element_type::bool_:
        return picojson::value(to_bool());

      case element_type::array1d_:
        return array1d_to_json(to_array1d());
      case element_type::array2d_: {
        picojson::object ret;
        array2d const& ary = to_array2d();
        for(array2d::const_iterator i = ary.begin(); i != ary.end(); ++i) {
          ret[picojson::object_key((boost::format("%d") % i->first).str())] = array1d_to_json(*(i->second));
        }
        return picojson::value(ret);
      }

      case element_type::event_: {
        picojson::array ret;
        BOOST_FOREACH(instruction const& i, to_event()) {
          picojson::object inst;
          picojson::array args;
          BOOST_FOREACH(int a, i) { args.push_back(picojson::value(double(a))); }

          inst[picojson::object_key("code")] = picojson::value(double(i.code));
          inst[picojson::object_key("nest")] = picojson::value(double(i.nest));
          inst[picojson::object_key("string_argument")] = picojson::value(i.string_argument.to_system());
          inst[picojson::object_key("arguments")] = picojson::value(args);

          ret.push_back(picojson::value(inst));
        }
        return picojson::value(ret);
      }

#define PP_enum(r, data, elem)                            \
        case element_type::BOOST_PP_CAT(elem, data): {    \
          picojson::array ret;                            \
          BOOST_FOREACH(int i, BOOST_PP_CAT(to_, elem)()) \
          { ret.push_back(picojson::value(double(i))); }  \
          return picojson::value(ret);                    \
        }
        BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (ber_enum)(binary) PP_array_types)
#undef PP_enum

            } else {
    picojson::array ret;
    BOOST_FOREACH(int i, *impl_.binary_)
    { ret.push_back(picojson::value(double(i))); }
    return picojson::value(ret);
  }

  rpg2k_assert(false);
  return picojson::value();
}

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
#define PP_enum(r, data, elem)                                \
      case element_type::BOOST_PP_CAT(elem, data):            \
        return impl_.BOOST_PP_CAT(elem, data)->serialize(s);
      BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types)
#undef PP_enum

#define PP_enum(r, data, elem)                                          \
					case element_type::BOOST_PP_CAT(elem, data):                  \
						return (binary() = impl_.BOOST_PP_CAT(elem, data)).serialize(s);
          BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum


#define PP_enum(r, data, elem)                                          \
              case element_type::BOOST_PP_CAT(elem, data):              \
                return (binary().assign(*impl_.BOOST_PP_CAT(elem, data))).serialize(s);
              BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_array_types)
#undef PP_enum

      default: rpg2k_assert(false); return s;
    } else { return impl_.binary_->serialize(s); }
}
size_t element::serialized_size() const
{
  if(is_defined()) switch(descriptor_->type) {
#define PP_enum(r, data, elem)                                    \
      case element_type::BOOST_PP_CAT(elem, data):                \
        return impl_.BOOST_PP_CAT(elem, data)->serialized_size();
      BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types)
#undef PP_enum

#define PP_enum(r, data, elem)                            \
					case element_type::BOOST_PP_CAT(elem, data): {  \
						binary bin;                                   \
						bin = impl_.BOOST_PP_CAT(elem, data);         \
						return bin.size();                            \
					}
          BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum

#define PP_enum(r, data, elem)                                          \
              case element_type::BOOST_PP_CAT(elem, data):              \
                return sizeof(elem::value_type) * impl_.BOOST_PP_CAT(elem, data)->size();
              BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_array_types)
#undef PP_enum

      default: rpg2k_assert(false); return 0;
    } else { return impl_.binary_->serialized_size(); }
}

void element::init()
{
  exists_ = false;

  if(descriptor_->has_default()) switch(descriptor_->type) {
#define PP_enum(r, data, elem)                                \
      case element_type::BOOST_PP_CAT(elem, data):            \
        impl_.BOOST_PP_CAT(elem, data) = this->definition();  \
        break;
      BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum

#define PP_enum(r, data, elem)                                \
          case element_type::BOOST_PP_CAT(elem, data):        \
            impl_.BOOST_PP_CAT(elem, data) = new elem(*this); \
            break;
          BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (array1d)(array2d))
#undef PP_enum

#define PP_enum_no_default(r, data, elem)                     \
              case element_type::BOOST_PP_CAT(elem, data):    \
                impl_.BOOST_PP_CAT(elem, data) = new elem();  \
                break;
              BOOST_PP_SEQ_FOR_EACH(PP_enum_no_default, _, (ber_enum)(binary)(event)(string))

      default: rpg2k_analyze_assert(false); break;
    } else switch(descriptor_->type) {
      BOOST_PP_SEQ_FOR_EACH(PP_enum_no_default, _, (ber_enum)(binary)(event)(string) PP_array_types)
#undef PP_enum_no_default

#define PP_enum(r, data, elem)                          \
          case element_type::BOOST_PP_CAT(elem, data):  \
            impl_.BOOST_PP_CAT(elem, data) = elem();    \
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
#define PP_enum(r, data, elem)                                \
      case element_type::BOOST_PP_CAT(elem, data):            \
        impl_.BOOST_PP_CAT(elem, data) = new elem(*this, b);  \
        break;
      BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (array1d)(array2d))
#undef PP_enum

#define PP_enum(r, data, elem)                          \
          case element_type::BOOST_PP_CAT(elem, data):  \
            impl_.BOOST_PP_CAT(elem, data) = b;         \
            break;
          BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum

#define PP_enum(r, data, elem)                                \
              case element_type::BOOST_PP_CAT(elem, data):    \
                impl_.BOOST_PP_CAT(elem, data) = new elem(b); \
                break;
              BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (ber_enum)(binary)(event)(string))
#undef PP_enum

#define PP_enum(r, data, elem)                                          \
                  case element_type::BOOST_PP_CAT(elem, BOOST_PP_CAT(_array, data)): \
                    impl_.BOOST_PP_CAT(elem, BOOST_PP_CAT(_array, data)) = new BOOST_PP_CAT(elem, _array) \
                        (b.to_vector<BOOST_PP_CAT(elem, _t)>());        \
                    break;
                  BOOST_PP_SEQ_FOR_EACH(PP_enum, _, (int32)(int16)(int8))
#undef PP_enum

                      } else { impl_.binary_ = new binary(b); }
}
void element::init(std::istream& s)
{
  exists_ = true;

  switch(descriptor_->type) {
#define PP_enum(r, data, elem)                              \
    case element_type::BOOST_PP_CAT(elem, data):            \
      impl_.BOOST_PP_CAT(elem, data) = new elem(*this, s);  \
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
#define PP_enum(r, data, elem)                                          \
      case element_type::BOOST_PP_CAT(elem, data):                      \
        impl_.BOOST_PP_CAT(elem, data) = new elem(*e.impl_.BOOST_PP_CAT(elem, data)); \
        break;
      BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types PP_array_types)
#undef PP_enum

#define PP_enum(r, data, elem)                                          \
          case element_type::BOOST_PP_CAT(elem, data):                  \
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
  if(is_defined()) switch(descriptor_->type) {
#define PP_enum(r, data, elem)                      \
      case element_type::BOOST_PP_CAT(elem, data):  \
        delete impl_.BOOST_PP_CAT(elem, data);      \
        break;
      BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types PP_array_types)
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
#define PP_enum(r, data, elem)                                          \
      case element_type::BOOST_PP_CAT(elem, data):                      \
        (*impl_.BOOST_PP_CAT(elem, data)) = (*src.impl_.BOOST_PP_CAT(elem, data)); \
        break;
      BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types PP_array_types)
#undef PP_enum

#define PP_enum(r, data, elem)                                          \
					case element_type::BOOST_PP_CAT(elem, data):                  \
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
#define PP_enum(r, data, elem)                                          \
      case element_type::BOOST_PP_CAT(elem, data):                      \
        if((impl_.BOOST_PP_CAT(elem, data)) == static_cast<elem const&>(*descriptor_)) { \
          exists_ = false;                                              \
          return;                                                       \
        }                                                               \
        break;
      BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_basic_types)
#undef PP_enum

#define PP_enum(r, data, elem) case element_type::BOOST_PP_CAT(elem, data):
          BOOST_PP_SEQ_FOR_EACH(PP_enum, _, PP_rpg2k_types PP_array_types)
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

#define PP_cast_operator(r, data, elem)                                 \
	element::operator elem const&() const                                 \
	{                                                                     \
		rpg2k_assert(this->is_defined());                                   \
		rpg2k_assert(descriptor_->type == element_type::BOOST_PP_CAT(elem, data)); \
		rpg2k_assert(impl_.BOOST_PP_CAT(elem, data));                       \
		return *impl_.BOOST_PP_CAT(elem, data);                             \
	}                                                                     \
	element::operator elem&()                                             \
	{                                                                     \
		rpg2k_assert(this->is_defined());                                   \
		rpg2k_assert(descriptor_->type == element_type::BOOST_PP_CAT(elem, data)); \
		rpg2k_assert(impl_.BOOST_PP_CAT(elem, data));                       \
		return *impl_.BOOST_PP_CAT(elem, data);                             \
	}
BOOST_PP_SEQ_FOR_EACH(PP_cast_operator, _, PP_rpg2k_types PP_array_types)
#undef PP_cast_operator

#define PP_cast_operator(r, data, elem)                                 \
	element::operator elem const&() const                                 \
	{                                                                     \
		rpg2k_assert(this->is_defined());                                   \
		rpg2k_assert(descriptor_->type == element_type::BOOST_PP_CAT(elem, data)); \
		rpg2k_assert(this->exists_ || descriptor_->has_default());          \
		return impl_.BOOST_PP_CAT(elem, data);                              \
	}                                                                     \
	element::operator elem&()                                             \
	{                                                                     \
		rpg2k_assert(this->is_defined());                                   \
		rpg2k_assert(descriptor_->type == element_type::BOOST_PP_CAT(elem, data)); \
		rpg2k_assert(this->exists_ || descriptor_->has_default());          \
		return impl_.BOOST_PP_CAT(elem, data);                              \
	}
BOOST_PP_SEQ_FOR_EACH(PP_cast_operator, _, PP_basic_types)
#undef PP_cast_operator

} // namespace structure
} // namespace rpg2k
